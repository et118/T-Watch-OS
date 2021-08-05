#include "InfoLib.h"

void logMemory() {
	uint32_t usedHeap = ESP.getHeapSize() - ESP.getFreeHeap();
	uint32_t usedPSRAM = ESP.getPsramSize() - ESP.getFreePsram();
	log_d("Used Heap  : %f%%    %d/%d",(float)usedHeap/(float)ESP.getHeapSize()*100,usedHeap, ESP.getHeapSize());
	log_d("Used PSRAM : %f%%    %d/%d",(float)usedPSRAM/(float)ESP.getPsramSize()*100,usedPSRAM, ESP.getPsramSize());
}