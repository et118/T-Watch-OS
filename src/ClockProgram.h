#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"
#include <iostream>
#include <iomanip>

class ClockProgram : public Program {
	private:
		TFT_eSPI *tft;
		TTGOClass *watch;
		int h;
		int m;
		int s;
	public:
		using Program::Program;

		void init();
		void update();
		void sleepUpdate();
		ProgramSleepMode getSleepMode();
		void sleep();
		void wakeup();
		void exit();
};