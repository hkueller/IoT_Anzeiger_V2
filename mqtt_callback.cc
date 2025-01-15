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

void mqtt_cb(char *topic, byte* payload, unsigned int length) {
	while(mutex) {
		yield();
		delay(100);
 	}
	mutex=true;
	String topic_pre;
	String data;
	
#ifdef DEBUG
	Serial.println("Message Arrived");
#endif
	for ( int i = 0; i < (int) length; i++) {
		data+=(char) payload[i];
	}
#ifdef DEBUG
	Serial.println("Topic: [" + String(topic) + "]:" + data);
#endif
	if ( String(topic).indexOf("/") > 0 ) {
		topic_pre=String(topic).substring(String(topic).indexOf("/")+1);
	}
#ifdef DEBUG
	Serial.println("sub topic: " + topic_pre);
#endif
	sm.SetMqtt(data,topic_pre);
	mutex=false;
}

