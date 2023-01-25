#include "rgb-led.h"

RgbLed::RgbLed(int r_pin, int g_pin, int b_pin, led_driver_t led_driver_mode) {
	
	driver_mode = led_driver_mode;

	LedR.setPin(r_pin);
	LedG.setPin(g_pin);
	LedB.setPin(b_pin);

	LedR.setDriverMode(driver_mode);
	LedG.setDriverMode(driver_mode);
	LedB.setDriverMode(driver_mode);
}

void RgbLed::setColor(c8_color_t color) {
	bool r = (color & 0x01);
	bool g = ((color >> 1) & 0x01);
	bool b = ((color >> 2) & 0x01);

	LedR.setState(driver_mode == LS_DRIVER ? r : !r);
	LedG.setState(driver_mode == LS_DRIVER ? g : !g);
	LedB.setState(driver_mode == LS_DRIVER ? b : !b);
}

void RgbLed::setBlink(c8_color_t color1, c8_color_t color2, unsigned long time1, unsigned long time2) {
	blink[0].color = color1;
	blink[0].time = time1;
	blink[1].color = color2;
	blink[1].time = time2;

	blink_index = false;
	setColor(color1);
}

void RgbLed::loop(void) {
	
	if((millis() - timer_blink) > blink[blink_index].time) {
		timer_blink = millis();
		blink_index = !blink_index;
		setColor(blink[blink_index].color);
	}
}