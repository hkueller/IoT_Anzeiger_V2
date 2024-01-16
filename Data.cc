#include "Data.h"
#include <config.h>

smarthome::smarthome() {
	sh_data=NULL;
};

smarthome::smarthome(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit){
	add(show_name, fhem_dev, fhem_reading, String(""), einheit, 0, 0);
};

smarthome::smarthome(String show_name, String fhem_dev, String fhem_reading, String einheit, int disp_line, int disp_pos) {
	add(show_name, fhem_dev, fhem_reading, String(""), disp_line, disp_pos);
};

smarthome::smarthome(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit, int disp_line, int disp_pos) {
	add(show_name, fhem_dev, fhem_reading, fhem_data, disp_line, disp_pos);
};
	
bool smarthome::add(String show_name, int line) {
	bool state=false;
	state=add(show_name,String(""),String(""),String(""),line,0);
	if ( state ) {
#ifdef DEBUG
	Serial.println("Setting headline to true");
	Serial.flush();
	delay(100);
#endif
		sh_data->headline=true;
	} 
	return state;
}

bool smarthome::add(String show_name, String fhem_dev, String fhem_reading, String einheit){
	return(add(show_name, fhem_dev, fhem_reading, String(""), einheit, 0, 0));
};

bool smarthome::add(String show_name, String fhem_dev, String fhem_reading, String einheit, int disp_line, int disp_pos){
	return(add(show_name, fhem_dev, fhem_reading, String(""), einheit, disp_line, disp_pos));
};
bool smarthome::add(String show_name, String fhem_dev, String fhem_reading, String fhem_data, String einheit, int disp_line, int disp_pos) {
#ifdef DEBUG
	Serial.println("Adding Entry for" + show_name);
	Serial.flush();
#endif
	SH_DATA *temp;
	if ( ! sh_data ) {
#ifdef DEBUG
		Serial.println("Requesting MEM for first entry");
		Serial.flush();
#endif
		sh_data = (SH_DATA *) malloc(sizeof(SH_DATA));
#ifdef DEBUG
		Serial.println("Setting Pointers");
		Serial.flush();
#endif
		if ( sh_data ) {
			sh_data->first = (void *) sh_data;
			sh_data->next = NULL;
			sh_data->prev = NULL;
		} else {
			return(false);
		}
		yield();
	} else {
		sh_data = (SH_DATA *) sh_data->first;
#ifdef DEBUG
		Serial.println("Searching last entry");
		Serial.flush();
#endif
		while ( sh_data->next ) {
			sh_data = (SH_DATA *) sh_data->next;
		}
		yield();
#ifdef DEBUG
		Serial.println("Requesting MEM");
		Serial.flush();
#endif
		temp = (SH_DATA *) malloc(sizeof(SH_DATA));
		if ( temp) {
#ifdef DEBUG
			Serial.println("Setting Pointers");
			Serial.flush();
#endif
			temp->first = (void *) sh_data->first;
			temp->next = NULL;
			temp->prev = (void *) sh_data;
			sh_data->next = (void *) temp;
			sh_data = (SH_DATA *) temp;
		} else {
#ifdef DEBUG
			Serial.println("Did not get mem!");
			Serial.flush();
#endif
			return(false);
		}
		yield();
	}
#ifdef DEBUG
	Serial.println("Adding " + show_name + "  to entrie");
	Serial.flush();
#endif
	sh_data->name = show_name;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + fhem_dev + "  to entrie");
	Serial.flush();
#endif
	sh_data->device = fhem_dev;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + fhem_reading + "  to entrie");
	Serial.flush();
#endif
	sh_data->reading = fhem_reading;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + fhem_data + "  to entrie");
	Serial.flush();
#endif
	sh_data->data = fhem_data;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + einheit + "  to entrie");
	Serial.flush();
#endif
	sh_data->einheit = einheit;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + String(disp_line) + "  to entrie");
	Serial.flush();
#endif
	sh_data->line = disp_line;
	yield();
#ifdef DEBUG
	Serial.println("Adding " + String(disp_pos) + "  to entrie");
	Serial.flush();
#endif
	sh_data->pos = disp_pos;
	yield();
#ifdef DEBUG
	Serial.println("Setting headline to false");
	Serial.flush();
	yield();
#endif
	//delay(1000);
	yield();
	//delay(500);
	sh_data->headline=false;
	return(true);
};

void smarthome::SetFirst() {
	sh_data = (SH_DATA *) sh_data->first;
};

void smarthome::SetLast() {
	while(sh_data->next) {
		sh_data = (SH_DATA * ) sh_data->next;
	}
};

bool smarthome::SetNext() {
	if(sh_data->next) {
		sh_data = (SH_DATA *) sh_data->next;
		return(true);
	} else {
		return(false);
	}
};

String smarthome::GetName() {
	return sh_data->name;
};

String smarthome::GetDev() {
	return sh_data->device;
};

String smarthome::GetReading() {
	return sh_data->reading;
};

String smarthome::GetData() {
	return sh_data->data;
};

String smarthome::GetEinheit() {
	return sh_data->einheit;
}

int smarthome::GetLine() {
	return sh_data->line;
};

int smarthome::GetPos() {
	return sh_data->pos;
}

bool smarthome::SetData(String data) {
	sh_data->data = data;
	return true;
};

bool smarthome::SetLine(int line) {
	sh_data->line = line;
	return true;
}

bool smarthome::IsHeadline() {
	return sh_data->headline;
}

