#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"
#include <WebServer.h>
#include <btAudio.h>
class BluetoothMusicProgram : public Program {
	private:
		TFT_eSPI *tft;
		TTGOClass *watch;
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