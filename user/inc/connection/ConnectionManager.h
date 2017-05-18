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

typedef std::function<void(uint8_t) > uint8_callback;

namespace connection {

union MessageData {
	int i;
	float f;
	long l;
	double d;
	bool b;
};

using message_callback = std::function<void(const MessageData&) >;

class MessageHandler {
	
public:

	message_callback callback;
	
	std::string applyOnField;
	
	MessageHandler(const char* field, message_callback cb): callback(cb), applyOnField(field) {
		
	}
	
	bool checkField(std::string field) {
		return applyOnField.compare(field) == 0;
	}

};

class ConnectionManager {
	std::shared_ptr<TCPServer> server;

	std::list<MessageHandler> handlers;

public:

	ConnectionManager();

	void init();

	void connectToNetwork();

	void getMACAddress(char* address);

	void getIpAddress(char* address);
	
	void addMessageHandler(const MessageHandler& handler);

	void startTcpServer(uint16_t port);

private:

	void tcp_server_worker(void);

	template<typename T>
	void send_ack(const char* message, T new_value, TCPClient& client);

	void send_nack(const char* message, TCPClient& client);

	template<typename T>
	T parse_message(TCPClient& client, bool* success);



};

}

