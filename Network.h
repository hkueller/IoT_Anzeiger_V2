#ifndef __NETWORK_H
#define __NETWORK_H

#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Display.h>
#include <config.h>

class network {
private:
	String header;
	WiFiServer *server;
	WiFiClient *client;
	WiFiServer *netvalues;
	WiFiManager wifimanager;
public:
	network();
	void begin();
	void testNet();
	void handleWeb(display *disp);
	void UpdateData(display *disp);
	WiFiClient * FhemConnect();
	void handleInc(display *disp);
	void handleOTA();
};
#endif
