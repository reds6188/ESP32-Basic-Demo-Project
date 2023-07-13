#include "main.h"

Adafruit_BME280 bme; // I2C

RgbLed rgb_led(R_LED_PIN, G_LED_PIN, B_LED_PIN, LS_DRIVER);

Button btn1(BTN_1_PIN, 80);
Button btn2(BTN_2_PIN, 80);

// Declarations for timestamp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void toggleRedLed(void) {
	rgb_led.setBlink(C8_RED, C8_BLACK, 500, 500);
	console.log(MAIN_T, "Red LED was toggled");
}

void toggleBlueLed(void) {
	rgb_led.setBlink(C8_BLUE, C8_BLACK, 500, 500);
	console.log(MAIN_T, "Blue LED was toggled");
}

String htmlProcessor(const String& var) {
    console.log(MAIN_T, "var = " + var);
    String strProc = String();
	
	/*
	Insert here how to process HTML template
	"var" is the string %PLACEHOLDER_NAME%
	*/

    console.log(MAIN_T, "strProc = " + strProc);
	return strProc;
}

String getParams(String command) {
	StaticJsonDocument<128> doc;
	String msg;

	if(command.equals("status")) {
		timeClient.update();
		long long timestamp = timeClient.getEpochTime();

		float temperature = bme.readTemperature();
		float pressure = bme.readPressure() / 100.0F;
		float humidity = bme.readHumidity();

		doc["timestamp"] = timestamp * 1000;
		doc["temperature"] = temperature;
		doc["pressure"] = pressure;
		doc["humidity"] = humidity;
	}
	else if(command.equals("version")) {
		doc["version"] = String(VERSION);
		doc["date"] = String(__DATE__);
	}
	else {
		doc["info"] = "Command not recognized";
	}

	serializeJson(doc, msg);
	return msg;
}

void setup() {
	rgb_led.setColor(C8_BLACK);
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	console.info(MAIN_T, "ESP32 Chip Model: " + String(ESP.getChipModel()));
	console.info(MAIN_T, "ESP32 Flash Size: " + String(ESP.getFlashChipSize()));
	console.info(MAIN_T, "Software Version: " + String(VERSION));
	console.info(MAIN_T, "Compile Date: " + String(__DATE__));
	console.info(MAIN_T, "Compile Time: " + String(__TIME__));
	setCredentials(wifi_ssid, wifi_password);
	initWiFi(WIFI_STA, "fairwind");
	//initWebServer(&htmlProcessor);
	initWebServer(getParams);
	bool status = bme.begin(0x76);
	console.header("END INITIALIZATION", DOUBLE_DASHED, 80);
	btn1.onPress(toggleRedLed);
	btn2.onPress(toggleBlueLed);
	rgb_led.setBlink(C8_GREEN, C8_BLACK, 500, 500);
}

void loop() {
	btn1.loop();
	btn2.loop();
	rgb_led.loop();
}