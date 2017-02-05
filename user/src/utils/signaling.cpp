/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "utils/signaling.h"
#include "spark_wiring.h"
#include "spark_wiring_rgb.h"
#include "spark_wiring_thread.h"
#include "spark_wiring_usbserial.h"


/**
 * brightness will store 8*8 bit of information about brightness
 */
static uint64_t _signaling_pattern_brightness = 0x00;
static uint8_t _signaling_pattern_red = 0x00;
static uint8_t _signaling_pattern_red_next = 0x00;
static uint8_t _signaling_pattern_green = 0x00;
static uint8_t _signaling_pattern_green_next = 0x00;
static uint8_t _signaling_pattern_blue = 0x00;
static uint8_t _signaling_pattern_blue_next = 0x00;
static volatile bool _show_previous_pattern = false;

static void __signaling_thread_worker(void) {

    uint8_t cntr = 0, masker = 0;
    uint8_t previous_pattern = 8;

    uint8_t r, g, b, brightness;
    uint64_t mask = 0xF000000000000000;


    while (1) {

        // set RGB control to be ours if it's not
        if (!RGB.controlled()) {
            RGB.control(true);
        }

        brightness = (_signaling_pattern_brightness & mask) >> ((15 - masker) * 4);
        Serial.print((uint32_t) (_signaling_pattern_brightness >> 32), HEX);
        Serial.print((uint32_t) _signaling_pattern_brightness, HEX);
        Serial.print(" & ");
        Serial.print((uint32_t) (mask >> 32), HEX);
        Serial.print((uint32_t) mask, HEX);
        Serial.print(" = ");
        Serial.print(brightness, HEX);
        Serial.print(" cntr: ");
        Serial.println(cntr);
        r = ((_signaling_pattern_red & (1 << cntr)) > 0) * 255;
        g = ((_signaling_pattern_green & (1 << cntr)) > 0) * 255;
        b = ((_signaling_pattern_blue & (1 << cntr)) > 0) * 255;

        RGB.brightness(brightness << 4 | 0x0F);
        RGB.color(r, g, b);
        ++cntr;
        ++masker;
        mask >>= 4;
        mask = mask > 0 ? mask : 0xF000000000000000;
        cntr = cntr > 7 ? 0 : cntr;
        masker = masker > 15 ? 0 : masker;
        delay(60); // 16 bit in pattern, 16*60 = 960

        // check if we are using old pattern or new one: if old pattern is used,
        // then we need to send 8 signal with the old pattern, and then change to
        // the new one
        if (_show_previous_pattern) {
            --previous_pattern;

            // if all the signal sent out, then use the new one
            if (previous_pattern == 0) {
                _show_previous_pattern = false;
                previous_pattern = 8;
                _signaling_pattern_red = _signaling_pattern_red_next;
                _signaling_pattern_green = _signaling_pattern_green_next;
                _signaling_pattern_blue = _signaling_pattern_blue_next;
            }
        }
    }
}

void signaling::set_state(STATES state, bool show_previous_pattern) {
    uint8_t* holder_to_write_red;
    uint8_t* holder_to_write_green;
    uint8_t* holder_to_write_blue;

    // if we defined that we want to show the previous pattern, then save
    // into next buffer the new pattern, otherwise it could go into the pattern
    _show_previous_pattern = show_previous_pattern;
    if (show_previous_pattern) {
        holder_to_write_red = &_signaling_pattern_red_next;
        holder_to_write_green = &_signaling_pattern_green_next;
        holder_to_write_blue = &_signaling_pattern_blue_next;
    } else {
        holder_to_write_red = &_signaling_pattern_red;
        holder_to_write_green = &_signaling_pattern_green;
        holder_to_write_blue = &_signaling_pattern_blue;
    }

    switch (state) {
        case INIT:
            _signaling_pattern_brightness = 0xFFFFFFFFFFFFFFFF;
            *holder_to_write_red = 0xFF;
            *holder_to_write_green = 0;
            *holder_to_write_blue = 0;
            break;

        case IDLE:
            //            _signaling_pattern_brightness = 0xFFBF7F3F003F7FBF;
            _signaling_pattern_brightness = 0xFDB9753112579BDF;
            *holder_to_write_red = 0xFF;
            *holder_to_write_green = 0xFF;
            *holder_to_write_blue = 0;
            break;

    }


}

void signaling::start_thread(uint8_t priority) {
    Thread("signaling", &__signaling_thread_worker, priority);
}