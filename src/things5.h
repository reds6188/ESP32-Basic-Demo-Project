#ifndef THINGS5_H_
#define THINGS5_H_

#include <Arduino.h>
#include <ESPRandom.h>
#include <ArduinoJson.h>

#define JSON_SIZE           2048

typedef struct {
    String label;
	float value;
}t5_data_t;

//String createMetricsPayload(unsigned long long timestamp, String * name, uint8_t * value, data_type_t * type, size_t size);
String createMetricsPayload(unsigned long long timestamp, t5_data_t * data, size_t size);
String createEventsPayload(unsigned long long timestamp, t5_data_t * data, size_t size);

#endif	// THINGS5_H_