/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "inc/utils/irtransceiver.h"
#include "spark_wiring_usartserial.h"
#include "debug.h"
#include "utils/debug.h"
#include "spark_wiring_thread.h"
#include "spark_wiring.h"

#define TRANSCEIVER Serial1

static long _commands[10] = {
    SAMSUNG_CHAN_0,
    SAMSUNG_CHAN_1,
    SAMSUNG_CHAN_2,
    SAMSUNG_CHAN_3,
    SAMSUNG_CHAN_4,
    SAMSUNG_CHAN_5,
    SAMSUNG_CHAN_6,
    SAMSUNG_CHAN_7,
    SAMSUNG_CHAN_8,
    SAMSUNG_CHAN_9
};

void change_to_channel(int channel) {
    
    long _numbers[3];
    
    for(uint8_t i=0; i<3; ++i) {
        _numbers[2-i] = channel%10;
        channel /= 10;
    }

    for(uint8_t i=0; i<3; ++i) {
        send_ir_command(_commands[_numbers[i]]);
        delay(200);
    }
    send_ir_command(SAMSUNG_ACCEPT);
   
}

void send_ir_command(long command) {
    // first, explode command into bytes
    uint8_t buffer[4];
    memcpy(buffer, &command, sizeof (command));

    // sending only non-zero bytes
    TRANSCEIVER.write(0xA1);
    TRANSCEIVER.write(0xF1);
    for (uint8_t i = 0; i < 4 && buffer[i] != 0; ++i) {
        TRANSCEIVER.write(buffer[i]);
    }
    TRANSCEIVER.flush();
}

