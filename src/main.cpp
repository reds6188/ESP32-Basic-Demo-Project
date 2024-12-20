#include "main.h"

Adafruit_BME280 bme; // I2C
WiFiHandler wifi_handler(wifi_ssid, wifi_password);
MqttClient	mqtt_client;
Timer TimerIdle, TimerMQTT;
weather_t temperature, pressure, humidity;
t5_data_t t5_metrics[3];

RgbLed rgb_led(R_LED_PIN, G_LED_PIN, B_LED_PIN, LS_DRIVER);

Button btn1(BTN_1_PIN, 80);
Button btn2(BTN_2_PIN, 80);

// Declarations for timestamp
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);

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
	//StaticJsonDocument<384> doc;
	DynamicJsonDocument doc(384);
	String msg;

	if(command.equals("status")) {
		/*
		timeClient.update();
		long long timestamp = timeClient.getEpochTime();
		*/

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

		//doc["timestamp"] = timestamp * 1000;
		doc["timestamp"] = mqtt_client.getTimestamp();

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

bool freq_flag;

void changeFreq(void)
{
	if(freq_flag)
	{
		freq_flag = false;
		ledcChangeFrequency(1, 100, 8);
		ledcChangeFrequency(2, 200, 8);
		ledcChangeFrequency(3, 300, 8);
	}
	else
	{
		freq_flag = true;
		ledcChangeFrequency(1, 50, 8);
		ledcChangeFrequency(2, 250, 8);
		ledcChangeFrequency(3, 1000, 8);
	}
}

void setup() {
	rgb_led.setColor(C8_BLACK);
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	console.info(MAIN_T, "ESP32 Chip Model: " + String(ESP.getChipModel()));
	console.info(MAIN_T, "ESP32 Flash Size: " + String(ESP.getFlashChipSize()));
	console.info(MAIN_T, "Software Version: " + String(VERSION));
	console.info(MAIN_T, "Compile Date: " + String(__DATE__));
	console.info(MAIN_T, "Compile Time: " + String(__TIME__));
	wifi_handler.begin(WIFI_STA, "fairwind");
	mqtt_client.begin();
	initWebServer(getParams);
	bool status = bme.begin(0x76);
	initParams();
	console.header("END INITIALIZATION", DOUBLE_DASHED, 80);
	//btn1.onPress(toggleRedLed);
	//btn2.onPress(toggleBlueLed);
	btn1.onPress(changeFreq);
	btn2.onPress(changeFreq);
	rgb_led.setBlink(C8_GREEN, C8_BLACK, 500, 500);

	ledcSetup(0, 200, 8);
	ledcSetup(1, 600, 8);
	ledcSetup(2, 450, 8);
	ledcAttachPin(5, 0);
	ledcAttachPin(18, 1);
	ledcAttachPin(19, 2);
	ledcWrite(0, 127);
	ledcWrite(1, 127);
	ledcWrite(2, 127);
}

void loop() {
	btn1.loop();
	btn2.loop();
	rgb_led.loop();

	if(TimerIdle.elapsedX1ms(100)) {
		TimerIdle.trigger();
		wifi_handler.loop();
		if(wifi_handler.connected()) {
			mqtt_client.loop();
		}
	}

	if(TimerMQTT.elapsedX1s(5)) {
		TimerMQTT.trigger();
		if(mqtt_client.isConnected()) {
			t5_metrics[0].label = "temperature";
			t5_metrics[0].value = temperature.current;
			t5_metrics[1].label = "humidity";
			t5_metrics[1].value = humidity.current;
			t5_metrics[2].label = "pressure";
			t5_metrics[2].value = pressure.current;
			unsigned long long timestamp = mqtt_client.getTimestamp();
			String payload = createMetricsPayload(timestamp, t5_metrics, 3);
			mqtt_client.publish(DATA_INGESTION_REQ, payload);
		}
	}
}