#include "wi-fi.h"

char ssid[100];
char password[100];

long start_wifi_millis;

WiFiHandler::WiFiHandler(const char* ssid, const char* password) {
	strcpy(this->ssid, ssid);
	strcpy(this->password, password);
}

void WiFiHandler::begin(wifi_mode_t mode, const char* host_name) {
	console.log(WIFI_T, "Starting Wi-Fi configuration...");
	if(mode == WIFI_AP)
	{
		IPAddress localIP(192, 168, 4, 1);		// Set your Static IP address
		IPAddress gatewayAddr(192, 168, 4, 0);	// Set your Gateway IP address
		IPAddress subnetMask(255, 255, 255, 0);	// Set your Subnet Mask

		console.log(WIFI_T, "Setting AP (Access Point)...");
		WiFi.mode(WIFI_AP);

		if(!WiFi.softAP(ssid, password, 1, 0, 1, false))
			console.log(WIFI_T, "AP failed to start");
        delay(100);
		if(!WiFi.softAPConfig(localIP, gatewayAddr, subnetMask))
			console.log(WIFI_T, "AP configuration failed");
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
		WiFi.begin(ssid, password);
	}
	else
	{
		console.log(WIFI_T, "ERROR! Invalid Wi-Fi operating mode");
	}

    if(MDNS.begin(host_name))
    {
        MDNS.addService("http", "tcp", 80);
        console.log(WIFI_T, "MDNS start, open the browser and type \"http://" + String(host_name) + ".local\"");
    }
}

void WiFiHandler::loop(void) {
	wl_status_t curr_status = WiFi.status();
	if(curr_status != status)
    {
		wifi_timeout = millis();
		status = curr_status;
		switch(status) {
			case WL_IDLE_STATUS:		console.log(WIFI_T, "Idle status");							break;
			case WL_NO_SSID_AVAIL:		console.error(WIFI_T, "No SSID available");					break;
			case WL_SCAN_COMPLETED:		console.success(WIFI_T, "Network scan was completed");		break;
			case WL_CONNECTED:			console.success(WIFI_T, "Connection was established");		break;
			case WL_CONNECT_FAILED:		console.error(WIFI_T, "Connection failed");					break;
			case WL_CONNECTION_LOST:	console.warning(WIFI_T, "Lost connection");					break;
			case WL_DISCONNECTED:		console.warning(WIFI_T, "Disconnected");					break;
			default:					console.error(WIFI_T, "Unknown status: " + String(status));	break;
		}
    }
	else
	{
		if(curr_status != WL_CONNECTED)
		{
			if(millis() - wifi_timeout > WIFI_TIMEOUT) {
				WiFi.disconnect(true, true);
				wifi_timeout = millis();
				console.error(WIFI_T, "Timeout has expired.");
				WiFi.begin(ssid, password);
			}
			else
				console.log(WIFI_T, "Trying to connect... ");
		}
	}
}

bool WiFiHandler::connected(void) {
	if(WiFi.status() == WL_CONNECTED)
		return true;

	return false;
}

void WiFiHandler::scanNetworks(void)
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