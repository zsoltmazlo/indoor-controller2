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

	Display(uint8_t cs, uint8_t dc, uint8_t rst) : tft(new Adafruit_ST7735(cs, dc, rst)) {

	}

	void init() {
		tft->initB();
		tft->clear();

		// create fix UI
		tft->setCursor(3, 5);
		tft->setTextSize(1);
		tft->setTextColor(ST7735_WHITE);
		tft->print("LED strip");
		tft->setCursor(3, 15);
		tft->print("brightness");

		tft->setCursor(3, 65);
		tft->setTextSize(1);
		tft->setTextColor(ST7735_WHITE);
		tft->print("HiFi volume");

		tft->setTextSize(1);
		tft->setTextColor(ST7735_WHITE);
		tft->setCursor(3, 118);
		tft->print("connected: 0");
	}

	void setLedPulseWidth(uint8_t new_led_pulse_width) {
		// if it remained the same, do not update screen
		if (new_led_pulse_width == _led_pulse_width) {
			return;
		}

		_led_pulse_width = new_led_pulse_width;
		tft->fillRect(30, 30, 100, 35, ST7735_BLACK);
		tft->setCursor(30, 30);
		tft->setTextSize(4);
		tft->setTextColor(ST7735_GREEN);
		tft->print(_led_pulse_width);
		tft->print("%");
	}

	void setHifiVolume(uint8_t new_volume) {
		// if it remained the same, do not update it
		if (new_volume == _hifi_volume) {
			return;
		}

		_hifi_volume = new_volume;
		tft->fillRect(30, 80, 100, 35, ST7735_BLACK);
		tft->setCursor(30, 80);
		tft->setTextSize(4);
		tft->setTextColor(ST7735_YELLOW);
		tft->print(_hifi_volume);
		tft->print("%");
	}

	void setConnectedHosts(uint8_t new_connected) {
		// if it remained the same, do not update it
		if (new_connected == _connected_devices) {
			return;
		}

		_connected_devices = new_connected;
		
		tft->setTextSize(1);
		tft->setTextColor(ST7735_WHITE);
		tft->fillRect(65, 118, 70, 10, ST7735_BLACK);
		tft->setCursor(65, 118);
		tft->print(_connected_devices);
	}


private:

	uint8_t _led_pulse_width;

	uint8_t _hifi_volume;

	uint8_t _connected_devices;
};



#endif /* DISPLAY_H */

