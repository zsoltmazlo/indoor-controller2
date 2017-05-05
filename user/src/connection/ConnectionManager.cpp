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

ConnectionManager::ConnectionManager() {
    led_brightness_arrived = nullptr;
    hifi_volume_arrived = nullptr;
    change_channel_callback = nullptr;
    clients_connected = 0;
}

void ConnectionManager::init() {
    spark::WiFi.on();

    // no need for password, home WiFi network satefy is MAC address based
    spark::WiFi.setCredentials("lsmx49");
}

void ConnectionManager::connectToNetwork() {
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

void ConnectionManager::getIpAddress(char* address) {
    IPAddress ip = spark::WiFi.localIP();

    // need to call directly Particle process to get really addres...
    Particle.process();

    sprintf(address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

void ConnectionManager::getMACAddress(char* address) {
    uint8_t mac[6];
    spark::WiFi.macAddress(mac);

    // need to call directly Particle process to get really addres...
    Particle.process();

    sprintf(address, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void ConnectionManager::setChangeChannelCallback(uint8_callback change_channel_callback) {
    this->change_channel_callback = change_channel_callback;
}

void ConnectionManager::setHifiVolumeArrivedCallback(uint8_callback hifi_volume_arrived) {
    this->hifi_volume_arrived = hifi_volume_arrived;
}

void ConnectionManager::setLedBrightnessArrivedCallback(uint8_callback led_brightness_arrived) {
    this->led_brightness_arrived = led_brightness_arrived;
}

void ConnectionManager::startTcpServer(uint16_t port) {
    server = new TCPServer(port);
    server->begin();
    Thread("server", std::bind(&ConnectionManager::tcp_server_worker, this), 9);
}

void ConnectionManager::send_nack(const char* message, TCPClient& client) {
    picojson::object response;
    response["message"] = value(message);
    response["success"] = value(false);
    client.printf("%s\n", value(response).serialize().c_str());
    client.flush();
}

template<typename T>
void ConnectionManager::send_ack(const char* message, T new_value, TCPClient& client) {
    picojson::object response;
    response["message"] = value(message);
    response["success"] = value(true);
    response["new_value"] = value((T) new_value);
    client.printf("%s\n", value(response).serialize().c_str());
    client.flush();
}

template<>
value ConnectionManager::parse_message(TCPClient& client, bool* success) {
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

void ConnectionManager::tcp_server_worker() {

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

            /**
             * if (i->first == "volume") {
                    uint8_t vol = (uint8_t) i->second.get<double>();
                    if (hifi_volume_arrived != nullptr) {
                        debug::println("\t Setting up volume: %d", vol);
                        hifi_volume_arrived(vol);
                        send_ack("Volume changed.", (double) vol, client);
                    }
                }
             */
            auto message_handler = [this, &client](
                    uint8_callback& cb,
                    std::pair<const std::string, picojson::value>& i,
                    const char* field,
                    const char* message) {
                if (i.first == field) {
                    uint8_t vol = (uint8_t) i.second.get<double>();
                    if (cb != nullptr) {
                        cb(vol);
                        debug::println("\t %s New value: %d", message, vol);
                        send_ack(message, (double) vol, client);
                    }
                }
            };


            // check if there was volume tag in message
            debug::println("SERVER | Received:");
            const value::object& message = json_message.get<object>();
            for (auto field : message) {

                debug::println("\t %s: %s", field.first.c_str(), field.second.to_str().c_str());
                message_handler(hifi_volume_arrived, field, "volume", "Volume changed.");
                message_handler(led_brightness_arrived, field, "led", "LED Brightness changed.");
                message_handler(change_channel_callback, field, "tvchannel", "TV channel changed.");

                if (field.first == "close_socket" && field.second.get<bool>()) {
                    close_socket = true;
                }
            }

            if (close_socket) {
                debug::println("\t Disconnect client");
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











