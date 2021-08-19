#include "WifiCapProgram.h"
esp_err_t event_handler(void *ctx, system_event_t *event){ return ESP_OK; }
std::vector<Packet*> WifiCapProgram::savedPackets = std::vector<Packet*>();
uint32_t WifiCapProgram::captureStartTime = 0;
int WifiCapProgram::capturedEapolPackets = 0;
int WifiCapProgram::capturedBeaconPackets = 0;
int WifiCapProgram::capturedProbeReqPackets = 0;
int WifiCapProgram::capturedProbeResPackets = 0;
MacAddr WifiCapProgram::macca = MacAddr({0xd0,0x17,0xc2,0xb2,0x39,0x58});
void WifiCapProgram::init() {
	watch = TTGOClass::getWatch();
	tft = watch->tft;
	tft->fillScreen(TFT_BLACK);

	//nvs_flash_init();
	//tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(sniffer);
	wifi_second_chan_t secondCh = (wifi_second_chan_t)NULL;
	esp_wifi_set_channel(1,secondCh);
	
}

bool WifiCapProgram::isPacketRepeat(wifi_promiscuous_pkt_t* pkt) {
	if(pkt->payload[1]>>6==0x01) {
		return true;
	} else {
		return false;
	}
}

WifiCapProgram::PacketType WifiCapProgram::getPacketType(wifi_promiscuous_pkt_t* pkt) {
	uint8_t* payload = pkt->payload;
	if(((payload[30]==0x88 && payload[31]==0x8e) ||
	   (payload[31]==0x88 && payload[32]==0x8e) ||
	   (payload[32]==0x88 && payload[33]==0x8e))) {
		   return EAPOL;
	}
	if(payload[0]==0x80) {return BEACON;}
	if(payload[0]==0x40) {return PROBE_REQUEST;}
	if(payload[0]==0x50) {return PROBE_RESPONSE;}
	if(payload[0]==0x08) {return DATA;}

	return NONE;
}

bool WifiCapProgram::doesBssidMatch(wifi_promiscuous_pkt_t* pkt,MacAddr address) {
	return (
	(pkt->payload[4] == address.mac[0] &&
	pkt->payload[5] == address.mac[1] &&
	pkt->payload[6] == address.mac[2] &&
	pkt->payload[7] == address.mac[3] &&
	pkt->payload[8] == address.mac[4] &&
	pkt->payload[9] == address.mac[5])
	||
	(pkt->payload[10] == address.mac[0] &&
	pkt->payload[11] == address.mac[1] &&
	pkt->payload[12] == address.mac[2] &&
	pkt->payload[13] == address.mac[3] &&
	pkt->payload[14] == address.mac[4] &&
	pkt->payload[15] == address.mac[5])
	||
	(pkt->payload[16] == address.mac[0] &&
	pkt->payload[17] == address.mac[1] &&
	pkt->payload[18] == address.mac[2] &&
	pkt->payload[19] == address.mac[3] &&
	pkt->payload[20] == address.mac[4] &&
	pkt->payload[21] == address.mac[5])
	);
}

