#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"

class StopwatchProgram : public Program {
	private:
		TFT_eSPI *tft;
		TTGOClass *watch;
		int h;
		int m;
		int s;
		int ms;
		int state;
		long startMillis;
		long mill;
		bool lastTouched = true;
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