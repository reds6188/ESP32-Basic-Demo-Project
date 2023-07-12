#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <console.h>

String processor(const String& var);
//extern void initWebServer(AwsTemplateProcessor callback);
extern void initWebServer(String (*func)(void));
void sendJSON(String action, String func, String value);

#endif  /* WEB_SERVER_H_ */