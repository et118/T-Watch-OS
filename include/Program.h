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
		virtual ProgramSleepMode getSleepMode()=0;
		virtual void sleep()=0;
		virtual void wakeup()=0;
		virtual void exit()=0; //ADD MAIN BUTTON PRESS CHECK
};
