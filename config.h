#ifndef __CONFIG_H
#define __CONFIG_H

//NTP Time Server Setup
//#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_NTP_SERVER "ladon.compuextreme.de"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"
//define the access to the FHEM Server
#define FHEM_HOST "rahanas.wlan.compuextreme.de"
#define FHEM_PORT 7072
//define intervall to pull updates out of the fhem server
#define UPDATE_MINUTE 1
#define INITFONT Font24
//#define LANDSCAPE
#define FONTSIZE_24
//#define FONTSIZE_20
//#define FONTSIZE_16
//#define FONTSIZE_12
//#define FONTSIZE_8
#define WAITFHEMAW 1000
#endif
#define APSID IOTAnzeiger
