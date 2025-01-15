#include "Data.h"

smarthome::smarthome() {
	sh_data=NULL;
	SetFont(24);
	SetMsgFont(24);
	SetOrientation(0);
#ifdef DEBUG
	Serial.println("smarthome: unblock data");
	Serial.flush();
#endif
	block=false;
};

smarthome::smarthome(String *show_name, int line) {
	add(show_name,line);
	SetFont(24);
	SetMsgFont(24);
	SetOrientation(0);
};

smarthome::smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit){
	String buffer = "";
	add(show_name, fhem_dev, fhem_reading, &buffer, einheit, 0, 0);
	SetFont(24);
	SetMsgFont(24);
	SetOrientation(0);
};

smarthome::smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit, int disp_line, int disp_pos) {
	String buffer = "";
	add(show_name, fhem_dev, fhem_reading, &buffer, disp_line, disp_pos);
	SetFont(24);
	SetMsgFont(24);
	SetOrientation(0);
};

smarthome::smarthome(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit, int disp_line, int disp_pos) {
	add(show_name, fhem_dev, fhem_reading, fhem_data, disp_line, disp_pos);
	SetFont(24);
	SetMsgFont(24);
	SetOrientation(0);
};
	
bool smarthome::add(String *show_name, int line) {
	bool state=false;
	String buffer = "";
	state=add(show_name,&buffer,&buffer,&buffer,line,0);
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

bool smarthome::add(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit){
	String buffer = "";
	return(add(show_name, fhem_dev, fhem_reading, &buffer, einheit, 0, 0));
};

bool smarthome::add(String *show_name, String *fhem_dev, String *fhem_reading, String *einheit, int disp_line, int disp_pos){
	String buffer = "";
	return(add(show_name, fhem_dev, fhem_reading, &buffer, einheit, disp_line, disp_pos));
};
bool smarthome::add(String *show_name, String *fhem_dev, String *fhem_reading, String *fhem_data, String *einheit, int disp_line, int disp_pos) {
	while ( block ) {
		yield();
		delay(100);
	}
#ifdef DEBUG
        Serial.println("add: blocking data");
	Serial.flush();
#endif
	block=true;
#ifdef DEBUG
	Serial.println("Adding Entry for " + *show_name);
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
#ifdef DEBUG
			Serial.println("Did not get mem!");
			Serial.println("add: unblock data");
			Serial.flush();
#endif
			block=false;
			return(false);
		}
	} else {
		sh_data = (SH_DATA *) sh_data->first;
#ifdef DEBUG
		Serial.println("Searching last entry");
		Serial.flush();
#endif
		while ( sh_data->next ) {
			sh_data = (SH_DATA *) sh_data->next;
		}
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
			Serial.println("add: unblock data");
			Serial.flush();
#endif
			block=false;
			return(false);
		}
	}
#ifdef DEBUG
	Serial.println("Adding >" + *show_name + "< to entry name");
	Serial.flush();
#endif
	sh_data->name = (char *) malloc(sizeof(char)*(show_name->length()+1));
	strcpy(sh_data->name,show_name->c_str());
#ifdef DEBUG
	Serial.println("Adding >" + *fhem_dev + "< to entry device");
	Serial.flush();
#endif
	sh_data->device = (char *) malloc(sizeof(char)*(fhem_dev->length()+1));
	strcpy(sh_data->device,fhem_dev->c_str());
#ifdef DEBUG
	Serial.println("Adding >" + *fhem_reading + "< to entry reading");
	Serial.flush();
#endif
	sh_data->reading = (char *) malloc(sizeof(char)*(fhem_reading->length()+1));
	strcpy(sh_data->reading,fhem_reading->c_str());
#ifdef DEBUG
	Serial.println("Adding >" + *fhem_data + "< to entry data");
	Serial.flush();
#endif
	strcpy(sh_data->data,fhem_data->c_str());
