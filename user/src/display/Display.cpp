/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "display/Display.h"
#include <stdarg.h>

Display::Display(uint8_t cs, uint8_t dc, uint8_t rst) :
tft(new Adafruit_ST7735(cs, dc, rst)),
_connected_devices(-1),
_hifi_volume(-1),
_led_pulse_width(-1) {

}

void Display::init() {
    tft->initB();
    tft->clear();

}

void Display::showApplicationUi(char* address, uint16_t port) {
    tft->clear();
    // create fix UI
    tft->setCursor(3, 0);
    tft->setTextSize(1);
    tft->setTextColor(ST7735_WHITE);
    tft->print("LED strip");
    tft->setCursor(3, 10);
    tft->print("brightness");

    tft->setCursor(3, 55);
    tft->setTextSize(1);
    tft->setTextColor(ST7735_WHITE);
    tft->print("HiFi volume");

    tft->setTextSize(1);
    tft->setTextColor(ST7735_WHITE);
    tft->setCursor(3, 102);
    tft->printf("IP: %s", address);
    tft->setCursor(3, 110);
    tft->printf("port: %u", port);
    tft->setCursor(3, 118);
    tft->print("connected: 0");
}

void Display::setConnectedHosts(uint8_t new_connected) {
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

void Display::setHifiVolume(uint8_t new_volume) {
    // if it remained the same, do not update it
    if (new_volume == _hifi_volume) {
        return;
    }

    _hifi_volume = new_volume;
    tft->fillRect(30, 65, 98, 30, ST7735_BLACK);
    tft->setCursor(30, 65);
    tft->setTextSize(4);
    tft->setTextColor(ST7735_YELLOW);
    tft->print(_hifi_volume);
    tft->print("%");
}

void Display::setLedPulseWidth(uint8_t new_led_pulse_width) {
    // if it remained the same, do not update screen
    if (new_led_pulse_width == _led_pulse_width) {
        return;
    }

    _led_pulse_width = new_led_pulse_width;
    tft->fillRect(30, 20, 98, 30, ST7735_BLACK);
    tft->setCursor(30, 20);
    tft->setTextSize(4);
    tft->setTextColor(ST7735_GREEN);
    tft->print(_led_pulse_width);
    tft->print("%");
}

void Display::setCursor(uint8_t x, uint8_t y) {
    tft->setCursor(x, y);
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









