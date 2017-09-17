/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "connection/ConnectionManager.h"
#include "spark_wiring_wifi.h"
#include "spark_wiring_ipaddress.h"
#include "spark_wiring_tcpclient.h"
#include "utils/picojson.h"
#include "utils/debug.h"
#include <string>

using namespace picojson;

connection::ConnectionManager::ConnectionManager() {

}

void connection::ConnectionManager::init() {
    spark::WiFi.on();

    // no need for password, home WiFi network satefy is MAC address based
    spark::WiFi.setCredentials("lsmx49");
}

void connection::ConnectionManager::connectToNetwork() {
    spark::WiFi.on();
    spark::WiFi.connect();

    while (!spark::WiFi.ready()) {
        if (!spark::WiFi.connecting()) {
            spark::WiFi.connect();
        }
        Particle.process();
        delay(300);
    }
    debug::println("CONN | Wifi status: %d", spark::WiFi.ready());
}

std::string connection::ConnectionManager::getIpAddress() {
    IPAddress ip = spark::WiFi.localIP();
    char buffer[40];

    // need to call directly Particle process to get really addres...
    Particle.process();
    
    sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return std::string(buffer);
}

std::string connection::ConnectionManager::getMACAddress() {
    uint8_t mac[6];
    spark::WiFi.macAddress(mac);
    char buffer[40];

    // need to call directly Particle process to get really addres...
    Particle.process();

    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(buffer);
}

void connection::ConnectionManager::addMessageHandler(const MessageHandler& handler) {
    handlers.push_back(handler);
}

void connection::ConnectionManager::startTcpServer(uint16_t port) {
    server = std::make_shared<TCPServer>(port);
    server->begin();
    Thread("server", std::bind(&connection::ConnectionManager::tcp_server_worker, this), 9);
}

void connection::ConnectionManager::send_nack(const std::string& message, TCPClient& client) {
    picojson::object response;
    response["message"] = value(message);
    response["success"] = value(false);
    client.printf("%s\n", value(response).serialize().c_str());
    client.flush();
}

namespace connection {

    template<typename T>
    void ConnectionManager::send_ack(const std::string& message, T new_value, TCPClient& client) {
        picojson::object response;
        response["message"] = picojson::value(message);
        response["success"] = picojson::value(true);
        response["new_value"] = picojson::value(new_value);
        client.printf("%s\n", picojson::value(response).serialize().c_str());
        client.flush();
    }

    template<>
    picojson::value ConnectionManager::parse_message(TCPClient& client, bool* success) {
        char buffer[1024];
        // then receive message
        memset(buffer, 0, sizeof (buffer));
        int i = 0;
        while (client.available()) {
            buffer[i++] = (char) client.read();
        }

        debug::println("SERVER | Raw message: %s", buffer);

        // interpret message
        std::string json(buffer);
        value v;
        std::string err = parse(v, json);

        // if there was any error message, debug it
        if (!err.empty()) {
            sprintf(buffer, "Error occur during receiving JSON: %s", err.c_str());
            debug::println("SERVER | %s", buffer);
            send_nack(buffer, client);
            *success = false;
        }

        *success = true;
        return v;
    }

};

void connection::ConnectionManager::tcp_server_worker() {

    debug::println("SERVER | Thread started.");

    TCPClient client;
    client.setTimeout(5000);

    while (true) {

        if (client.connected()) {

            // if no message received yet, continue
            if (client.available() == 0) {
                continue;
            }

            debug::println("SERVER | Client connected.");

            bool close_socket = false;
            bool parse_succeeded = false;
            value json_message = parse_message<value>(client, &parse_succeeded);
            if (!parse_succeeded) {
                debug::println("SERVER | Message parsing failed.");
                continue;
            }

            // if received message is not an JSON object, halt
            if (!json_message.is<object>()) {
                debug::println("SERVER | Received message is not a JSON object.");
                send_nack("Received message is not a JSON object.", client);
                continue;
            }

            // check all message handler
            debug::println("SERVER | Received:");
            const value::object& message = json_message.get<object>();
            for (auto field : message) {

                debug::println("         %s: %s", field.first.c_str(), field.second.to_str().c_str());
                if (field.first == "close_socket" && field.second.get<bool>()) {
                    close_socket = true;
                    continue;
                }

                // all message handler will test if need to be called
                for (auto handler : handlers) {
                    if (handler.checkField(field.first)) {
                        utils::Message data;
                        if (field.second.is<double>()) {
                            double val = field.second.get<double>();
                            // if the field is containing integer number instead of a floating number,
                            // it still stored as a double, therefore we need to check
                            if (val != (int) val) {
                                data = val;
                            } else {
                                data = (int)val;
                            }
                            send_ack("Message processed.", val, client);


                        }
                        if (field.second.is<bool>()) {
                            data = field.second.get<bool>();
                            send_ack("Message processed.", (bool)data, client);
                        }
                        handler.callback(data);
                    }
                }
            }

            if (close_socket) {
                debug::println("         Disconnect client");
                send_ack<const char*>("Closing socket.", "disconnected", client);
                client.flush();
                client.stop();
            }



        } else {
            // if no client is yet connected, check for a new connection
            client = server->available();
        }

        delay(100);

    }
}