#ifdef DEBUG
	Serial.println("Adding <" + *einheit + "> to entry einheit");
	Serial.flush();
#endif
	sh_data->einheit = (char *) malloc(sizeof(char)*(einheit->length()+1));
	strcpy(sh_data->einheit,einheit->c_str());
#ifdef DEBUG
	Serial.println("Adding >" + String(disp_line) + "< to entry line number");
	Serial.flush();
#endif
	sh_data->line = disp_line;
#ifdef DEBUG
	Serial.println("Adding >" + String(disp_pos) + "< to entry position");
	Serial.flush();
#endif
	sh_data->pos = disp_pos;
#ifdef DEBUG
	Serial.println("Setting headline to false");
	Serial.flush();
#endif
	//delay(1000);
	//delay(500);
	sh_data->headline=false;
#ifdef DEBUG
	Serial.println("add: unblock data");
	Serial.flush();
#endif
	block=false;
	return(true);
};

bool smarthome::SetFirst() {
	while ( block ) {
		yield();
		delay(100);
	}
	block=true;
	if ( ! sh_data ) {
		block=false;
		return false;
	}
	sh_data = (SH_DATA *) sh_data->first;
	block=false;
	return true;
};

void smarthome::SetLast() {
	if ( ! sh_data )  {
		return;
	}
	while(sh_data->next) {
		sh_data = (SH_DATA * ) sh_data->next;
	}
};

bool smarthome::SetNext() {
	if ( ! sh_data ) {
		return(false);
	}
	if(sh_data->next) {
		sh_data = (SH_DATA *) sh_data->next;
		return(true);
	} else {
		return(false);
	}
};

String smarthome::GetName() {
	if ( ! sh_data ) {
		return(String("FAIL"));
	}
	return String(sh_data->name);
};

String smarthome::GetDev() {
	if ( ! sh_data ) {
		return(String("FAIL"));
	}
	return String(sh_data->device);
};

String smarthome::GetReading() {
	if ( ! sh_data ) {
		return(String("FAIL"));
	}
	return String(sh_data->reading);
};

String smarthome::GetData() {
	if ( ! sh_data ) {
		return(String("FAIL"));
	}
	return String(sh_data->data);
};

String smarthome::GetEinheit() {
	if ( ! sh_data ) {
		return(String("FAIL"));
	}
	return String(sh_data->einheit);
}

int smarthome::GetLine() {
	if ( ! sh_data ) {
		return(-1);
	}
	return sh_data->line;
};

int smarthome::GetPos() {
	if ( ! sh_data ) {
		return(-1);
	}
	return sh_data->pos;
}

bool smarthome::SetData(String data) {
	if ( ! sh_data ) {
		return(false);
	}
	strcpy(sh_data->data,data.c_str());
	return true;
};

bool smarthome::SetLine(int line) {
	if ( ! sh_data ) {
		return(false);
	}
	sh_data->line = line;
	return true;
}

bool smarthome::IsHeadline() {
	if ( ! sh_data ) {
		return(false);
	}
	return sh_data->headline;
}

bool smarthome::EntryExists(String *name) {
	String buffer;
#ifdef DEBUG
	Serial.println("loading first entry");
	Serial.flush();
#endif
	while ( block ) {
		yield();
		delay(100);
	}
#ifdef DEBUG
        Serial.println("EntryExists: blocking data");
	Serial.flush();
#endif
	block=true;
	if ( ! sh_data) {
#ifdef DEBUG
		Serial.println("EntryExists: unblock data");
		Serial.flush();
#endif
		block=false;
		return false;
	}

	sh_data=(SH_DATA *) sh_data->first;
#ifdef DEBUG
	Serial.println("copy sh_data->name to buffer");
	Serial.flush();
#endif
	buffer=String(sh_data->name);
#ifdef DEBUG
	Serial.println("compare first entry");
	Serial.flush();
#endif
	if ( buffer.compareTo(name->c_str()) == 0 ) {
#ifdef DEBUG
		Serial.println("first entry matches");
		Serial.println("EntryExists: unblock data");
		Serial.flush();
#endif
		block=false;
		return true;
	} else {
#ifdef DEBUG
		Serial.println("Not the firstone, going through all entrys");
		Serial.flush();
#endif
		yield();
		while(sh_data->next) {
			sh_data = (SH_DATA *) sh_data->next;
			buffer=sh_data->name;
			if ( buffer.compareTo(*name) == 0 ) {
#ifdef DEBUG
				Serial.println("Entry found!");
				Serial.println("EntryExists: unblock data");
				Serial.flush();
#endif
				block=false;
				return true;
			}
		}
		yield();
	}
#ifdef DEBUG
	Serial.println("EntryExists: unblock data");
	Serial.flush();
#endif
	block=false;
	return false;
}

