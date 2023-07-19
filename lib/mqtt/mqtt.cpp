#include "mqtt.h"

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Declarations for timestamp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* mqttServer = "a1w2cyy1n5kil8-ats.iot.eu-west-1.amazonaws.com"; // Server URL
String device_id, client_id, thing5_url;

//const String device_id = "4a765346-0ff8-4348-80fb-78113c6ae47c";		// Letto Demo FAVERO
//const String device_id = "2fefc660-4746-4e1b-bafa-84f1f3dfefbf";		// Test
//const String device_id = "f7c850ce-530c-429d-b723-b27efd2a4b68";		// Letto Test FAIRWIND
//const String device_id = "7b68f4dd-db1b-4c3f-b16c-07a32af1d4f3";		// Fairwind
//const String client_id = "things5-production-machine-" + device_id;
//const String thing5_url = "things5-production/v1/devices/" + device_id;

MqttClient::MqttClient(void) {
	connected = false;
}

bool MqttClient::begin(void) {
	String privateKey, caKey, certificateKey;

    console.info(SSL_T, "Reading certificates...");

	//SPIFFS.begin();

	if(!SPIFFS.begin()) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		return false;
	}
	
	/*
	File root = SPIFFS.open("/");
	File file = root.openNextFile();
 
	while(file){
		Serial.print("FILE: ");
		Serial.println(file.name());
		file = root.openNextFile();
	}
	*/

	// Read "device_id.txt" file ------------------------------------------------------------------
	if(SPIFFS.exists(PATH_DEVICE_ID)) {
		File dev_id = SPIFFS.open(PATH_DEVICE_ID, "r");
		while(dev_id.available()){
			char c = dev_id.read();
			device_id += String(c);
		}
		dev_id.close();
		console.info(MQTT_T, "Device ID: \"" + device_id + "\"");
		
		client_id = "things5-production-machine-" + device_id;
		thing5_url = "things5-production/v1/devices/" + device_id;
	}
	else {
		console.error(SSL_T, "File \"" + String(PATH_DEVICE_ID) + "\" does not exists!");
		return false;
	}

	// Read "device.ca.pem" file ------------------------------------------------------------------
	if(SPIFFS.exists(PATH_DEVICE_CA)) {
		File ca = SPIFFS.open(PATH_DEVICE_CA, "r");
		
		if(wifiClient.loadCACert(ca, ca.size())) {
			console.success(SSL_T, "Certificate was loaded");
		}
		else {
			console.error(SSL_T, "Failed to load certificate!");
			return false;
		}
		ca.close();
	}
	else {
		console.error(SSL_T, "File \"" + String(PATH_DEVICE_CA) + "\" does not exists!");
		return false;
	}

	// Read "device.certificate.pem" file ---------------------------------------------------------
	if(SPIFFS.exists(PATH_DEVICE_CERTIFICATE)) {
		File cert = SPIFFS.open(PATH_DEVICE_CERTIFICATE, "r");
		
		if(wifiClient.loadCertificate(cert, cert.size())) {
			console.success(SSL_T, "Certificate was loaded");
		}
		else {
			console.error(SSL_T, "Failed to load certificate!");
			return false;
		}
		cert.close();
	}
	else {
		console.error(SSL_T, "File \"" + String(PATH_DEVICE_CERTIFICATE) + "\" does not exists!");
		return false;
	}

	// Read "device.private.pem" file -------------------------------------------------------------
	if(SPIFFS.exists(PATH_DEVICE_PRV_KEY)) {
		File priv = SPIFFS.open(PATH_DEVICE_PRV_KEY, "r");
		
		if(wifiClient.loadPrivateKey(priv, priv.size())) {
			console.success(SSL_T, "Certificate was loaded");
		}
		else {
			console.error(SSL_T, "Failed to load certificate!");
			return false;
		}
		priv.close();
	}
	else {
		console.error(SSL_T, "File \"" + String(PATH_DEVICE_PRV_KEY) + "\" does not exists!");
		return false;
	}

	SPIFFS.end();

    wifiClient.setTimeout(60);

	return true;
}

bool MqttClient::isConnected(void) {
	return mqttClient.connected();
}

