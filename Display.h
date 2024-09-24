#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <Data.h>

#define COLORED     0
#define UNCOLORED   1

#define FIRSTLINE   5

class display {
public:
	display();
	void begin(smarthome *config);
	void Message(String message, smarthome *config);
	void Message(String line1, String line2, smarthome *config);
	void LoadFrame(smarthome *data);
	void PrintHeadLine(String head, String data, smarthome *config);
	void PrintHeadLine(int linenum, String head, smarthome *config);
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
	//Epd epd(5,0,15,4);
	Paint *paint;
	bool Update=true;
	unsigned int lasttime;
	String date;
};

#endif
