#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

class display {
public:
	display();
	display(long leistung, long verbrauch, int batterie, float temp, float feuchte, float druck, float gewicht);
	void begin();
	void LoadFrame();
	void LoadFrame(long leistung, long verbrauch, int batterie, float temp, float feuchte, float druck, float gewicht);
	void UpdatePVLeistung(long leistung);
	void UpdatePVLeistung_ost(long verbrauch);
	void UpdatePVLeistung_west(long verbrauch);
	void UpdatePVLeistung_batt(long verbrauch);
	void UpdatePVLeistung_grid(long verbrauch);
	void UpdatePVVerbrauch(long verbrauch);
	void UpdatePVBatterie(int batterie);
	void UpdatePVHeizstab(long leistung);
	void UpdateWTTemperatur(float temperatur);
	void UpdateWTFeuchte(float feuchte);
	void UpdateWTDruck(float Druck);
	void UpdatePLGewicht(float Gewicht);
	long GetPVLeistung();
	long GetPVLeistung_ost();
	long GetPVLeistung_west();
	long GetPVLeistung_batt();
	long GetPVLeistung_grid();
	long GetPVVerbrauch();
	int GetPVBatterie();
	long GetPVHeizstab();
	float GetWTTemperatur();
	float GetWTFeuchte();
	float GetWTDruck();
	float GetPLGewicht();
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
	int PV_Batterie;
	float WT_Temperatur;
	float WT_Feuchte;
	float WT_Druck;
	float PL_Gewicht;
	unsigned char *framebuffer;
	Epd epd;
	Paint *paint;
	bool Update=true;
	unsigned int lasttime;
	String date;
};

#endif
