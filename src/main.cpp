#include "main.h"

RgbLed rgb_led(R_LED_PIN, G_LED_PIN, B_LED_PIN, LS_DRIVER);

Button btn1(BTN_1_PIN, 80);
Button btn2(BTN_2_PIN, 80);

void toggleRedLed(void) {
	rgb_led.setBlink(C8_RED, C8_BLACK, 500, 500);
	console.log(MAIN_T, "Red LED was toggled");
}

void toggleBlueLed(void) {
	rgb_led.setBlink(C8_BLUE, C8_BLACK, 500, 500);
	console.log(MAIN_T, "Blue LED was toggled");
}

void setup() {
	rgb_led.setColor(C8_BLACK);
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	console.log(MAIN_T, "Compile Date: " + String(__DATE__));
	console.log(MAIN_T, "Compile Time: " + String(__TIME__));
	console.log(MAIN_T, "ESP32 Chip Model: " + String(ESP.getChipModel()));
	console.log(MAIN_T, "ESP32 Flash Size: " + String(ESP.getFlashChipSize()));
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