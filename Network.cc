#include <Network.h>

network::network() {
	void *ptr;
	ptr=(WiFiServer *) malloc(sizeof(WiFiServer));
	server = (WiFiServer *) new(ptr) WiFiServer(80);
}

void network::begin() {
	//wifimanager.resetSettings();
	wifimanager.setConfigPortalTimeout(300);
	if( ! wifimanager.autoConnect("IOTAnzeiger") ) {
		ESP.restart();
	}
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else { // U_FS
			type = "filesystem";
		}
	});
  	ArduinoOTA.begin();
	configTime(MY_TZ, MY_NTP_SERVER);
	while(!getLocalTime(&zeit)) {
#ifdef DEBUG
		Serial.println("Time still not available");
		Serial.flush();
#endif
		delay(100);
	}
	server->begin();
	server->setNoDelay(true);
	network::FhemConnect();
	yield();
}

int network::testNet(display *disp) {
	int rc=0;
	int wifi_retry = 0;
#ifdef DEBUG
	Serial.println("testNet() running");
        Serial.flush();
#endif
#ifdef DEBUG
	Serial.println("checking uptime");
	Serial.flush();
#endif
	while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
#ifdef DEBUG
		Serial.println("Network Offline, Reconnect");
        	Serial.flush();
#endif
		disp->Message("Network Offline!");
		WiFi.disconnect();
		delay(100);
		wifimanager.setConfigPortalTimeout(300);
		wifimanager.autoConnect("IOTAnzeiger");
		delay(100);
		wifi_retry++;
		rc=1;
	}
	if(WiFi.status() != WL_CONNECTED) {
#ifdef DEBUG
		Serial.println("Network Offline, Reconnect failed, forcing system reset");
        	Serial.flush();
#endif
		ESP.restart();
	}
	if((WiFi.status() == WL_CONNECTED) && ( rc == 1) ) {
#ifdef DEBUG
		Serial.println("Checking for OTA");
        	Serial.flush();
#endif
  		ArduinoOTA.begin();
#ifdef DEBUG
		Serial.println("Pull System time with ntp");
        	Serial.flush();
#endif
		configTime(MY_TZ, MY_NTP_SERVER);
#ifdef DEBUG
		Serial.println("starting the servers");
        	Serial.flush();
#endif
		server->begin();
		server->setNoDelay(true);
		network::FhemConnect();
		yield();
	}
	return(rc);
}

void network::handleWeb(smarthome *data) {
	//some variables
	//here for the actual client connection
	WiFiClient webclient;
	//and a buffer used to build the 
	//buffer to store the data send bei the client

	//Let us show if some client 
	//is available
	webclient = server->accept();
	if (webclient) {
		//do this only if we have a client
		String currentline = "";
		while(webclient.connected()) {
			//and if the client connection was successfull
			if (webclient.available()) {
				//and the client is still there
				//first of all we must take the initial GET call
				//of the client anywhere (http protocoll definition)
				//so let us read the send string char by char
				char c = webclient.read();
				header +=c;
				if ( c == '\n' ) {
					//OK we recived the newline command
					if (currentline.length() == 0 ) {
						//and nothing else - it was just a 
						//empty line -> we are ready to send our data

						//define a string to convert our
						//float values to stings
						String temp;
						//Let us load the web page into the temp string
						//start with HTML Header Information
						temp="HTTP/1.1 200 OK\n";
						temp+="Content-type:text/html\n";
						temp+="Connection: close\n\n";
						temp+="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
						temp+="<html><head><meta http-equiv=\"refresh\" content=\"60\"></head><body>\n";
						//stylesheed for a better table display
						temp+="<style>\ntable {\nborder-collapse: seperate;\nborder-spacing: 0.2em;\n}\ntbody tr:nth-child(even) td {\nbackground-color: #e4ebf2;\ncolor: #000;\n}\n\n";
						temp+="</style>\n\n";
						//start with the HTML HEader Information
						//display a Head Line and add some empty lines
						temp+="<H1><center>IOT Display Device</center>\n";
						temp+="</H1><br><br><center><table border=0 cellpadding=\"8\" cellspacing=\"0\" width=\"80%\"><tbody>\n";
						//get the information for the last data update
						temp+="<tr><td>Last Update:</td><td style=\"text-align:right\">\n";
						temp+=twodigit(zeit.tm_mday);
						temp+=".";
						temp+=twodigit(zeit.tm_mon+1);
						temp+=".";
						temp+=zeit.tm_year+1900;
						temp+=" ";
						temp+=twodigit(zeit.tm_hour);
						temp+=":";
						temp+=twodigit(zeit.tm_min);
						temp+=".";
						temp+=twodigit(zeit.tm_sec);
						data->SetFirst();
						if ( ! data->IsHeadline() ) {
							temp+="</td></tr><tr><td>";
							temp+=data->GetName();
							temp+=":</td><td style=\"text-align:right\">\n";
							temp+=data->GetData();
						}
						while(data->SetNext()) {
							if ( ! data->IsHeadline() ) {
								temp+="</td></tr><tr><td>";
								temp+=data->GetName();
								temp+=":</td><td style=\"text-align:right\">\n";
								temp+=data->GetData();
								temp+=" ";
								temp+=data->GetEinheit();
							}
						}
						temp+="</td></tr></tbody></table></center><br></body></html>\n\n";
						//now send the string buffer
						webclient.print(temp);
						//and jump out -> we are ready and will close the connection
						break;
					} else {
						//this was not a empty line - so we have still to get data
						//empty the variable, and startover for the next line
						currentline = "";
					}
				} else if ( c != '\r' ) {
					//collect the line send by client char for char.
					currentline += c;
				}
			}
		}
		//reset our header variable
		header = "";
		// and stop the client connection
		webclient.flush();
		webclient.stop();
	}
	//give ESP8266 time to to wlan stuff
	yield();
}


