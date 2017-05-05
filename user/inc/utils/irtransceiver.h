/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   irtransceiver.h
 * Author: zsoltmazlo
 *
 * Created on May 5, 2017, 9:56 PM
 */

#pragma once

#define SAMSUNG_VOLU_D 0x0B0707
#define SAMSUNG_VOLU_U 0x070707

#define SAMSUNG_CHAN_D 0x100707
#define SAMSUNG_CHAN_U 0x120707

#define SAMSUNG_CHAN_1  0x040707
#define SAMSUNG_CHAN_2  0x050707
#define SAMSUNG_CHAN_3  0x060707
#define SAMSUNG_CHAN_4  0x080707
#define SAMSUNG_CHAN_5  0x090707
#define SAMSUNG_CHAN_6  0x0A0707
#define SAMSUNG_CHAN_7  0x0C0707
#define SAMSUNG_CHAN_8  0x0D0707
#define SAMSUNG_CHAN_9  0x0E0707
#define SAMSUNG_CHAN_0  0x110707

#define SAMSUNG_PRE_CH  0x130707
#define SAMSUNG_SOURCE  0x010707
#define SAMSUNG_RETURN  0x580707
#define SAMSUNG_ACCEPT  0x680707

void change_to_channel(int channel);

void send_ir_command(long command);

