#include "main.h"

Led RedLed(R_LED_PIN, LS_DRIVER);
Led GrnLed(G_LED_PIN, LS_DRIVER);
Led BluLed(B_LED_PIN, LS_DRIVER);

Button Btn1(BTN_1_PIN, 80);
Button Btn2(BTN_2_PIN, 80);

void toggleRedLed(void) {
	RedLed.toggle();
	console.log(MAIN_T, "Red LED was toggled");
}

void toggleBlueLed(void) {
	BluLed.toggle();
	console.log(MAIN_T, "Blue LED was toggled");
}

void setup() {
	RedLed.set();
	GrnLed.set();
	BluLed.set();
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	console.log(MAIN_T, "Compile Date: " + String(__DATE__));
	console.log(MAIN_T, "Compile Time: " + String(__TIME__));
	console.log(MAIN_T, "ESP32 Chip Model: " + String(ESP.getChipModel()));
	console.log(MAIN_T, "ESP32 Flash Size: " + String(ESP.getFlashChipSize()));
	console.header("END INITIALIZATION", DOUBLE_DASHED, 80);
	Btn1.onPress(toggleRedLed);
	Btn2.onPress(toggleBlueLed);
	RedLed.reset();
	GrnLed.setBlink(500, 500);
	BluLed.reset();
}

void loop() {
	Btn1.loop();
	Btn2.loop();
	GrnLed.loop();	// only for blinking
}