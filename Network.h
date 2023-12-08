#ifndef __NETWORK_H
#define __NETWORK_H

#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Display.h>
#include <time.h>
#include <config.h>

class network {
private:
	String header;
	WiFiServer *server;
	WiFiClient *client;
	WiFiServer *netvalues;
	WiFiManager wifimanager;
	tm zeit;
public:
	network();
	void begin();
	int testNet(display *disp);
	void handleWeb(display *disp);
	void UpdateData(display *disp);
	int FhemGetData(String *result,const String device, const String reading, long unsigned int *lasttime);
	WiFiClient * FhemConnect();
	void handleInc(display *disp);
	void handleOTA();
	void StoreTimeLastRead();
	String twodigit(int number);
	int ReturnYear();
	int ReturnMonth();
	int ReturnDay();
	int ReturnHour();
	int ReturnMinutes();
	String ReturnSeconds();
};
#endif
