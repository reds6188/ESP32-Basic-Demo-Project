#ifndef MQTT_H_
#define MQTT_H_

#include <Arduino.h>
#include <console.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <ESPRandom.h>

#define MQTT_T						"MQTT"	// Tag for log
#define SSL_T						"SSL"   // Tag for log

#define MQTT_BUFFER_SIZE			2048

#define DATA_INGESTION_REQ			"/data_req"
#define DATA_INGESTION_RES			"/data_res"
#define WRITE_VAR_REQ				"/cmd/write_variables_req"
#define WRITE_VAR_RES				"/cmd/write_variables_res"
#define UPDATE_FIRMWARE_REQ			"/cmd/update_firmware_req"
#define UPDATE_FIRMWARE_RES			"/cmd/update_firmware_res"
#define UPDATE_FIRMWARE_PROGRESS	"/cmd/update_firmware_progress"

#define PATH_DEVICE_ID				"/cert/device_id.txt"
#define PATH_DEVICE_CA				"/cert/device.ca.pem"
#define PATH_DEVICE_CERTIFICATE		"/cert/device.certificate.pem"
#define PATH_DEVICE_PRV_KEY			"/cert/device.private.pem"

class MqttClient {
	private:
		bool connected;
		bool connecting;
	public:
		MqttClient(void);
		bool begin(void);
		bool isConnected(void);
		void loop(void);
		unsigned long long getTimestamp(void);
		void publish(String topic, String payload);
};

void mqttCallback(char* topic, byte* message, unsigned int length);
/*
void sendUpdateProgress(String uuid, uint8_t progress);
void sendUpdateSuccess(String uuid, bool success);
*/
#endif  /* MQTT_H_ */