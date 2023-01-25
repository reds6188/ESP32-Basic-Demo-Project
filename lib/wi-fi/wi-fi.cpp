#include "wi-fi.h"
#include <ESPmDNS.h>

char ssid[100];
char password[100];

long start_wifi_millis;

void setCredentials(const char* new_ssid, const char* new_pswd)
{
    strcpy(ssid, new_ssid);
    strcpy(password, new_pswd);
    console.log(WIFI_T, "Credentials was set successfully!");
    console.log(WIFI_T, "New SSID is: " + String(ssid));
}

bool checkConnection(void)
{
    start_wifi_millis = millis();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        console.write(WIFI_T, ".");
        if(millis() - start_wifi_millis > WIFI_TIMEOUT)
        {
            WiFi.disconnect(true, true);
            return false;
        }
    }
    return true;
}

void scanNetworks(void)
{
    console.log(WIFI_T, "Scanning for new networks...");
    int16_t n = WiFi.scanNetworks();
    console.log(WIFI_T, "Scan done");
    if(n == 0)
    {
        console.log(WIFI_T, "No networks found!");
    }
    else
    {
        console.log(WIFI_T, String(n) + " network(s) found:");
        for(int16_t i=0 ; i<n ; i++)
        {
            String encriptionType = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
            String rssi = String(WiFi.RSSI(i));
            console.log(WIFI_T, String(i) + ": " + WiFi.SSID(i) + " (" + rssi + " dBm) " + encriptionType);
        }
        console.log(WIFI_T, "Note: networks with \"*\" char are encrypted");
    }
}

void initWiFi(wifi_mode_t mode)
{
    console.log(WIFI_T, "Starting Wi-Fi configuration...");
    if(mode == WIFI_AP)
    {
        IPAddress localIP(192, 168, 4, 1);    // Set your Static IP address
        IPAddress gatewayAddr(192, 168, 4, 0);  // Set your Gateway IP address
        IPAddress subnetMask(255, 255, 255, 0);   // Set your Subnet Mask

        console.log(WIFI_T, "Setting AP (Access Point)...");
        WiFi.mode(WIFI_AP);

        if(!WiFi.softAP(ssid, password))
        {
            console.log(WIFI_T, "AP failed to start");
        }
        delay(100);
        if(!WiFi.softAPConfig(localIP, gatewayAddr, subnetMask))
        {
            console.log(WIFI_T, "AP configuration failed");
        }
        else
        {
            console.log(WIFI_T, "AP started!");
            console.log(WIFI_T, "Network SSID: " + String(ssid));
            IPAddress IP = WiFi.softAPIP();
            console.log(WIFI_T, "AP IP address: " + IP.toString());
        }
    }
    else if(mode == WIFI_STA)
    {
        console.log(WIFI_T, "Setting STA (STAtion mode)...");
        WiFi.mode(WIFI_STA);
        while(!checkConnection())
        {
            console.log(WIFI_T, "Error: failed to connect.");
            console.log(WIFI_T, "Connection restart...");
        }
        console.log(WIFI_T, "Wi-Fi connected!");
        console.log(WIFI_T, "Network SSID: " + String(ssid));
        IPAddress IP = WiFi.localIP();
        console.log(WIFI_T, "Local IP address: " + IP.toString());
    }
    else
    {
        console.log(WIFI_T, "ERROR! Invalid Wi-Fi operating mode");
    }

    const char* host_name = "esp32";

    if(MDNS.begin(host_name))
    {
        //Serial.println("http://esp32.local/");
        MDNS.addService("http", "tcp", 80);
        console.log(WIFI_T, "MDNS start, open the browser and type \"http://" + String(host_name) + ".local\"");
    }
}