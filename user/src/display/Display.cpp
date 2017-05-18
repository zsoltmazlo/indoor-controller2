/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "display/Display.h"
#include <stdarg.h>

Display::Display(uint8_t cs, uint8_t dc, uint8_t rst) :
tft(new Adafruit_ST7735(cs, dc, rst)) {

}

void Display::init() {
    tft->initB();
    tft->clear();

}

void Display::addItem(int key, const char* label, ItemProperties properties) {

    // immediately show on LCD
    tft->setCursor(properties.x, properties.y);
    tft->setTextSize(1);
    tft->setTextColor(properties.color);
    tft->print(label);

    switch (properties.align) {
        case ItemProperties::NEW_LINE:
            properties.y += 10;
            break;

        case ItemProperties::SAME_LINE:
            properties.x = tft->getCursorX();
            break;
    }

    tft->setCursor(properties.x + properties.offset, properties.y);
    tft->setTextSize(properties.font_size);
    tft->print("n.a.");


    items[key] = properties;
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









