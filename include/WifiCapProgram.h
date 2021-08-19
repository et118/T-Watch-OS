#pragma once
#include <config.h>
#include "Program.h"
#include "Arduino.h"
#include <GraphicsLib.h>
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "lwip/err.h"
#include <SPI.h>
#include <TimeLib.h>
#include <vector>


class Packet {
	public:
		Packet(uint32_t timestamp,uint32_t microseconds,int inclLen,int origLen,uint8_t* buf) {
			this->timestamp = timestamp;
			this->microseconds = microseconds;
			this->inclLen = inclLen;
			this->origLen = origLen;
			this->buf = (uint8_t*)malloc(origLen);
			memcpy(this->buf,buf,inclLen);
		};
		~Packet() {
			delete this->buf;
		}
		uint32_t timestamp;
		uint32_t microseconds;
		int inclLen;
		int origLen;
		uint8_t *buf;
};

typedef struct {
    uint8_t mac[6];
} __attribute__((packed)) MacAddr;

class WifiCapProgram : public Program {
	private:
		TFT_eSPI *tft;
		TTGOClass *watch;
		bool lastTouched = true;
		
		long timeTouched = 0;

		
		static const uint32_t magic_number = 0xa1b2c3d4;
		static const uint16_t version_major = 2;
		static const uint16_t version_minor = 4;
		static const uint32_t thiszone = 0;
		static const uint32_t sigfigs = 0;
		static const uint32_t snaplen = 2500;
		static const uint32_t network = 105;

		/* converts a 32 bit integer into 4 bytes */
		void escape32(uint32_t n, uint8_t* buf){
			buf[0] = n;
			buf[1] = n >>  8;
			buf[2] = n >> 16;
			buf[3] = n >> 24;
		}

		/* converts a 16 bit integer into 2 bytes */
		void escape16(uint16_t n, uint8_t* buf){
			buf[0] = n;
			buf[1] = n >>  8;
		}

		/* writes a 32 bit integer to Serial */
		void serialwrite_32(uint32_t n){
			uint8_t _buf[4];
			escape32(n, _buf);
			Serial.write(_buf, 4);
		}

		/* writes a 16 bit integer to Serial */
		void serialwrite_16(uint16_t n){
			uint8_t _buf[2];
			escape16(n, _buf);
			Serial.write(_buf, 2);
		}

		void printStatus(int status);
	public:
		using Program::Program;
		enum PacketType {
			NONE,
			EAPOL,
			BEACON,
			PROBE_REQUEST,
			PROBE_RESPONSE,
			DATA
		};
		void init();
		void update();
		void sleepUpdate();
		ProgramSleepMode getSleepMode();
		void sleep();
		void wakeup();
		void exit();
		static bool isPacketRepeat(wifi_promiscuous_pkt_t* pkt);
		static PacketType getPacketType(wifi_promiscuous_pkt_t*pkt);
		static bool doesBssidMatch(wifi_promiscuous_pkt_t*pkt,MacAddr mac);
		static void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);
		
		
		static std::vector<Packet*> savedPackets;
		
		static uint32_t captureStartTime;
		static int capturedEapolPackets;
		static int capturedBeaconPackets;
		static int capturedProbeReqPackets;
		static int capturedProbeResPackets;
		static const int maxEapolPackets = 4;
		static const int maxBeaconPackets = 10;
		static const int maxProbeReqPackets = 0;
		static const int maxProbeResPackets = 10;
		static MacAddr macca;
};