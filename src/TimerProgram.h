#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"
#include <iostream>
#include <iomanip>

class TimerProgram : public Program {
	private:

		class Button {
			private:
				int x1;
				int y1;
				int x2;
				int y2;
				uint32_t fillColor;
				uint32_t outlineColor;

			public:
				Button(int x1, int y1, int x2, int y2, uint32_t fillColor, uint32_t outlineColor) {
					Button::x1 = x1;
					Button::x2 = x2;
					Button::y1 = y1;
					Button::y2 = y2;
					Button::fillColor = fillColor;
					Button::outlineColor = outlineColor;
				};

				void drawButton(TFT_eSPI* tft) {
					tft->fillRect(x1,y1,x2-x1,y2-y1,fillColor);
					tft->drawRect(x1,y1,x2-x1,y2-y1,outlineColor);
				};

				void changeButtonColor(uint32_t fillColor, uint32_t outlineColor) {
					Button::fillColor = fillColor;
					Button::outlineColor = outlineColor;
				}

				bool isButtonClicked(int x, int y) {
					if(x > x1 && x < x2 && y > y1 && y < y2) {
						return true;
					} else {
						return false;
					}
				};


		};

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