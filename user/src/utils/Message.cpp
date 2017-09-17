/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "./utils/Message.h"

utils::Message::Message() {

}

utils::Message::Message(double d) {
    state = Double;
    data.d = d;
}

utils::Message::Message(float f) {
    state = Float;
    data.f = f;
}

utils::Message::Message(bool b) {
    state = Bool;
    data.b = b;
}

utils::Message::Message(int i) {
    state = Integer;
    data.i = i;
}

utils::Message::Message(long l) {
    state = Long;
    data.l = l;
}

utils::Message::Message(const std::string& str) {
    state = String;
    s = str;
}

utils::Message::Message(std::string&& str) {
    state = String;
    s = std::move(str);
}

utils::Message::Message(const char* str) {
    state = String;
    s = str;
}

utils::Message& utils::Message::operator=(double d) {
    state = Double;
    data.d = d;
    return *this;
}

utils::Message& utils::Message::operator=(float f) {
    state = Float;
    data.f = f;
    return *this;
}

utils::Message& utils::Message::operator=(bool b) {
    state = Bool;
    data.b = b;
    return *this;
}

utils::Message& utils::Message::operator=(int i) {
    state = Integer;
    data.i = i;
    return *this;
}

utils::Message& utils::Message::operator=(long l) {
    state = Long;
    data.l = l;
    return *this;
}

utils::Message& utils::Message::operator=(const std::string& str) {
    state = String;
    s = str;
    return *this;
}

utils::Message& utils::Message::operator=(std::string&& str) {
    state = String;
    s = std::move(str);
    return *this;
}

utils::Message& utils::Message::operator=(const char* str) {
    state = String;
    s = str;
    return *this;

}










