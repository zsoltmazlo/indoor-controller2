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

typedef std::function<void(uint8_t) > uint8_callback;

namespace connection {

using message_callback = std::function<void(const utils::Message&) >;

class MessageHandler {
public:

	message_callback callback;

	std::string topic;

	MessageHandler(std::string&& t, message_callback cb) : callback(cb), topic(t) {

	}

	bool checkField(const std::string& t) {
		return topic.compare(t) == 0;
	}

};

class ConnectionManager {
	std::shared_ptr<TCPServer> server;

	std::list<MessageHandler> handlers;

public:

	ConnectionManager();

	void init();

	void connectToNetwork();
	
	void connectToBroker();

	std::string getMACAddress();

	std::string getIpAddress();

	void addMessageHandler(const MessageHandler& handler);

	void startTcpServer(uint16_t port);

private:

	void tcp_server_worker(void);

	template<typename T>
	void send_ack(const std::string& message, T new_value, TCPClient& client);

	void send_nack(const std::string& message, TCPClient& client);

	template<typename T>
	T parse_message(TCPClient& client, bool* success);

};

}

