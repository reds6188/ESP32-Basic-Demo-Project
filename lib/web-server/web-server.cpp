#include "web-server.h"

AsyncWebServer server(80);

void initWebServer(String (*func)(String))
{
    if(!SPIFFS.begin())
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
    //server.on("/", HTTP_GET, [callback](AsyncWebServerRequest *request){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        console.log(HTTP_T, "Serving index.html");
        request->send(SPIFFS, "/index.html", "text/html");
        //request->send(SPIFFS, "/index.html", String(), false, callback);
    });

    server.on("/status", HTTP_GET, [func](AsyncWebServerRequest *request) {
        console.log(HTTP_T, "Status request");
        String payload = func("status");
        console.log(HTTP_T, payload);
        request->send(200, "text/json", payload);
    });

        server.on("/version", HTTP_GET, [func](AsyncWebServerRequest *request) {
        console.log(HTTP_T, "Status request");
        String payload = func("version");
        console.log(HTTP_T, payload);
        request->send(200, "text/json", payload);
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        console.log(HTTP_T, "ERROR 404 - The content you are looking for was not found.");
        request->send(404, "text/plain", "ERROR 404 - The content you are looking for was not found.");
    });

    // Start server
    server.begin();
}