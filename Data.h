#ifndef _DATA_H
#define _DATA_H

#include <Arduino.h>
//#include <epd4in2.h>
#include <epdpaint.h>
#include <config.h>

#define MSG true
#define DISP false

void removequote(String *data);
class smarthome {

struct SH_MSG_FONT {
	sFONT TextFont=Font24;
	int fontwidth=17;
	int fontheight=24;
};

struct SH_FONT {
	sFONT TextFont;
	int fontwidth;
	int fontheight;
};

struct SH_DATA {
        //Some pointers to manage memory
	void *first;
	void *next;
	void *prev;
	//Name which will be displayed
	char *name;
	//Name of the device in FHEM
	char *device;
	//FHEM Reading of device to use
	char *reading;
	//Data of this reading will be stored in "data"
	char data[128];
	char *einheit;
	//Give a line number where to display it on the
	//E-Ink display - 0 for only display on web page
	int line;
	int pos;
	bool headline;
};

struct SH_SETUP {
	int LayOut=0;
	int width=DISPLAY_WIDTH;
	int height=DISPLAY_HEIGHT;
	int NumOfLines=0;
	int *LinePos=NULL;
	SH_FONT TextFont;
	SH_MSG_FONT MsgFont;
};

public:
	smarthome();
	smarthome(String *show_name, int line);
	smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit); 
	smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit, int disp_line, int disp_pos); 
	smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit, int disp_line, int disp_pos); 
	bool add(String *show_name,int line);
	bool add(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit); 
	bool add(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit, int disp_line, int disp_pos); 
	bool add(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit, int disp_line, int disp_pos); 
	String GetName();
	String GetDev();
	String GetReading();
	String GetData();
	String GetEinheit();
	bool SetFont(int size);
	bool SetOrientation(int orientation);
	int GetOrientation();
	int GetWidth();
	int GetHeight();
	int GetLine();
	int GetPos();
	int GetCharWidth(bool msg);
	int GetCharHeight(bool msg);
	sFONT *GetFont(bool msg);
	bool SetDashNum(int NumOfLine);
	int GetDashNum();
	bool SetDashPos(int num, int pos);
	int GetDashPos(int LineNum);
	bool SetData(String data);
	bool SetLine(int line);
	bool SetFirst();
	void SetLast();
	bool SetNext();
	bool IsHeadline();
	bool EntryExists(String *name);
	bool DelEntry(String name);

private:
	SH_DATA * sh_data=NULL;
	SH_SETUP sh_setup;
	bool block;
};
#endif
