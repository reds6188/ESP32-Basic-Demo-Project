#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <Arduino.h>
#include <led.h>

/*
typedef enum {
    LS_DRIVER = 0,      // Low-Side Driver
    HS_DRIVER = 1       // High-Side Driver
} led_driver_t;
*/


// 8-Color
typedef enum {
    C8_BLACK = 0,
    C8_RED,
    C8_GREEN,
    C8_YELLOW,
    C8_BLUE,
    C8_FUCHSIA,
    C8_CYAN,
    C8_WHITE,
} c8_color_t;

typedef struct {
    int r_led;
    int g_led;
    int b_led;
} rgb_led_t;

typedef struct {
    c8_color_t color;
    unsigned long time;
} led_blink_t;

class RgbLed {
	private:
        Led LedR;
        Led LedG;
        Led LedB;
        rgb_led_t pin;
        led_driver_t driver_mode;
        led_blink_t blink[2];
        unsigned long timer_blink;
        bool blink_index;
	public:
        RgbLed(int r_pin, int g_pin, int b_pin, led_driver_t led_driver_mode);
        void setColor(c8_color_t color);
        void setBlink(c8_color_t color1, c8_color_t color2, unsigned long time1, unsigned long time2);
        void loop(void);
};

#endif  /* RGB_LED_H_ */