#ifndef __CONFIG_H
#define __CONFIG_H

//#define DEBUG
/*
 * Network Setup
 */

//NTP Time Server Setup
//#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_NTP_SERVER "ladon.compuextreme.de"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"
//define the access to the FHEM Server
#define FHEM_HOST "rahanas.wlan.compuextreme.de"
#define FHEM_PORT 7072
#define FHEM_RETRY 2
//define what is needed for mqtt config access
#define MQTT_SERVER "rahanas.wlan.compuextreme.de"
#define MQTT_PORT 1883
#define MQTT_REQUEST_TOPIC "display/get_config"
#define MQTT_SUBSCRIBE_TOPIC "display/+"
#define MQTT_DEPTH 40
/*
 * Display Setup
 */
//4.2" Waveshare E-Ink
#define DISPLAY_HEIGHT 400
#define DISPLAY_WIDTH 300
//define intervall to pull updates out of the fhem server
#define UPDATE_MINUTE 1
#define APSID "IOT Anzeiger"
#endif
