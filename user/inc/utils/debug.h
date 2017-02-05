/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   debug.h
 * Author: zsoltmazlo
 *
 * Created on February 4, 2017, 11:05 PM
 */

#ifndef APP_DEBUG_H
#define APP_DEBUG_H

#include <stdint.h>

namespace debug {
	
	void init(uint32_t baud_rate);
	
	void print(const char* fmt, ...);
	
	void println(const char* fmt, ...);
}

#endif /* APP_DEBUG_H */

