/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConnectionManager.h
 * Author: zsoltmazlo
 *
 * Created on February 4, 2017, 10:58 PM
 */

#pragma once

#include <stdint.h>
#include <functional>
#include "spark_wiring_tcpserver.h"
#include <string>
#include <list>
#include <memory>
#include "../utils/Message.h"
#include "MQTT.h"

typedef std::function<void(uint8_t) > uint8_callback;

namespace connection {

using message_callback = std::function<void(const utils::Message&) >;

class MessageHandler {
public:

	message_callback callback;

	std::string field;

	MessageHandler(std::string&& field, message_callback cb) : callback(cb), field(field) {

	}

	bool checkField(const std::string& f) {
		return field.compare(f) == 0;
	}

};

class ConnectionManager {

public:
	
	enum State {
		DISCONNECTED,
		CONNECTING,
		CONNECTED
	};

	ConnectionManager();
	
	void init();

	void connectToNetwork(const std::string& ssid, const std::string& password = "");
	
	std::string getMACAddress();

	std::string getIpAddress();

	void addMessageHandler(const MessageHandler& handler);

	void startTcpServer(uint16_t port);
	
	void connectToBroker(const std::string& broker, uint16_t port);
	
	State getConnectionState() const;

private:
	
	State state;
	
	std::shared_ptr<TCPServer> server;

	std::list<MessageHandler> handlers;
	
	std::shared_ptr<MQTT> broker;

	void tcp_server_worker(void);
	
	void mqtt_server_worker(void);
	
	void mqtt_onmessage_callback(char* topic, uint8_t* payload, unsigned int len);

	template<typename T>
	void send_ack(const std::string& message, T new_value, TCPClient& client);

	void send_nack(const std::string& message, TCPClient& client);

	template<typename T>
	T parse_message(TCPClient& client, bool* success);

};

}