bool smarthome::DelEntry(String name) {
	SH_DATA *todelete;
	SH_DATA *prev;

#ifdef DEBUG
	Serial.println("loading first entry");
	Serial.flush();
#endif
	while ( block ) {
		yield();
		delay(100);
	}
#ifdef DEBUG
        Serial.println("EntryExists: blocking data");
	Serial.flush();
#endif
	block=true;

	if ( ! sh_data ) {
#ifdef DEBUG
		Serial.println("EntryExists: unblocking data");
		Serial.flush();
#endif
		return(false);
	}
	sh_data=(SH_DATA *) sh_data->first;
	if ( String(sh_data->name).compareTo(name) == 0 ) {
		todelete=sh_data;
		sh_data=(SH_DATA *) sh_data->next;
		sh_data->prev=NULL;
		sh_data->first=sh_data;
		while (sh_data->next) {
			sh_data=(SH_DATA *) sh_data->next;
			sh_data->first=todelete->next;
		}
		delete(todelete->name);
		delete(todelete->device);
		delete(todelete->reading);
		delete(todelete->einheit);
		delete(todelete);
#ifdef DEBUG
		Serial.println("EntryExists: unblocking data");
		Serial.flush();
#endif
		block=false;
		return(true);
	} else {
		while(sh_data->next) {
			sh_data = (SH_DATA *) sh_data->next;
			if ( String(sh_data->name).compareTo(name) == 0) {
				todelete=sh_data;
				sh_data=(SH_DATA *) sh_data->next;
				prev=(SH_DATA *) todelete->prev;
				sh_data->prev=(void *) prev;
				prev->next=(void *) sh_data;
				delete(todelete->name);
				delete(todelete->device);
				delete(todelete->reading);
				delete(todelete->einheit);
				delete(todelete);
#ifdef DEBUG
				Serial.println("EntryExists: unblocking data");
				Serial.flush();
#endif
				block=false;
				return(true);
			}
		}
	}
#ifdef DEBUG
	Serial.println("EntryExists: unblocking data");
	Serial.flush();
#endif
	block=false;
	return(false);
}

void smarthome::SetFont(SH_FONT *font, int size) {
#ifdef DEBUG
	Serial.println("Setting up font to size: " + size);
	Serial.flush();
#endif
	font->fontheight = size;
	font->TextFont = Font24;
	font->fontwidth=17;
	if ( size == 20 ) {
		font->TextFont=Font20;
		font->fontwidth=14;
	}
	if ( size == 16 ) {
		font->TextFont=Font16;
		font->fontwidth=11;
	}
	if ( size == 12 ) {
		font->TextFont=Font12;
		font->fontwidth=7;
	}
	if ( size == 8 ) {
		font->TextFont=Font8;
		font->fontwidth=5;
	}
}

bool smarthome::SetFont(int size) {
	SetFont(&sh_setup.TextFont, size);
	return true;
}

bool smarthome::SetMsgFont(int size) {
	SetFont(&sh_setup.MsgFont, size);
	return true;
}

