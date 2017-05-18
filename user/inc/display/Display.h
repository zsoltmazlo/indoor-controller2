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

#pragma once

#include "Adafruit_ST7735.h"
#include <memory>
#include <map>
#include <string>

struct ItemProperties {

	enum Align {
		NEW_LINE,
		SAME_LINE
	};

	uint8_t font_size;
	uint8_t align;

	uint16_t color, x, y, offset;
	
	std::string postfix;

};

class Display {
	std::shared_ptr<Adafruit_ST7735> tft;
	std::map<int, ItemProperties> items;

public:

	Display(uint8_t cs, uint8_t dc, uint8_t rst);

	void init();

	void addItem(int key, const char* label, ItemProperties properties);

	template<typename T>
	void updateItem(int key, T value) {
		auto props = items[key];
		// clear the whole row for the new item
		tft->fillRect(props.x + props.offset, props.y, 128 - props.x + props.offset, props.font_size * 10, ST7735_BLACK);
		tft->setCursor(props.x + props.offset, props.y);
		tft->setTextColor(props.color);
		tft->setTextSize(props.font_size);
		tft->print(value);
		tft->print(props.postfix.c_str());
	}
	
	void clear() {
		tft->clear();
	}

	void print(const char* fmt, ...);

	void println(const char* fmt, ...);

};


