/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Display.h
 * Author: zsoltmazlo
 *
 * Created on February 4, 2017, 10:06 PM
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_ST7735.h"

class Display {
	Adafruit_ST7735* tft;

public:

	Display(uint8_t cs, uint8_t dc, uint8_t rst);

	void init();
	
	void showApplicationUi(char* address, uint16_t port);

	void setLedPulseWidth(uint8_t new_led_pulse_width);

	void setHifiVolume(uint8_t new_volume);

	void setConnectedHosts(uint8_t new_connected);
	
	void setCursor(uint8_t x, uint8_t y);
	
	void print(const char* fmt, ...);
	
	void println(const char* fmt, ...);


private:

	uint8_t _led_pulse_width;

	uint8_t _hifi_volume;

	uint8_t _connected_devices;
};



#endif /* DISPLAY_H */

