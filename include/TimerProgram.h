#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"
#include <GraphicsLib.h>

class TimerProgram : public Program {
	private:
		std::vector<Button*> buttons;
		TFT_eSPI *tft;
		TTGOClass *watch;
		int h;
		int m;
		int s;
		int state;
		bool lastTouched;
		unsigned long endTime;
		void handleButtonPresses();
		void drawButtons();
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