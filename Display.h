#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

#define FIRSTLINE   5
#define INITLINESPACE 24
#define INITCHARSIZE 17
#ifdef LANDSCAPE
#define INITTYPEPOS 17
#else
#define INITTYPEPOS 23
#endif

#ifdef FONTSIZE_24
#define INFOFONT Font24
#define LINESPACE   24
#define CHARSIZE    17
#ifdef LANDSCAPE
#define TYPEPOS     17
#else
#define TYPEPOS	    23
#endif
#endif
#ifdef FONTSIZE_20
#define INFOFONT Font20
#define LINESPACE   20
#define CHARSIZE    14
#ifdef LANDSCAPE
#define TYPEPOS	    21
#else
#define TYPEPOS	    28
#endif
#endif
#ifdef FONTSIZE_16
#define INFOFONT Font16
#define LINESPACE   16
#define CHARSIZE    11
#ifdef LANDSCAPE
#define TYPEPOS     27
#else
#define TYPEPOS	    36
#endif
#endif
#ifdef FONTSIZE_12
#define INFOFONT Font12
#define LINESPACE   12
#define CHARSIZE    7
#ifdef LANDSCAPE
#define TYPEPOS     42
#else
#define TYPEPOS	    57
#endif
#endif
#ifdef FONTSIZE_8
#define INFOFONT Font8
#define LINESPACE   8
#define CHARSIZE    5
#ifdef LANDSCAPE
#define TYPEPOS     59
#else
#define TYPEPOS	    79
#endif
#endif

class display {
public:
	display();
	display(long leistung, long verbrauch, float batterie, float temp, float feuchte, float druck, float gewicht);
	void begin();
	void NetDone();
	void NetOffline();
	void LoadFrame();
	void LoadFrame(long leistung, long verbrauch, float batterie, float temp, float feuchte, float druck, float gewicht);
	void PrintHeadLine(int linenum, String head);
	void PrintValueLine(int x, int linenum, int left, String name, String type, String value);
	void UpdatePVLeistung(long leistung);
	void UpdatePVLeistung_ost(long verbrauch);
	void UpdatePVLeistung_west(long verbrauch);
	void UpdatePVLeistung_batt(long verbrauch);
	void UpdatePVLeistung_grid(long verbrauch);
	void UpdatePVVerbrauch(long verbrauch);
	void UpdatePVBatterie(float batterie);
	void UpdatePVWallboxWatt(long WallboxWatt);
	void UpdatePVHeizstab(long leistung);
	void UpdateWTTemperatur(float temperatur);
	void UpdateWTFeuchte(float feuchte);
	void UpdateWTDruck(float Druck);
	void UpdatePLGewicht(float Gewicht);
	void UpdateHzLager(float Lager);
	void UpdateHzFehler(String fehler);
	long GetPVLeistung();
	long GetPVLeistung_ost();
	long GetPVLeistung_west();
	long GetPVLeistung_batt();
	long GetPVLeistung_grid();
	long GetPVWallboxWatt();
	long GetPVVerbrauch();
	float GetPVBatterie();
	long GetPVHeizstab();
	float GetWTTemperatur();
	float GetWTFeuchte();
	float GetWTDruck();
	float GetPLGewicht();
	float GetHzLager();
	String GetHzFehler();
	void EnableUpdate();
	bool GetUpdate();
	unsigned int getLastTime();
	void updatelast();
	void SetDate(String datestring);
	String GetDate();

private:
	long PV_Leistung;
	long PV_Leistung_ost;
	long PV_Leistung_west;
	long PV_Leistung_batt;
	long PV_Leistung_grid;
	long PV_Verbrauch;
	long PV_Heizstab;
	float PV_Batterie;
	long PV_WallboxWatt;
	float WT_Temperatur;
	float WT_Feuchte;
	float WT_Druck;
	float PL_Gewicht;
	float HZ_Lager;
	String HZ_Fehler;
	unsigned char *framebuffer;
	Epd epd;
	Paint *paint;
	bool Update=true;
	unsigned int lasttime;
	String date;
};

#endif
