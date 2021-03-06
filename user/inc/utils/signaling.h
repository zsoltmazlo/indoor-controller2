/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   debug.h
 * Author: zsoltmazlo
 *
 * Created on February 4, 2017, 3:22 PM
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

namespace signaling {

typedef enum {
	INIT,
	IDLE,
	VOLUME,
	LED_CHANGED,
	CHANNEL_CHANGED,
//	OFF
} States;

void set_state(States state, bool show_previous_pattern = false);

void start_thread(uint8_t priority);

}



#endif /* DEBUG_H */

