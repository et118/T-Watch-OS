#include "BluetoothMusicProgram.h"
btAudio audio = btAudio("TTGO WATCH V1");
void BluetoothMusicProgram::init() {
	watch = TTGOClass::getWatch();
	tft = watch->tft;
	tft->fillScreen(TFT_BLACK);
	watch->power->setLDO3Mode(1);
    watch->power->setPowerOutPut(AXP202_LDO3, true);
	audio.begin();
	audio.I2S(TWATCH_DAC_IIS_BCK,TWATCH_DAC_IIS_DOUT,TWATCH_DAC_IIS_WS);
}

void BluetoothMusicProgram::update() {
	audio.updateMeta();
	tft->setTextColor(TFT_WHITE,TFT_BLACK);
	tft->setTextSize(1);
	tft->setTextDatum(CC_DATUM);
	tft->drawString("Album: " + audio.album,120,40);
	tft->drawString("Artist: " + audio.artist,120,120);
	tft->drawString("Title: " + audio.title,120,200);
	delay(10);
}

void BluetoothMusicProgram::sleepUpdate() {

}

ProgramSleepMode BluetoothMusicProgram::getSleepMode()
{
	return ProgramUpdateSleep;
}

void BluetoothMusicProgram::sleep() {

}

void BluetoothMusicProgram::wakeup() {

}

void BluetoothMusicProgram::exit() {
	watch->power->setLDO3Mode(0);
    watch->power->setPowerOutPut(AXP202_LDO3, false);
	audio.end();
}