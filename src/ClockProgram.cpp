#include "ClockProgram.h"

void ClockProgram::init()
{
	watch = TTGOClass::getWatch();
	tft = watch->tft;
	tft->fillScreen(TFT_BLACK);
}

void ClockProgram::update()
{
	tft->setTextColor(TFT_WHITE,TFT_BLACK);
	tft->setTextSize(4);
	RTC_Date d = watch->rtc->getDateTime();
	h = d.hour;
	m = d.minute;
	s = d.second;
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

	tft->drawString(time,120,120);
	tft->setTextSize(3);
	tft->drawString((String)watch->power->getBattPercentage()+"%",200,30);
	delay(10);
}

void ClockProgram::sleepUpdate() {
}

ProgramSleepMode ClockProgram::getSleepMode()
{
	return ProgramLightSleep;
}

void ClockProgram::sleep() 
{
}

void ClockProgram::wakeup()
{
}

void ClockProgram::exit()
{
}