#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <console.h>

#define HTTP_T      "HTTP"  // HTTP console tag

extern void initWebServer(String (*func)(void));

#endif  /* WEB_SERVER_H_ */