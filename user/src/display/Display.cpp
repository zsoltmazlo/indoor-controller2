/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "display/Display.h"
#include <stdarg.h>
#include "utils/debug.h"

Display::Display(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t backlight) :
//tft(new Adafruit_ST7735(cs, dc, rst)) {
tft(new Adafruit_ILI9341(cs, dc, rst)) {
    this->backlight = backlight;
    pinMode(backlight, OUTPUT);
    this->_graph.data_index = 0;
}

void Display::init() {
    //    tft->initB();
    tft->begin();
    tft->setRotation(3);
    this->clear();

}

void Display::addItem(int key, const std::string& label, ItemProperties properties) {

    // immediately show on LCD
    tft->setCursor(properties.x, properties.y);
    tft->setTextSize(1);
    tft->setTextColor(properties.color);
    tft->print(label.c_str());

    switch (properties.align) {
        case ItemProperties::NEW_LINE:
            properties.y += 12;
            break;

        case ItemProperties::SAME_LINE:
            properties.x = tft->getCursorX();
            break;
    }

    tft->setCursor(properties.x + properties.offset, properties.y);
    tft->setTextSize(properties.font_size);
    sprintf(properties.current_value, "-");
    tft->print(properties.current_value);

    items[key] = properties;
}

void Display::clear() {
    tft->fillRect(0, 0, DISPLAY_MAXX, DISPLAY_MAXY, COLOR_BLACK);
}

void Display::setBacklight(DisplayBlacklight backlight) {
    switch (backlight) {
        case On:
            digitalWrite(this->backlight, HIGH);
            break;
        case Off:
            digitalWrite(this->backlight, LOW);
            break;
    }
}

static char* construct_message(const char* fmt, va_list args) {
    static char _buffer[1024];
    memset(_buffer, 0, sizeof (_buffer));
    vsprintf(_buffer, fmt, args);
    return _buffer;
}

void Display::print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    tft->print(construct_message(fmt, args));

}

void Display::println(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    tft->println(construct_message(fmt, args));

}

template<>
void Display::updatePropertyValue<int>(int key, int value) {
    sprintf(items[key].current_value, "%d", value);
}

template<>
void Display::updatePropertyValue<char*>(int key, char* value) {
    sprintf(items[key].current_value, "%s", value);
}

template<>
void Display::updatePropertyValue<float>(int key, float value) {
    sprintf(items[key].current_value, "%.2f", value);
}

template<>
void Display::updatePropertyValue<const char*>(int key, const char* value) {
    sprintf(items[key].current_value, "%s", value);
}

template<>
void Display::updatePropertyValue<const std::string&>(int key, const std::string& value) {
    sprintf(items[key].current_value, "%s", value.c_str());
}

void Display::_drawGraphAxes() {
    this->tft->fillRect(_graph.x, _graph.y, _graph.w, _graph.h, COLOR_BLACK);

    // draw grid
    uint16_t grid = _graph.x + 6 + _graph.grid_x;
    while (grid < _graph.x + _graph.w - 6) {
        this->tft->drawFastVLine(grid, _graph.y, _graph.h, COLOR_GREY);
        grid += _graph.grid_x;
    }
    grid = _graph.y + _graph.grid_y;
    while (grid < _graph.y + _graph.h) {
        this->tft->drawFastHLine(0, grid, _graph.w, COLOR_GREY);
        grid += _graph.grid_y;
    }

    // draw axes
    this->tft->drawFastHLine(_graph.x, _graph.y + _graph.h - _graph.y_pos, _graph.w, COLOR_BLUE);
    this->tft->drawFastVLine(_graph.x + 6, _graph.y, _graph.h, COLOR_BLUE);

}

void Display::enableGraph(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    this->_graph.x = x;
    this->_graph.y = y;
    this->_graph.w = w;
    this->_graph.h = h;
    this->_graph.grid_x = (w - 12) / NUMBER_OF_DATA_POINTS;
    this->_graph.grid_y = h / 10;
    this->_graph.y_pos = h - 10;
    this->_drawGraphAxes();

}

void Display::addDataToGraph(float data) {
    _graph.data[_graph.data_index] = data;
    _graph.data_index = (_graph.data_index + 1) % NUMBER_OF_DATA_POINTS;

    // search for min/max
    float min = _graph.data[0];
    float max = _graph.data[0];
    for (uint16_t i = 1; i < NUMBER_OF_DATA_POINTS; ++i) {
        if (_graph.data[i] < min) {
            min = _graph.data[i];
        }
        if (_graph.data[i] > max) {
            max = _graph.data[i];
        }
    }

    max += 1;
    min -= 1;

    if (min > -1) {
        min = -1;
    }

    float range = max - min;
    //		debug::println("DISPLAY |\nmax temp: %.1f\nmin temp: %.1f\nrange: %.1f", max, min, range);

    // y position calculation
    _graph.y_pos = _graph.h - (max / range * _graph.h);

    // redraw everything
    this->_drawGraphAxes();
    uint16_t prev_x = 0;
    uint16_t prev_y = _graph.y + _graph.h - _graph.y_pos;
    uint16_t k, x, y;

    x = _graph.x + 6 + _graph.grid_x * 2;

    // first point is the same with the first measurement
    k = _graph.data_index % NUMBER_OF_DATA_POINTS;
    prev_y = _graph.y + _graph.h - (_graph.data[k] / range * _graph.h) - _graph.y_pos;
    prev_x = 0;

    for (uint16_t i = 0; i < NUMBER_OF_DATA_POINTS; ++i) {
        k = (i + _graph.data_index) % NUMBER_OF_DATA_POINTS;
        y = _graph.y + _graph.h - (_graph.data[k] / range * _graph.h) - _graph.y_pos;
        tft->drawLine(prev_x, prev_y, x, y, COLOR_GREEN);
        prev_x = x;
        prev_y = y;
        x += _graph.grid_x;
    }

    // the last measurement will be repeated again
    tft->drawLine(prev_x, prev_y, _graph.grid_x + _graph.w, prev_y, COLOR_GREEN);

    // redraw helpers
    tft->setTextSize(1);
    tft->setTextColor(COLOR_LIGHTGREY);

    uint16_t grid = _graph.y + _graph.grid_y;
    while (grid < _graph.y + _graph.h) {
        tft->setCursor(_graph.x, grid - 4);
        float c = max - ((grid - _graph.y) / ((float) _graph.h) * range);
        tft->printf("%.1f", c);
        grid += _graph.grid_y * 2;
    }

    grid = _graph.x + 6 + _graph.grid_x;
    uint8_t clock = 24;
    while (grid < _graph.x + _graph.w - 6) {
        tft->setCursor(grid - 4, _graph.y + _graph.h + 2);
        tft->printf("%dh", clock);
        grid += _graph.grid_x * 6;
        clock -= 6;
    }

}