int network::FhemConnect() {
	if ( fhemclient == 0 ) {
		void *ptr;
		ptr=(WiFiClient *) malloc(sizeof(WiFiClient));
		fhemclient = (WiFiClient *) new(ptr) WiFiClient();
	}

	if (!fhemclient->connect(FHEM_HOST, FHEM_PORT)) {
		yield();
		return 0;
	}
	return 1;
}

void network::FhemDisconnect() {
	fhemclient->flush();
	fhemclient->stop();
	delete(fhemclient);
	fhemclient=0;
}

void network::UpdateData(display *disp, smarthome *data) {
	String Result;
	unsigned long lasttime;

	if ( ! fhemclient->connected()) {
		network::FhemDisconnect();
		delay(100);
		if ( network::FhemConnect() != 0 ) {
			yield();
			return;
		}
	}
#ifdef DEBUG
	Serial.println("Connection OK\n");
        Serial.flush();
        Serial.flush();
#endif
	fhemclient->print("\n");
	while(fhemclient->available()) {
		Result = fhemclient->readStringUntil('\n');
	}
	fhemclient->flush();
#ifdef DEBUG
	Serial.println("Start reading data");
        Serial.flush();
#endif
	data->SetFirst();
	if ( ! data->IsHeadline() ) {
		for ( int i=0; i<FHEM_RETRY; i++) {
#ifdef DEBUG
			Serial.println(data->GetDev() + "->" + data->GetReading() );
			Serial.flush();
#endif
			if (FhemGetData(&Result, data->GetDev(), data->GetReading(), &lasttime) == 1 ) {
				data->SetData(Result);
				break;
			} else {
				Result="-1";
				data->SetData(Result);
			}
		}
	}
	while(data->SetNext()) {
		for ( int i=0; i<FHEM_RETRY; i++) {
#ifdef DEBUG
			Serial.println(data->GetDev() + "->" + data->GetReading() );
			Serial.flush();
#endif
			if (FhemGetData(&Result, data->GetDev(), data->GetReading(), &lasttime) == 1 ) {
				data->SetData(Result);
				break;
			} else {
				Result="-1";
				data->SetData(Result);
			}
		}
	}
	StoreTimeLastRead();
	Result=twodigit(zeit.tm_mday);
	Result+=".";
	Result+=twodigit(zeit.tm_mon+1);
	Result+=" ";
	Result+=twodigit(zeit.tm_hour);
	Result+=":";
	Result+=twodigit(zeit.tm_min);
	disp->SetDate(Result);
	disp->EnableUpdate();
	while(!fhemclient->available()) {yield();}
	fhemclient->print("quit\n");
	//network::FhemDisconnect();
};

int network::FhemGetData(String *result, const String device, const String reading, long unsigned int *lasttime) {
	String buffer;
	*result="";
#ifdef DEBUG
	Serial.println("Flushing Network input");
       	Serial.flush();
#endif
	while(fhemclient->available()) {
		buffer = fhemclient->read();
	}
	buffer = "{ ReadingsVal(\"" + device + "\",\"" + reading + "\",0) }\n";
#ifdef DEBUG
	Serial.println("Send fhem command");
       	Serial.flush();
#endif
	fhemclient->print(buffer);
#ifdef DEBUG
	Serial.println("Waiting for answer");
       	Serial.flush();
#endif
	*lasttime=millis();
	while(fhemclient->available() < 1 ) {yield();}
#ifdef DEBUG
	Serial.println("Reading answer");
       	Serial.flush();
#endif
	*result=fhemclient->readStringUntil('\n');
	result->trim();
#ifdef DEBUG
	Serial.println("done");
       	Serial.flush();
#endif
	return(1);
}

void network::handleOTA() {
	ArduinoOTA.handle();
}

void network::StoreTimeLastRead() {
        time_t now;

        now = time(nullptr);
        localtime_r(&now, &zeit);
}

int network::ReturnYear() {
	return(zeit.tm_year+1990);
}

int network::ReturnMonth() {
	return(zeit.tm_mon);
}

int network::ReturnDay() {
	return(zeit.tm_mday);
}

int network::ReturnHour() {
	return(zeit.tm_hour);
}

int network::ReturnMinutes() {
	return(zeit.tm_min);
}

String network::ReturnSeconds() {
	String seconds;

	if ( zeit.tm_sec < 0 ) {
		seconds=0;
		seconds+=zeit.tm_sec;
	} else {
		seconds=zeit.tm_sec;
	}
	return(seconds);
}

String network::twodigit(int number) {
	String buffer;

	buffer="";
	if ( number < 10 ) {
		buffer+="0";
	}
	buffer+=number;
	return(buffer);
}
