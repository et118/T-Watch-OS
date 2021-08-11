#include "TimerProgram.h"
void TimerProgram::init() {
	watch = TTGOClass::getWatch();
	tft = watch->tft;
	state = 0;
	h = 0;
	m = 0;
	s = 0;
	endTime = 0;
	lastTouched = true;
	
	buttons = std::vector<Button*>{
		new Button(0,85,240,155,TFT_GREEN,TFT_GREEN),
		new Button(0,0,80,85,TFT_LIGHTGREY,TFT_DARKGREY),
		new Button(80,0,160,85,TFT_LIGHTGREY,TFT_DARKGREY),
		new Button(160,0,240,85,TFT_LIGHTGREY,TFT_DARKGREY),
		new Button(0,155,80,240,TFT_LIGHTGREY,TFT_DARKGREY),
		new Button(80,155,160,240,TFT_LIGHTGREY,TFT_DARKGREY),
		new Button(160,155,240,240,TFT_LIGHTGREY,TFT_DARKGREY),
	};

	drawButtons();
}

void TimerProgram::drawButtons() {
	tft->fillScreen(TFT_BLACK);
	for(Button* b : buttons) {
		b->drawButton(tft);
	}
}

void TimerProgram::handleButtonPresses() {
	bool touched = watch->touched();
	if(touched && !lastTouched) {
		int16_t x = 0;
		int16_t y = 0;
		watch->getTouch(x,y);
		if(state == 3) {
			state = 4;
			return;
		}
		for(int i = 0;i<buttons.size();i++) {
			if(buttons[i]->isButtonClicked(x,y)) {
				if(i == 0) {
					if(state == 0) {
						state = 1;
					} else if(state == 2) {
						state = 0;
						buttons[0]->changeButtonColor(TFT_GREEN,TFT_GREEN);
						drawButtons();
					}
				} else if(state == 0) {
					switch(i) {
						case 0:
							break;
						case 1:
							h++;
							break;
						case 2:
							m++;
							break;
						case 3:
							s++;
							break;
						case 4:
							h--;
							break;
						case 5:
							m--;
							break;
						case 6:
							s--;
							break;
					};
				}
				break;
			}
		}
		lastTouched = true;
	}
	if(!touched) {
		lastTouched = false;
	}
}

void TimerProgram::update() {
	handleButtonPresses();
	switch(state) {
		case 0: {
			if(h < 0) h = 0;
			if(h >= 24) h = 0;
			if(m < 0) m = 0;
			if(m >= 60) m = 0;
			if(s < 0) s = 0;
			if(s >= 60) s = 0;
			String timeString = "";
			if(h>=10) {
				timeString += (String)h;
			}else{
				timeString += "0"+(String)h;
			}
			if(m>=10) {
				timeString += "|"+(String)m;
			}else{
				timeString += "|0"+(String)m;
			}
			if(s>=10) {
				timeString += "|"+(String)s;
			}else{
				timeString += "|0"+(String)s;
			}
			tft->setTextColor(TFT_BLACK,TFT_GREEN);
			tft->setTextSize(5);
			tft->setTextDatum(CC_DATUM);
			tft->drawString(timeString,120,120);
			
			break;
		}
		
		case 1: {
			endTime = millis() + h * 3600000 + m * 60000 + s * 1000;
			buttons[0]->changeButtonColor(TFT_RED,TFT_RED);
			drawButtons();
			state++;
		}

		case 2: {
			if(endTime < millis()) {
				state++;
				break;
			} else {
				long mill = endTime - millis();
				s = fmod(mill/1000,60);
				m = fmod(mill/1000/60,60);
				h = fmod(mill/1000/60/60,24);
			}

			String timeString = ""; // TODO make this to a function to stop reusing it
			if(h>=10) {
				timeString += (String)h;
			}else{
				timeString += "0"+(String)h;
			}
			if(m>=10) {
				timeString += "|"+(String)m;
			}else{
				timeString += "|0"+(String)m;
			}
			if(s>=10) {
				timeString += "|"+(String)s;
			}else{
				timeString += "|0"+(String)s;
			}
			tft->setTextColor(TFT_WHITE,TFT_RED);
			tft->setTextSize(5);
			tft->setTextDatum(CC_DATUM);
			tft->drawString(timeString,120,120);
			break;
		}

		case 3: {
			watch->rtc->disableAlarm();
			watch->motor->onec(350);
			tft->fillScreen(TFT_RED);
			delay(500);
			tft->fillScreen(TFT_BLACK);
			watch->motor->onec(350);
			delay(500);
			tft->fillScreen(TFT_RED);
			watch->motor->onec(350);
			delay(500);
			watch->motor_begin(); // Because the screen brightness got lowered when vibrating.
								  // But why tho?
			state++;
			break;
		}

		case 4: {
			state = 0;
			h = 0;
			m = 0;
			s = 0;
			endTime = 0;
			buttons[0]->changeButtonColor(TFT_GREEN,TFT_GREEN);
			drawButtons();
			break;
		}
	}
}

void TimerProgram::sleepUpdate() {

}

ProgramSleepMode TimerProgram::getSleepMode()
{
	return ProgramLightSleep;
}

void TimerProgram::sleep() {
	if(state == 1 || state == 2) {
		esp_sleep_enable_timer_wakeup((endTime-millis())*1000);
	}
}

void TimerProgram::wakeup() {
	if(state == 2 || state == 3) {
		esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
	}
}

void TimerProgram::exit() {

}