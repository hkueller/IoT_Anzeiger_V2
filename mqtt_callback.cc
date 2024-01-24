#include <Data.h>
#include <Network.h>

extern bool mutex;
extern network net;
extern smarthome sm;

void removequote(String *data) {
	int pos;
	while(data->indexOf('"') > -1) {
		pos=data->indexOf('"');
		if ( pos == 0 ) {
			*data=data->substring(pos+1);
		} else {
			*data=data->substring(0,pos);
		}
	}
}

void callback(char* topic, byte* payload, unsigned int length) {
	while(mutex) {
		yield();
		delay(100);
 	}
	mutex=true;
	String topic_pre;
	String topic_number;
	String data;
	String name;
	String dev;
	String reading;
	String einheit;
	String pos;
	int komma;
#ifdef DEBUG
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	Serial.flush();
#endif
	for (int i = 0; i < (int) length; i++) {
#ifdef DEBUG
		Serial.print((char)payload[i]);
#endif
		data+=(char) payload[i];
	}
#ifdef DEBUG
	Serial.println();
#endif
	if ( String(topic).indexOf("/") > 0 ) {
		topic_pre=String(topic).substring(String(topic).indexOf("/")+1);
	}
#ifdef DEBUG
	Serial.println("sub topic: " + topic_pre);
	Serial.flush();
#endif
	if ( topic_pre.compareTo("conf") == 0 ) {
#ifdef DEBUG
		Serial.println("loading configuration with mqtt");
		Serial.flush();
#endif
		komma=data.indexOf(",");
		if( komma > -1 ) {
			name=data.substring(0,komma);
			data=data.substring(komma+1);
			removequote(&name);
#ifdef DEBUG
			Serial.println("name contain: " + name);
			Serial.flush();
#endif
			sm.SetOrientation(name.toInt());
		}
		/*name=data.substring(0,data.indexOf(","));
		data=data.substring(data.indexOf(",")+1);*/
#ifdef DEBUG
		Serial.println("after orientation data is: " + data);
		Serial.flush();
#endif
		removequote(&data);
		sm.SetFont(data.toInt());
#ifdef DEBUG
		Serial.print("Fontheight is set to: ");
		Serial.println(sm.GetCharHeight(false));
		Serial.flush();
#endif
	}
	if ( topic_pre.compareTo("dash") == 0 ) {
#ifdef DEBUG
		Serial.println("loading dashes to draw");
		Serial.flush();
#endif
		komma=data.indexOf(",");
		if(komma > -1 ) {
			name=data.substring(0,komma);
			data=data.substring(komma+1);
			removequote(&name);
			sm.SetDashNum(name.toInt());
		}
		for (int i = 0; i < sm.GetDashNum(); i++) {
			komma=data.indexOf(",");
			name=data.substring(0,komma);
			data=data.substring(komma+1);
			removequote(&name);
			sm.SetDashPos(i,name.toInt());
		}
	}
	komma=topic_pre.indexOf("_");
	if ( komma > 0 ) {
 		topic_number=topic_pre.substring(komma+1);
		topic_pre=topic_pre.substring(0,komma);
	}
#ifdef DEBUG
	Serial.println("check topic: " + topic_pre);
	Serial.println("check line:  " + topic_number);
	Serial.flush();
#endif
	if ( topic_pre.compareTo("line") == 0 ) {
#ifdef DEBUG
		Serial.println("linenumber: " + topic_number);
		Serial.flush();
#endif
		komma=data.indexOf(",");
		if(komma > -1) {
			name=data.substring(0,komma);
			data=data.substring(komma+1);
			removequote(&name);
			komma=data.indexOf(",");
			if(komma > -1) {
				dev=data.substring(0,komma);
				data=data.substring(komma+1);
			} else {
				dev=data;
			}
			removequote(&dev);
			komma=data.indexOf(",");
			if(komma > -1) {
				reading=data.substring(0,komma);
				data=data.substring(komma+1);
			} else {
				reading=data;
			}
			removequote(&reading);
			komma=data.indexOf(",");
			if(komma > -1) {
				einheit=data.substring(0,komma);
				data=data.substring(komma+1);
			} else {
				reading=data;
			}
			removequote(&reading);
			komma=data.indexOf(",");
			if(komma > -1) {
				pos=data.substring(0,komma);
				data=data.substring(komma+1);
			} else {
				pos=data;
			}
			removequote(&reading);
			if ( ! sm.EntryExists(&name) ) {
				sm.add(&name,&dev,&reading,&einheit,topic_number.toInt(),pos.toInt());
				yield();
			}
  		} else {
  			name=data;
			while(name.indexOf('"') > -1) {
				komma=name.indexOf('"');
				if ( komma == 0 ) {
					name=name.substring(komma+1);
				} else {
					name=name.substring(0,komma);
				}
			}
			if ( ! sm.EntryExists(&name) ) {
				sm.add(&name,topic_number.toInt());
				yield();
			}
  		}
#ifdef DEBUG
  		Serial.print("Name:");
  		Serial.println(name.c_str());
#endif
		net.ConfigDone();
  	}
	mutex=false;
}
