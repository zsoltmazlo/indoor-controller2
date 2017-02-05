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
    connected_hosts_changed = nullptr;
    clients_connected = 0;
}

void ConnectionManager::init() {
    spark::WiFi.on();

    // no need for password, home WiFi network satefy is MAC address based
    spark::WiFi.setCredentials("lsmx");
}

void ConnectionManager::connectToNetwork() {
    spark::WiFi.on();
    spark::WiFi.connect();

    while (!spark::WiFi.ready()) {

        if (!spark::WiFi.connecting()) {
            spark::WiFi.connect();
        }

        Particle.process();
        delay(100);
    }
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

void ConnectionManager::setHostsChangedCallback(uint8_callback connected_hosts_changed) {
    this->connected_hosts_changed = connected_hosts_changed;
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

void ConnectionManager::tcp_server_worker() {

    debug::println("SERVER | Thread started.");

    TCPClient client;
    char buffer[1024];
    uint8_t i = 0;

    IPAddress lastClient(0, 0, 0, 0);
    client.setTimeout(5000);

    while (true) {

        if (client.connected()) {

            if (!(lastClient == client.remoteIP())) {
                debug::println("Server | New client connected:\n\t IP: %s",
                        client.remoteIP().toString().c_str());
                lastClient = client.remoteIP();
                ++clients_connected;
                if (connected_hosts_changed != nullptr) {
                    connected_hosts_changed(clients_connected);
                }
            }

            // if no message received yet, continue
            if (client.available() == 0) {
                continue;
            }

            // then receive message
            memset(buffer, 0, sizeof (buffer));
            i = 0;
            while (client.available()) {
                buffer[i++] = (char) client.read();
            }

            // interpret message
            std::string json(buffer);
            value v;
            std::string err = parse(v, json);

            picojson::object response;

            // if there was any error message, debug it
            if (!err.empty()) {
                debug::println("SERVER | Error occur during receiving JSON file:\n\t %s",
                        err.c_str());

                sprintf(buffer, "Error occur during receiving JSON file: %s", err.c_str());
                response["message"] = value(buffer);
                response["success"] = value(false);

                client.printf("%s\n", value(response).serialize().c_str());
                continue;
            }

            // if received message is not an JSON object, halt
            if (!v.is<object>()) {
                debug::println("SERVER | Received message is not a JSON object.");

                strcpy(buffer, "Received message is not a JSON object.");
                response["message"] = value(buffer);
                response["success"] = value(false);

                client.printf("%s\n", value(response).serialize().c_str());

                continue;
            }


            // check if there was volume tag in message
            debug::println("SERVER | Received:");
            const value::object& obj = v.get<object>();
            for (value::object::const_iterator i = obj.begin();
                    i != obj.end();
                    ++i) {
                debug::println("\t %s: %s", i->first.c_str(), i->second.to_str().c_str());

                if (i->first == "volume") {
                    uint8_t vol = (uint8_t) i->second.get<double>();
                    if (hifi_volume_arrived != nullptr) {
                        debug::println("\t Setting up volume: %d", vol);
                        hifi_volume_arrived(vol);

                        strcpy(buffer, "Volume changed.");
                        response["message"] = value(buffer);
                        response["success"] = value(true);
                        response["new_value"] = value((double) vol);

                        client.printf("%s\n", value(response).serialize().c_str());
                    }
                }

                if (i->first == "led") {
                    uint8_t percent = (uint8_t) i->second.get<double>();
                    if (led_brightness_arrived != nullptr) {
                        debug::println("\t Setting up led brightness: %d", percent);
                        led_brightness_arrived(percent);

                        strcpy(buffer, "LED brightness changed.");
                        response["message"] = value(buffer);
                        response["success"] = value(true);
                        response["new_value"] = value((double) percent);

                        client.printf("%s\n", value(response).serialize().c_str());
                    }
                }
            }

            client.stop();
            --clients_connected;
            if (connected_hosts_changed != nullptr) {
                connected_hosts_changed(clients_connected);
            }
            lastClient = IPAddress(0, 0, 0, 0);

        } else {
            // if no client is yet connected, check for a new connection
            client = server->available();
        }

        delay(100);

    }
}











