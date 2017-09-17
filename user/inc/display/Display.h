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
#include "Adafruit_ILI9341.h"
#include <memory>
#include <map>
#include <string>

#define COLOR_BLACK  ILI9341_BLACK
#define COLOR_RED  ILI9341_RED
#define COLOR_GREEN  ILI9341_GREEN
#define COLOR_BLUE  ILI9341_BLUE
#define COLOR_YELLOW ILI9341_YELLOW
#define COLOR_GREY ILI9341_DARKERGREY
#define COLOR_LIGHTGREY ILI9341_LIGHTGREY

#define DISPLAY_TYPE Adafruit_ILI9341
#define DISPLAY_MAXX 320
#define DISPLAY_MAXY 240

struct ItemProperties {

	enum Align {
		NEW_LINE,
		SAME_LINE
	};

	uint8_t font_size;
	uint8_t align;

	uint16_t color, x, y, offset;

	char postfix[4];

	char current_value[20];

};

#define NUMBER_OF_DATA_POINTS 24

class Display {
	std::shared_ptr<DISPLAY_TYPE> tft;
	std::map<int, ItemProperties> items;

	uint8_t backlight;

	struct Graph {
		uint16_t x, y, w, h, grid_x, grid_y, y_pos;
		uint8_t data_index;
		float data[NUMBER_OF_DATA_POINTS] = {0.0f};
	};

	Graph _graph;

public:

	enum DisplayBlacklight {
		Off,
		On
	};

	Display(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t backlight);

	void init();

	void addItem(int key, const char* label, ItemProperties properties);

	template<typename T>
	void updateItem(int key, T value) {
		auto props = items[key];

		// clear only the bounding box of the previous box
		char tempStr[24];
		sprintf(tempStr, "%s%s", props.current_value, props.postfix);

		tft->setTextSize(props.font_size);

		// calculating approximately the size of the text
		uint16_t w = props.font_size * 6 * strlen(tempStr) - 1;
		uint16_t h = props.font_size * 8;

		tft->fillRect(props.x + props.offset, props.y, w, h, COLOR_BLACK);
		tft->setCursor(props.x + props.offset, props.y);
		tft->setTextColor(props.color);
		tft->print(value);
		tft->print(props.postfix);
		this->updatePropertyValue(key, value);
	}

	void _drawGraphAxes();

	void enableGraph(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

	void addDataToGraph(float data);

	template<typename T>
	void updatePropertyValue(int key, T value);

	void clear();

	void setBacklight(DisplayBlacklight backlight);

	void print(const char* fmt, ...);

	void println(const char* fmt, ...);

};