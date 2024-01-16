#ifndef _DATA_H
#define _DATA_H

#include <Arduino.h>

class smarthome {

public:
	smarthome();
	smarthome(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit); 
	smarthome(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit, int disp_line, int disp_pos); 
	smarthome(String show_name, String fhem_dev, String fhem_reading, String einheit, int disp_line, int disp_pos); 
	bool add(String show_name,int line);
	bool add(String show_name, String fhem_dev, String fhem_reading, String einheit); 
	bool add(String show_name, String fhem_dev, String fhem_reading, String einheit, int disp_line, int disp_pos); 
	bool add(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit, int disp_line, int disp_pos); 
	String GetName();
	String GetDev();
	String GetReading();
	String GetData();
	String GetEinheit();
	int GetLine();
	int GetPos();
	bool SetData(String data);
	bool SetLine(int line);
	void SetFirst();
	void SetLast();
	bool SetNext();
	bool IsHeadline();

private:
	struct SH_DATA {
	        //Some pointers to manage memory
		void *first;
		void *next;
		void *prev;
		//Name which will be displayed
		String name;
		//Name of the device in FHEM
		String device;
		//FHEM Reading of device to use
		String reading;
		//Data of this reading will be stored in "data"
		String data;
		String einheit;
		//Give a line number where to display it on the
		//E-Ink display - 0 for only display on web page
		int line;
		int pos;
		bool headline;
	};
	SH_DATA * sh_data=NULL;
};
#endif