void MqttClient::loop(void) {
	String write_var_req_t = thing5_url + WRITE_VAR_REQ;
    String update_firmware_req_t = thing5_url + UPDATE_FIRMWARE_REQ;

	connected = wifiClient.connected();
	if(!connected) {
		timeClient.begin();
		console.log(SSL_T, "Trying to connect via SSL...");
		console.heap("Before SSL conn.");
		wifiClient.connect(mqttServer, 8883);
		console.heap("After SSL conn.");
	}
	else {
		mqttClient.setServer(thing5_url.c_str(), 1883);
		mqttClient.setCallback(mqttCallback);
		mqttClient.setBufferSize(MQTT_BUFFER_SIZE);

		if(!mqttClient.connected())
		{
			console.log(MQTT_T, "Attempting MQTT connection...");
			// Attempt to connect
			//if (mqttClient.connect(client_id))
			if (mqttClient.connect(client_id.c_str()))
			{
				console.log(MQTT_T, "Connected!");
				// Subscribe
				if(mqttClient.subscribe(write_var_req_t.c_str()))
					console.log(MQTT_T, "+++ Subscribe successfully done on topic \"" + String(WRITE_VAR_REQ) + "\"");
				if(mqttClient.subscribe(update_firmware_req_t.c_str()))
					console.log(MQTT_T, "+++ Subscribe successfully done on topic \"" + String(UPDATE_FIRMWARE_REQ) + "\"");
			}
			else
			{
				console.log(MQTT_T, "FAILED! Reason Code: " + String(mqttClient.state()));
				console.log(MQTT_T, "Try again in 5 seconds...");
			}
		}
		mqttClient.loop();
	}
}

unsigned long long MqttClient::getTimestamp(void)
{
    timeClient.update();
    long long timestamp = timeClient.getEpochTime();
    return (timestamp * 1000);
}

void mqttCallback(char* topic, byte* message, unsigned int length)
{
    DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
    uint8_t index = String(topic).lastIndexOf("/");
    String rxTopic = String(topic).substring(index);
    String rxMessage, payload;

    console.log(MQTT_T, ">>> Message arrived on topic \"" + rxTopic + "\"");

	for (int i = 0; i < length; i++) {
		rxMessage += (char)message[i];
	}

    console.log(MQTT_T, ">>> Message: \"" + rxMessage + "\"");

    // Message parser -------------------------------------
    deserializeJson(doc, rxMessage);

	if(!rxTopic.compareTo("/update_firmware_req"))
	{
		const char* req_id = doc["request_id"];
		const char* url = doc["firmware_url"];
		size_t bin_size = doc["size"];

		/*
		console.log(MAIN_T, "Binary size is" + String(bin_size));
		bool updated = updateFirmware(url, req_id, sendUpdateProgress, sendUpdateSuccess);
		if(updated)
			console.warning(MQTT_T, "Device restarting...");
		else
			console.error(MQTT_T, "Error on update! Device restarting...");
		ESP.restart();
		*/
	}
}

void MqttClient::publish(String topic, String payload)
{
    String extended_topic = thing5_url + topic;

    if(mqttClient.publish(extended_topic.c_str(), payload.c_str()))
        console.log(MQTT_T, "<<< Publish successfully done on topic \"" + topic + "\"");
    else
        console.log(MQTT_T, "--- Publish error on topic \"" + topic + "\"");

    if(!topic.compareTo(String(WRITE_VAR_RES)))
        console.log(MQTT_T, "<<< Payload: \"" + payload + "\"");
}

/*
//void MqttClient::sendUpdateProgress(String uuid, uint8_t progress)
void sendUpdateProgress(String uuid, uint8_t progress)
{
    StaticJsonDocument<MQTT_BUFFER_SIZE> doc;
    String payload;

    doc["request_id"] = uuid;
    doc["progress_percentage"] = progress;

    serializeJson(doc, payload);

	// Publish ----------------------------------------------------------------
	String topic = String(UPDATE_FIRMWARE_PROGRESS);
	String extended_topic = thing5_url + topic;

    if(mqttClient.publish(extended_topic.c_str(), payload.c_str()))
        console.log(MQTT_T, "<<< Publish successfully done on topic \"" + topic + "\" - Progress: " + String(progress) + "%");
    else
        console.log(MQTT_T, "--- Publish error on topic \"" + topic + "\"");
}

//void MqttClient::sendUpdateSuccess(String uuid, bool success)
void sendUpdateSuccess(String uuid, bool success)
{
    StaticJsonDocument<MQTT_BUFFER_SIZE> doc;
    String payload;

    doc["request_id"] = uuid;
    doc["success"] = success;

    serializeJson(doc, payload);

	// Publish ----------------------------------------------------------------
	String topic = String(UPDATE_FIRMWARE_RES);
	String extended_topic = thing5_url + topic;

    if(mqttClient.publish(extended_topic.c_str(), payload.c_str()))
        console.success(MQTT_T, "<<< Publish successfully done on topic \"" + topic + "\" - Success: " + String(success));
    else
        console.error(MQTT_T, "--- Publish error on topic \"" + topic + "\"");
}
*/