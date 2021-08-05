#pragma once
#include <string>
#include "Arduino.h"

enum ProgramSleepMode {
	ProgramNoSleep,
	ProgramUpdateSleep,
	ProgramLightSleep,
	ProgramDeepSleep
};

class Program {
	private:
		std::string name;

	public:
		Program(std::string n){
			Program::name = n;
		};
		std::string getName();

		virtual void init()=0;
		virtual void update()=0;
		virtual void sleepUpdate()=0;
		virtual ProgramSleepMode sleep()=0;
		virtual void wakeup()=0;
		virtual void exit()=0;
};
