/*
 Probe API, 
 Library that structures Arduino programs as probe-able functions 
 via Serial communication. 

 Created by Cesar Torres, 25 April 2018
 
 Header File Name: probe.h 
 Implementation File Name: probe.cpp
 
 */

#ifndef Probe_h
	#define Probe_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif
	
	typedef void (*functype)(void);

	class Probe {
		public:
		    Probe (char* name, Stream & port, int baud) : port_ (port), name_(name), baud_(baud){ }
		    void begin();
		    void enable_api();
		    void add_api(char, functype);
		private:
			Stream & port_; 
			char* myPrefixes;
			functype *myFuncs;
			char* name_;
			int baud_;
			int pos_= 0;
			char prefix = 0;
			char buffer = ' ';
		    void findCommandEnd();	
			void api_call(char);
			
	};
	
	

#endif
