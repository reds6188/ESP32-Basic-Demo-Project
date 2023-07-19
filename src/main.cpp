#include "main.h"

Adafruit_BME280 bme; // I2C

weather_t temperature, pressure, humidity;

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

double roundFloat(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

void initParams(void) {
	temperature.current = bme.readTemperature();
	temperature.min = temperature.current;
	temperature.max = temperature.current;

	pressure.current = bme.readPressure() / 100.0F;
	pressure.min = pressure.current;
	pressure.max = pressure.current;

	humidity.current = bme.readHumidity();
	humidity.min = humidity.current;
	humidity.max = humidity.current;
}

String getParams(String command) {
	StaticJsonDocument<384> doc;
	String msg;

	if(command.equals("status")) {
		timeClient.update();
		long long timestamp = timeClient.getEpochTime();

		// Read temperature
		temperature.current = roundFloat(bme.readTemperature());
		if(temperature.current < temperature.min)
			temperature.min = temperature.current;
		else if(temperature.current > temperature.max)
			temperature.max = temperature.current;

		// Read pressure
		pressure.current = roundFloat(bme.readPressure() / 100.0F);
		if(pressure.current < pressure.min)
			pressure.min = pressure.current;
		else if(pressure.current > pressure.max)
			pressure.max = pressure.current;

		// Read humidity
		humidity.current = roundFloat(bme.readHumidity());
		if(humidity.current < humidity.min)
			humidity.min = humidity.current;
		else if(humidity.current > humidity.max)
			humidity.max = humidity.current;

		doc["timestamp"] = timestamp * 1000;

		JsonObject temp = doc.createNestedObject("temperature");
		temp["current"] = String(temperature.current, 1);
		temp["min"] = String(temperature.min, 1);
		temp["max"] = String(temperature.max, 1);

		JsonObject pres = doc.createNestedObject("pressure");
		pres["current"] = String(pressure.current, 1);
		pres["min"] = String(pressure.min, 1);
		pres["max"] = String(pressure.max, 1);

		JsonObject humi = doc.createNestedObject("humidity");
		humi["current"] = String(humidity.current, 1);
		humi["min"] = String(humidity.min, 1);
		humi["max"] = String(humidity.max, 1);
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
	initParams();
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