bool smarthome::SetOrientation(int orientation) {
#ifdef DEBUG
	Serial.print("Setting Orientation to ");
	Serial.println(orientation);
	Serial.flush();
#endif
	sh_setup.LayOut=0;
	sh_setup.width=DISPLAY_HEIGHT;
	sh_setup.height=DISPLAY_WIDTH;
	if ( orientation == 3) {
		sh_setup.LayOut=orientation;
		sh_setup.width=DISPLAY_WIDTH;
		sh_setup.height=DISPLAY_HEIGHT;
	}
	return true;
}

int smarthome::GetOrientation() {
	return sh_setup.LayOut;
}

int smarthome::GetWidth() {
	return sh_setup.width;
}

int smarthome::GetHeight() {
	return sh_setup.height;
}

bool smarthome::SetDashNum(int NumOfLine) {
#ifdef DEBUG
	Serial.println("Setting num of lines to: " + NumOfLine);
	Serial.flush();
#endif
	if (NumOfLine > sh_setup.NumOfLines ) {
		if ( sh_setup.LinePos ) {
			delete sh_setup.LinePos;
			sh_setup.LinePos=NULL;
		}
		sh_setup.NumOfLines=NumOfLine;
		sh_setup.LinePos=(int *) malloc(sizeof(int)*sh_setup.NumOfLines);
		if ( ! sh_setup.LinePos ) {
			return false;
		}
		for(int i=0; i<sh_setup.NumOfLines; i++) {
			sh_setup.LinePos[i]=0;
		}
		return true;
	} else {
		return false;
	}
}

int smarthome::GetDashNum() {
	return sh_setup.NumOfLines;
}

bool smarthome::SetDashPos(int num, int pos) {
#ifdef DEBUG
	Serial.print("Setting up Dash Line: ");
	Serial.print(num);
	Serial.print(" With position: ");
	Serial.println(pos);
	Serial.flush();
#endif
	if(!sh_setup.LinePos) {
		sh_setup.LinePos[num] = pos;
		return(true);
	} else {
		return(false);
	}
}

int smarthome::GetDashPos(int num) {
	if(sh_setup.LinePos) {
		return(sh_setup.LinePos[num]);
	} else {
		return 0;
	}
}

int smarthome::GetCharWidth(bool msg) {
	if ( msg ) {
		return(sh_setup.MsgFont.fontwidth);
	} else {
		return(sh_setup.TextFont.fontwidth);
	}
}

int smarthome::GetCharHeight(bool msg) {
	if ( msg ) {
		return(sh_setup.MsgFont.fontheight);
	} else {
		return(sh_setup.TextFont.fontheight);
	}
}

sFONT *smarthome::GetFont(bool msg) {
	if ( msg ) {
		return(&sh_setup.MsgFont.TextFont);
	} else {
		return(&sh_setup.TextFont.TextFont);
	}
}

void smarthome::SetMqtt(String data, String topic) {
	for(int mqtt_data=0; mqtt_data < MQTT_DEPTH; mqtt_data ++) {
		if (! mqtt_new[mqtt_data].avail) {
			mqtt_new[mqtt_data].data=data;
			mqtt_new[mqtt_data].topic=topic;
			mqtt_new[mqtt_data].avail=true;
			break;
		}
	}
}

