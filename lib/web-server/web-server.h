#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <console.h>

String processor(const String& var);
extern void initWebServer(AwsTemplateProcessor callback);
//extern void initWebSocket(AwsEventHandler event);
void sendJSON(String action, String func, String value);
//void initWebSocket(void (*handler)(uint8_t *data, size_t len));
void initWebSocket(void (*handler)(uint8_t *, size_t));
extern void wsCleanupClients(void);
//extern void wsUpdateClients(void);
//extern void wsUpdateStatus(void);
extern void wsUpdateMsg(String msg);

#endif  /* WEB_SERVER_H_ */