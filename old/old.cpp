
#include "config.h"
using namespace std;
TTGOClass *ttgo;
AXP20X_Class *axp;

QueueHandle_t eventQueueHandle;


enum EVENT {
    EVENT_AXP_INTERRUPT
};
void logMemory() {
    uint32_t usedHeap = ESP.getHeapSize() - ESP.getFreeHeap();
    uint32_t usedPSRAM = ESP.getPsramSize() - ESP.getFreePsram();
    log_d("Used Heap: %f",(float)usedHeap / (float)ESP.getHeapSize() * 100);
    log_d("Used PSRAM: %f",(float)usedPSRAM / (float)ESP.getPsramSize()* 100);
    //ps_malloc(Size)
}

void setup()
{
    Serial.begin(115200);
    Serial.printf("NONONONO");
    eventQueueHandle = xQueueCreate(20,sizeof(EVENT));
    
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    axp = ttgo->power;
    ttgo->lvgl_begin();
    ttgo->openBL();
    ttgo->setBrightness(30);
    

    axp->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_PEK_LONGPRESS_IRQ,AXP202_ON);
    axp->clearIRQ();
    pinMode(AXP202_INT,INPUT);
    attachInterrupt(AXP202_INT,[]{
        BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
        uint8_t eventData = EVENT_AXP_INTERRUPT;
        xQueueSendFromISR(eventQueueHandle,&eventData,&xHigherPriorityTaskWoken);
        if(xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    },FALLING);

    ttgo->rtc->check();
    ttgo->rtc->syncToSystem();
}

void loop()
{
    EVENT eventData;
    if(xQueueReceive(eventQueueHandle,&eventData,5/portTICK_RATE_MS) == pdPASS) { //5/portTICK_RATE_MS stannar i 5 ms och väntar på events.
        switch(eventData) {
            case EVENT_AXP_INTERRUPT:
                axp->readIRQ();
                if(axp->isPEKShortPressIRQ()) {
                    axp->clearIRQ();
                    if(ttgo->bl->isOn()) {
                        ttgo->closeBL();
                        ttgo->displaySleep();
                        ttgo->stopLvglTick();
                    } else{
                        ttgo->openBL();
                        ttgo->startLvglTick();
                        ttgo->displayWakeup();
                    }
                    return;
                }
                else if(axp->isPEKLongtPressIRQ()) {
                    axp->clearIRQ();
                    log_d("Battery: %d",axp->getBattPercentage());
                    return;
                }
        }
    }
    lv_task_handler();
}