#include "StopwatchProgram.h"

void StopwatchProgram::init() {
	watch = TTGOClass::getWatch();
	tft = watch->tft;
	tft->fillScreen(TFT_BLACK);
	h = 0;
	m = 0;
	s = 0;
	ms = 0;
	state = 0;
	startMillis = 0;
	mill = 0;
}

void StopwatchProgram::update() {
	bool touched = watch->touched();
	if(touched && !lastTouched) {
		state++;
		lastTouched = true;
	}
	if(!touched) {
		lastTouched = false;
	}

	switch(state) {
		case 0:
			h = 0;
			m = 0;
			s = 0;
			ms = 0;
			break;
		case 1:
			startMillis = millis();
			state++;
			break;
		case 2:
			mill = millis() - startMillis;
			ms = fmod(mill, 1000);
			s = fmod(mill/1000,60);
			m = fmod(mill/1000/60,60);
			h = fmod(mill/1000/60/60,24);
			break;
		case 3:
			break;
		case 4:
			state = 0;
			break;
	}
	tft->setTextColor(TFT_WHITE,TFT_BLACK);
	tft->setTextSize(2);
	tft->setTextDatum(CC_DATUM);
	String time;
	if(h>=10) {
		time += (String)h;
	}else{
		time += "0"+(String)h;
	}

	if(m>=10) {
		time += ":"+(String)m;
	}else{
		time += ":0"+(String)m;
	}

	if(s>=10) {
		time += ":"+(String)s;
	}else{
		time += ":0"+(String)s;
	}

	if(ms>=100) {
		time += ":"+(String)ms;
	} else if(ms >= 10) {
		time += ":0"+(String)ms;
	} else {
		time += ":00"+(String)ms;
	}
	tft->drawString(time,120,120);
	//delay(1);
}

void StopwatchProgram::sleepUpdate() {
}

ProgramSleepMode StopwatchProgram::getSleepMode() {
	return ProgramLightSleep;
}

void StopwatchProgram::sleep() {
	
}

void StopwatchProgram::wakeup() {
}

void StopwatchProgram::exit() {
}