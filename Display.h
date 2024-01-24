#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <Data.h>

#define COLORED     0
#define UNCOLORED   1

#define FIRSTLINE   5
//font height in pixel
#define INITLINESPACE 24
//fondwith in pixel
#define INITCHARSIZE 17
//number of chars per line
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
	display(long leistung, long leistung_ost, long leistung_west, long leistung_batt, long leistung_grid, long verbrauch, long leistung_heizstab, float batterie, long leistung_wallbox, float temp, float feuchte, float druck, float gewicht, float hz_gewicht, String hz_fehler);
	void begin();
	void begin(smarthome *config);
	void Message(String message);
	void Message(String message, smarthome *config);
	void LoadFrame(smarthome *data);
	void PrintHeadLine(String head, String data, smarthome *config);
	void PrintHeadLine(int linenum, String head);
	void PrintHeadLine(int linenum, String head, smarthome *config);
	void PrintValueLine(int x, int linenum, int left, String name, String type, String value);
	void PrintValueLine(int x, int linenum, int left, String name, String type, String value, smarthome *config);
	void EnableUpdate();
	bool GetUpdate();
	unsigned int getLastTime();
	void updatelast();
	void SetDate(String datestring);
	String GetDate();

private:
	unsigned char *framebuffer;
	Epd epd;
	Paint *paint;
	bool Update=true;
	unsigned int lasttime;
	String date;
};

#endif
