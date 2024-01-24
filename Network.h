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
#include <PubSubClient.h>
#include <config.h>
#include <Data.h>

void callback(char* topic, byte* payload, unsigned int length);

class network {
private:
	String header;
	String clientId;
	tm zeit;

	WiFiManager wifimanager;
	WiFiServer *server;
	WiFiClient *fhemclient=0;
	WiFiClient mqtt_client;
	void reconnect();
	PubSubClient *mqtt;
public:
	network();
	void begin();
	int testNet(display *disp);
	void handleWeb(smarthome *data);
	void UpdateData(display *disp, smarthome *data);
	//int FhemGetData(String *result,const String device, const String reading, long unsigned int *lasttime);
	int FhemGetData(String *result,const String device, const String reading);
	//WiFiClient * FhemConnect();
	int FhemConnect();
	void FhemDisconnect();
	void handleOTA();
	void handleNet(smarthome *sm, display *disp);
	void StoreTimeLastRead();
	String twodigit(int number);
	int ReturnYear();
	int ReturnMonth();
	int ReturnDay();
	int ReturnHour();
	int ReturnMinutes();
	String ReturnSeconds();
	void RequestConfig();
	void ConfigDone();
	long unsigned int lasttime;
};
#endif
