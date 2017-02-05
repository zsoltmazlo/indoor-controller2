/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "utils/debug.h"

#include <cstdio>
#include <stdarg.h>
#include "spark_wiring_usartserial.h"

static char* construct_message(const char* fmt, va_list args) {
    static char _buffer[1024];
    memset(_buffer, 0, sizeof(_buffer));
    vsprintf(_buffer, fmt, args);
    return _buffer;
}

void debug::init(uint32_t baud_rate) {
    Serial1.begin(baud_rate);
}

void debug::print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Serial1.print(construct_message(fmt, args));
}

void debug::println(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Serial1.println(construct_message(fmt, args));
}