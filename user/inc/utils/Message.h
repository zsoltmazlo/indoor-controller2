/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Message.h
 * Author: zsoltmazlo
 *
 * Created on May 19, 2017, 11:27 AM
 */

#pragma once


#include <math.h>
#include <string>

namespace utils {

class Message {

	union MessageData {
		int i;
		float f;
		long l;
		double d;
		bool b;
	};
	
	std::string s;

	enum State {
		Integer,
		Float,
		Long,
		Double,
		Bool,
		String
	};
	

	State state;

	MessageData data;

	template<typename T>
	T int_convert() const {
		switch (state) {
			case Integer: return (T) data.i;
			case Float: return (T) trunc(data.f);
			case Double: return (T) trunc(data.d);
			case Long: return (T) data.l;
			case Bool: return (T) data.b;
			case String: return (T)s.size();
			default:
				return (T) 0;
		}
	}

	template<typename T>
	T double_convert() const {
		switch (state) {
			case Integer: return (T) data.i;
			case Float: return (T) data.f;
			case Double: return (T) data.d;
			case Long: return (T) data.l;
			case Bool: return (T) data.b;
			case String: return (T)s.size();
			default:
				return (T) 0;
		}
	}

public:

	Message();

	Message(double d);

	Message(float d);

	Message(bool b);

	Message(int i);

	Message(long i);

	Message(const std::string& s);
	
	Message(std::string&& s);
	
	Message(const char* s);

	Message& operator=(double d);

	Message& operator=(float d);

	Message& operator=(bool b);

	Message& operator=(int i);

	Message& operator=(long i);
	
	Message& operator=(const std::string& s);
	
	Message& operator=(std::string&& s);
	
	Message& operator=(const char* s);

	operator double() const {
		return double_convert<double>();
	}

	operator float() const {
		return (float) double_convert<float>();
	}

	operator bool() const {
		return int_convert<bool>();
	}

	operator int() const {
		return int_convert<int>();
	}

	operator long() const {
		return int_convert<long>();
	}
	
	operator std::string() const {
		return s;
	}
	
	operator const char*() const {
		return s.c_str();
	}


};

}

