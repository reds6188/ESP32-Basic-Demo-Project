#ifndef MAIN_H_
#define MAIN_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <Adafruit_BME280.h>
#include <timer.h>
#include <console.h>
#include <rgb-led.h>
#include <button.h>
#include <wi-fi.h>
#include <web-server.h>
#include <mqtt.h>
#include "def.h"
#include "credentials.h"
#include "things5.h"

typedef struct {
    float current;
    float min;
    float max;
}weather_t;

#endif  /* MAIN_H_ */