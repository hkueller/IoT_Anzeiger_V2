#ifndef __NETWORK_H
#define __NETWORK_H

#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Display.h>
#include <time.h>
#include "sntp.h"
#include <config.h>
#include <Data.h>

class network {
private:
	String header;
	WiFiServer *server;
	WiFiClient *fhemclient=0;
	WiFiManager wifimanager;
	tm zeit;
public:
	network();
	void begin();
	int testNet(display *disp);
	void handleWeb(smarthome *data);
	void UpdateData(display *disp, smarthome *data);
	int FhemGetData(String *result,const String device, const String reading, long unsigned int *lasttime);
	//WiFiClient * FhemConnect();
	int FhemConnect();
	void FhemDisconnect();
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
