//TODO Graphics library with buttons and stuff
#include "config.h"
#include "InfoLib.h"
#include "Program.h"
#include "ClockProgram.h"
#include "StopwatchProgram.h"
#include "TimerProgram.h"
#include "esp_wifi.h"
#include "esp_bt.h"
using namespace std;
RTC_DATA_ATTR int selectedProgram;
RTC_DATA_ATTR ProgramSleepMode currentSleepMode = ProgramNoSleep;

int defaultProgram = 2;
int clicks = 0;
double doubleClickTime = 300;
double startClickTime = 0;
bool powerIRQ = false;
Program* programs[]{new ClockProgram("Clock"),new StopwatchProgram("Stopwatch"),new TimerProgram("Timer")};
TTGOClass *watch;
TFT_eSPI *tft;

void switchToProgram(int program,bool exit) {
	if(program+1 > sizeof(programs)/sizeof(programs[0])) {
		program = 0;
	}
	if(exit) {
		programs[selectedProgram]->exit();
	}
	selectedProgram = program;
	programs[selectedProgram]->init();
}

void toggleSleep() {
	ProgramSleepMode mode = programs[selectedProgram]->getSleepMode();
	if(currentSleepMode == ProgramNoSleep) { //Put to sleep
		currentSleepMode = mode;
		Serial.println("Switching to sleep mode: " + (String)currentSleepMode);
		programs[selectedProgram]->sleep();
		switch(currentSleepMode) {
			case ProgramNoSleep:
				break;
			case ProgramUpdateSleep:
				watch->closeBL();
				watch->displaySleep();
				break;
			case ProgramLightSleep:
				watch->closeBL();
				watch->displaySleep();
				setCpuFrequencyMhz(20);
				gpio_wakeup_enable((gpio_num_t)AXP202_INT,GPIO_INTR_LOW_LEVEL);
				esp_sleep_enable_gpio_wakeup();
				esp_light_sleep_start();
				toggleSleep();
				break;
			case ProgramDeepSleep:
				esp_sleep_enable_ext0_wakeup((gpio_num_t)AXP202_INT, LOW);
				esp_deep_sleep_start();
				break;
		}
	} else { //Wakeup
		switch(currentSleepMode) {
			case ProgramNoSleep:
				break;
			case ProgramUpdateSleep:
				watch->openBL();
				watch->displayWakeup();
				powerIRQ = false;
				watch->power->clearIRQ();
				break;
			case ProgramLightSleep:
				setCpuFrequencyMhz(240);
				gpio_wakeup_disable((gpio_num_t)AXP202_INT);
				esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO);
				watch->openBL();
				watch->displayWakeup();
				powerIRQ = false;
				watch->power->clearIRQ();
				break;
			case ProgramDeepSleep:
				break;
		}
		programs[selectedProgram]->wakeup();
		Serial.println("Woken up from sleep mode: " + (String)currentSleepMode);
		currentSleepMode = ProgramNoSleep;
	}
}

void initWatch() {
	watch = TTGOClass::getWatch();
	tft = watch->tft;

	watch->begin();
	watch->rtc->check();
	watch->rtc->syncToSystem();
	watch->setBrightness(30);
	watch->openBL();
	watch->motor_begin();
	esp_bt_controller_disable();
	esp_wifi_stop();
	pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
    	powerIRQ = true;
    }, FALLING);
	watch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	watch->power->adc1Enable(AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1,true);
	watch->power->clearIRQ();
	if(!selectedProgram) {
		selectedProgram = defaultProgram;
	}
}

void setup()
{
	Serial.begin(115200);
	initWatch();
	switchToProgram(selectedProgram,false);
	if(currentSleepMode == ProgramDeepSleep) {
		toggleSleep();
	}
}

void loop()
{

	if(powerIRQ) {
		powerIRQ = false;
		watch->power->readIRQ();
		if(watch->power->isPEKShortPressIRQ()) {
			if(clicks == 0) {
				clicks = 1;
				startClickTime = millis();
			} else if(clicks == 1) {
				if(millis() - startClickTime < doubleClickTime) {
					switchToProgram(selectedProgram+1,true);
					Serial.println(programs[selectedProgram]->getName().c_str());
					clicks = 0;
					startClickTime = 0;
				}
			}

		}
		watch->power->clearIRQ();
	}

	if(currentSleepMode == ProgramUpdateSleep) {
		programs[selectedProgram]->sleepUpdate();
	} else if(currentSleepMode == ProgramNoSleep) {
		programs[selectedProgram]->update();
	}

	if(clicks == 1) {
		if(millis() - startClickTime > doubleClickTime) {
			clicks = 0;
			startClickTime = 0;
			toggleSleep();
		}
	}
}