void WifiCapProgram::sniffer(void *buf, wifi_promiscuous_pkt_type_t pktType) {
	
	wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
	PacketType type = getPacketType(pkt);
	
	if(doesBssidMatch(pkt,macca) && (type != NONE && type != DATA && type != PROBE_REQUEST)){
		
		//TODO Add probe req packets

		if(type==EAPOL && capturedEapolPackets<maxEapolPackets) {
			capturedEapolPackets++;
		} else if(type==BEACON && capturedBeaconPackets<maxBeaconPackets) {
			capturedBeaconPackets++;
		} else if(type==PROBE_RESPONSE && capturedProbeResPackets<maxProbeResPackets) {
			capturedProbeResPackets++;
		} else {
			return;
		}

		// Eapol :   Router BSSID ?/Router Receiver
		// Beacon:   Router Source BSSID
		// ProbeReq: Not needed(Devices looking for wifi)
		// ProbeRes: Router BSSID

		// Total: Router BSSID       ?/Router Receiver


		/*
		//d0:17:c2:b2:39:58   ROUTER BSSID
		(pkt->payload[4] == 0xd0 &&
		pkt->payload[5] == 0x17 &&
		pkt->payload[6] == 0xc2 &&
		pkt->payload[7] == 0xb2 &&
		pkt->payload[8] == 0x39 &&
		pkt->payload[9] == 0x58)

		//d0:17:c2:b2:39:58   ROUTER Receiver
		(pkt->payload[10] == 0xd0 &&
		pkt->payload[11] == 0x17 &&
		pkt->payload[12] == 0xc2 &&
		pkt->payload[13] == 0xb2 &&
		pkt->payload[14] == 0x39 &&
		pkt->payload[15] == 0x58)
		
		//28:16:7f:e0:9f:ab  PHONE RECIEVER
		(pkt->payload[4] == 0x28 &&
		pkt->payload[5] == 0x16 &&
		pkt->payload[6] == 0x7f &&
		pkt->payload[7] == 0xe0 &&
		pkt->payload[8] == 0x9f &&
		pkt->payload[9] == 0xab)

		//28:16:7f:e0:9f:ab  PHONE TRANSMITTER
		(pkt->payload[10] == 0x28 &&
		pkt->payload[11] == 0x16 &&
		pkt->payload[12] == 0x7f &&
		pkt->payload[13] == 0xe0 &&
		pkt->payload[14] == 0x9f &&
		pkt->payload[15] == 0xab)
		
		*/
		if(capturedEapolPackets == 0 && capturedBeaconPackets == 0 && capturedProbeReqPackets == 0 && capturedProbeResPackets == 0) {
			WifiCapProgram::captureStartTime = micros();
		}
		wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;
		uint32_t len = ctrl.sig_len;
		if(pktType == WIFI_PKT_MGMT) {
			len -= 4;
		}

		uint32_t timestamp = micros()/1000000;
		uint32_t microseconds = micros() - WifiCapProgram::captureStartTime;
		uint32_t origLen = len;
		uint32_t inclLen = len;
		if(inclLen > snaplen) inclLen = snaplen;
		Packet* p = new Packet(timestamp,microseconds,origLen,inclLen,pkt->payload);
		savedPackets.push_back(p);
	}
	
}

void WifiCapProgram::printStatus(int status) { //TODO add back serial writing so it can change itself without input
	if(status == 0) {
		tft->setTextColor(TFT_GREEN,TFT_BLACK);
	} else if(status == 1) {
		tft->setTextColor(TFT_BLUE,TFT_BLACK);
	} else if(status == 2) {
		tft->setTextColor(TFT_RED,TFT_BLACK);
	}
	tft->setTextSize(3);
	tft->setTextDatum(CC_DATUM);
	tft->drawString("Eapol: " + (String)capturedEapolPackets + "/" + (String)maxEapolPackets,120,50);
	tft->drawString("Beacon: " + (String)capturedBeaconPackets + "/" + (String)maxBeaconPackets,120,100);
	tft->drawString("Probe Req: " + (String)capturedProbeReqPackets + "/" + (String)maxProbeReqPackets,120,150);
	tft->drawString("Probe Res: " + (String)capturedProbeResPackets + "/" + (String)maxProbeResPackets,120,200);
}

void WifiCapProgram::update() {
	if(millis() - timeTouched > 1000) {
		printStatus(0);
	} else {
		printStatus(2);
	}


	bool touched = watch->touched();
	if(touched && !lastTouched && millis() - timeTouched > 1000) {
		lastTouched = true;
		timeTouched = millis();
		printStatus(1);

		Serial.println("<<<StartFile>>>");
		Serial.flush();

		//.pcap file header
		serialwrite_32(magic_number);
		serialwrite_16(version_major);
		serialwrite_16(version_minor);
		serialwrite_32(thiszone);
		serialwrite_32(sigfigs);
		serialwrite_32(snaplen);
		serialwrite_32(network);
		Serial.flush();
		
		//Send packets over serial
		for(int i = 0; i < savedPackets.size(); i++){
			serialwrite_32(savedPackets.at(i)->timestamp);
			serialwrite_32(savedPackets.at(i)->microseconds);
			serialwrite_32(savedPackets.at(i)->inclLen);
			serialwrite_32(savedPackets.at(i)->origLen);
			Serial.write(savedPackets.at(i)->buf,savedPackets.at(i)->inclLen);
			
		}
		Serial.flush();
		std::vector<Packet*>().swap(savedPackets);
		captureStartTime = 0;
		capturedEapolPackets = 0;
		capturedBeaconPackets = 0;
		capturedProbeReqPackets = 0;
		capturedProbeResPackets = 0;
		printStatus(2);
	}
	if(!touched) {
		lastTouched = false;
	}
	delay(10);
}

void WifiCapProgram::sleepUpdate() {

}

ProgramSleepMode WifiCapProgram::getSleepMode()
{
	return ProgramUpdateSleep;
}

void WifiCapProgram::sleep() {

}

void WifiCapProgram::wakeup() {

}

void WifiCapProgram::exit() {

}