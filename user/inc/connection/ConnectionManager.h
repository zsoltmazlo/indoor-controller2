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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <stdint.h>
#include <functional>
#include "spark_wiring_tcpserver.h"

typedef std::function<void(uint8_t) > uint8_callback;

class ConnectionManager {
	TCPServer* server;

	uint8_callback led_brightness_arrived;
	uint8_callback hifi_volume_arrived;
	uint8_callback change_channel_callback;

	uint8_t clients_connected;

public:

	ConnectionManager();

	void init();

	void connectToNetwork();

	void getMACAddress(char* address);

	void getIpAddress(char* address);

	void setChangeChannelCallback(uint8_callback change_channel_callback);

	void setHifiVolumeArrivedCallback(uint8_callback hifi_volume_arrived);

	void setLedBrightnessArrivedCallback(uint8_callback led_brightness_arrived);

	void startTcpServer(uint16_t port);

private:
	
	void tcp_server_worker(void);
	
	template<typename T>
	void send_ack(const char* message, T new_value, TCPClient& client);
	
	void send_nack(const char* message, TCPClient& client);
	
	template<typename T>
	
	T parse_message(TCPClient& client, bool* success);
	
	
};



#endif /* CONNECTIONMANAGER_H */

