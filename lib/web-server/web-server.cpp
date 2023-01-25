#include "web-server.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void sendJSON(String action, String func, String value) {
	StaticJsonDocument<128> doc;
	String msg;

	doc["action"] = action;
    doc["function"] = func;
	doc["value"] = value;

	serializeJson(doc, msg);
	wsUpdateMsg(msg);
}

void wsUpdateMsg(String msg)
{
    ws.textAll(msg);
}

void initWebSocket(void (*handler)(uint8_t *, size_t)) {
    ws.onEvent([handler](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        AwsFrameInfo *info;
        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("WebSocket client #%u disconnected\n", client->id());
                break;
            case WS_EVT_DATA:
                info = (AwsFrameInfo*)arg;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                    data[len] = 0;
                    handler(data, len);
                }
                break;
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    });
    server.addHandler(&ws);
}

void wsCleanupClients(void) {
    ws.cleanupClients();
}

void initWebServer(AwsTemplateProcessor callback)
{
    if(!SPIFFS.begin(true))
    {
        console.log(HTTP_T, "An Error has occurred while mounting SPIFFS");
        return;
    }

    server.on("^(\\/.+\\/)*(.+)\\.(.+)$", HTTP_GET, [](AsyncWebServerRequest *request){
        String dir = request->pathArg(0);
        String name = request->pathArg(1);
        String ext = request->pathArg(2);
        String MIMEtype = "text/plain";
        String path = dir + name + "." + ext;
        if(!ext.compareTo("svg")) {
            console.log(HTTP_T, "Serving SVG file: " + dir + name);
            MIMEtype = "image/svg+xml";
        }
        else if(!ext.compareTo("css")) {
            console.log(HTTP_T, "Serving CSS file" + dir + name);
            MIMEtype = "text/css";
        }
        else if(!ext.compareTo("js")) {
            console.log(HTTP_T, "Serving JS file" + dir + name);
            MIMEtype = "text/javascript";
        }
        else if(!ext.compareTo("ico")) {
            console.log(HTTP_T, "Serving ICO file" + dir + name);
            MIMEtype = "image/x-icon";
        }
        else
            console.log(HTTP_T, "ERROR: path not recognized");
        request->send(SPIFFS, path, MIMEtype);
    });

    // Route for root / web page
    server.on("/", HTTP_GET, [callback](AsyncWebServerRequest *request){
        console.log(HTTP_T, "Serving index.html");
        //request->send(SPIFFS, "/index.html", "text/html");
        request->send(SPIFFS, "/index.html", String(), false, callback);
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "ERROR 404 - The content you are looking for was not found.");
        console.log(HTTP_T, "ERROR 404 - The content you are looking for was not found.");
    });

    // Start server
    server.begin();
}