int smarthome::HandleMQTT() {
	String topic_number;
	String name;
	String data;
	String dev;
	String reading;
	String einheit;
	String pos;
	int return_code=0;
	int deli=0;

	for ( int mqtt_data=0; mqtt_data < MQTT_DEPTH; mqtt_data ++) {
		if(mqtt_new[mqtt_data].avail) {
			return_code=1;
			if ( mqtt_new[mqtt_data].topic.compareTo("conf_done") == 0 ) {
				return_code=2;
			}
			if ( mqtt_new[mqtt_data].topic.compareTo("conf") == 0 ) {
	#ifdef DEBUG
				Serial.println("Loading configuration with MQTT");
	#endif
				deli=mqtt_new[mqtt_data].data.indexOf(",");
				if( deli > 1) {
					name=mqtt_new[mqtt_data].data.substring(0,deli);
					data=mqtt_new[mqtt_data].data.substring(deli+1);
					removequote(&name);
	#ifdef DEBUG
					Serial.println("name contain: " +name);
	#endif
					SetOrientation(name.toInt());
				}
	#ifdef DEBUG
				Serial.println("after orientation, data is: " + data);
	#endif
				removequote(&data);
				SetFont(data.toInt());
	#ifdef DEBUG
				Serial.println("Fontheight is set to: " + String(GetCharHeight(false)));
	#endif
			}
			if ( mqtt_new[mqtt_data].topic.compareTo("dash") == 0 ) {
	#ifdef DEBUG
				Serial.println("loading dashes to draw");
	#endif
				deli=mqtt_new[mqtt_data].data.indexOf(",");
				if(deli > -1) {
					name=mqtt_new[mqtt_data].data.substring(0,deli);
					data=mqtt_new[mqtt_data].data.substring(deli+1);
					removequote(&name);
					//sh_setup.NumOfLines=name.toInt();
					SetDashNum(name.toInt());
				}
				for(int i = 0; i < GetDashNum(); i++) {
					deli=data.indexOf(",");
					if(deli > -1) {
						name=data.substring(0,deli);
						data=data.substring(deli,1);
					}
					removequote(&name);
					//sh_setup.LinePos[i] = name.toInt();
					SetDashPos(i,name.toInt());
				}
			}
			deli=mqtt_new[mqtt_data].topic.indexOf("_");
			if ( deli > -1 ) {
				topic_number=mqtt_new[mqtt_data].topic.substring(deli+1);
				mqtt_new[mqtt_data].topic=mqtt_new[mqtt_data].topic.substring(0,deli);
			}
	#ifdef DEBUG
			Serial.println("check topic: " + mqtt_new[mqtt_data].topic + "\n" + "check line:  " + topic_number);
	#endif
			if ( mqtt_new[mqtt_data].topic.compareTo("line") == 0 ) {
				deli=mqtt_new[mqtt_data].data.indexOf(",");
				if(deli > -1) {
					name=mqtt_new[mqtt_data].data.substring(0,deli);
					data=mqtt_new[mqtt_data].data.substring(deli+1);
					removequote(&name);
					deli=data.indexOf(",");
					if(deli > -1) {
						dev=data.substring(0,deli);
						data=data.substring(deli+1);
					} else {
						dev=data;
					}
					removequote(&dev);
					deli=data.indexOf(",");
					if(deli > -1) {
						reading=data.substring(0,deli);
						data=data.substring(deli+1);
					} else {
						reading=data;
					}
					removequote(&reading);
					deli=data.indexOf(",");
					if(deli > -1) {
						einheit=data.substring(0,deli);
						data=data.substring(deli+1);
					} else {
						einheit=data;
					}
					removequote(&einheit);
					deli=data.indexOf(",");
					if(deli > -1) {
						pos=data.substring(0,deli);
						data=data.substring(deli+1);
					} else {
						pos=data;
					}
					removequote(&pos);
					if ( ! EntryExists(&name) ) {
						add(&name,&dev,&reading,&einheit,topic_number.toInt(),pos.toInt());
						yield();
					}
				} else {
	  				name=mqtt_new[mqtt_data].data;
					while(name.indexOf('"') > -1) {
						deli=name.indexOf('"');
						if ( deli == 0 ) {
							name=name.substring(deli+1);
						} else {
							name=name.substring(0,deli);
						}
					}
					if ( ! EntryExists(&name) ) {
						add(&name,topic_number.toInt());
						yield();
					}
	  			}
		#ifdef DEBUG
				Serial.println("Name: " + name);
		#endif
			}
			//put handling for data here
			mqtt_new[mqtt_data].avail=false;
		}
	}
	return(return_code);
}
