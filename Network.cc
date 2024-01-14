#include <Network.h>

network::network() {
	void *ptr;
	ptr=(WiFiServer *) malloc(sizeof(WiFiServer));
	server = (WiFiServer *) new(ptr) WiFiServer(80);
	ptr=(WiFiServer *) malloc(sizeof(WiFiServer));
	netvalues = (WiFiServer *) new(ptr) WiFiServer(5023);
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
	server->begin();
	netvalues->begin();
	server->setNoDelay(true);
	netvalues->setNoDelay(true);
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
	/*if ( millis() > 30 * 1000 * 60 ) {
#ifdef DEBUG
		Serial.println("time over, restarting");
        	Serial.flush();
#endif
		ESP.restart();
	}*/
	while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
#ifdef DEBUG
		Serial.println("Network Offline, Reconnect");
        	Serial.flush();
#endif
		disp->NetOffline();
		WiFi.disconnect();
		delay(100);
		wifimanager.setConfigPortalTimeout(300);
		wifimanager.autoConnect("AutoConnectAP");
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
		netvalues->begin();
		server->setNoDelay(true);
		netvalues->setNoDelay(true);
		network::FhemConnect();
		yield();
	}
	return(rc);
}

//the minimalistic webserver
void network::handleWeb(display *disp) {
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
						temp+="</td></tr><tr><td>Haus Verbrauch:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVVerbrauch());
						temp+=" W";
						temp+="</td></tr><tr><td>Wallbox Leistung:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVWallboxWatt());
						temp+=" W";
						temp+="</td></tr><tr><td>Externer Versorger:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVLeistung_grid());
						temp+=" W";
						temp+="</td></tr><tr><td>Batterie Strom:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVLeistung_batt());
						temp+=" W";
						temp+="</td></tr><tr><td>Ertrag Gesammt:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVLeistung());
						temp+=" W";
						temp+="</td></tr><tr><td>Ertrag Osten:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVLeistung_ost());
						temp+=" W";
						temp+="</td></tr><tr><td>Ertrag Westen:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVLeistung_west());
						temp+=" W";
						temp+="</td></tr><tr><td>PV Batterie:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVBatterie());
						temp+=" %";
						temp+="</td></tr><tr><td>PV-Heizstab:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetPVHeizstab());
						temp+=" W";
						temp+="</td></tr><tr><td>Temperatur:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetWTTemperatur());
						temp+=" C";
						temp+="</td></tr><tr><td>Feuchte:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetWTFeuchte());
						temp+=" %";
						temp+="</td></tr><tr><td>Luftdruck:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetWTDruck(),1);
						temp+=" hP";
						temp+="</td></tr><tr><td>Pellets im Lager:</td><td style=\"text-align:right\">\n";
						//we want the pellts ammount in tonns with 3 values after .
						temp+=String(disp->GetPLGewicht(),3);
						temp+=" T";
						temp+="</td></tr><tr><td>Pellets im Lager(Hzg):</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetHzLager(),3);
						temp+=" T";
						temp+="</td></tr><tr><td>Heizung Status:</td><td style=\"text-align:right\">\n";
						temp+=disp->GetHzFehler();
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

void network::UpdateData(display *disp) {
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
	/*for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("global->altitude");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("global"), String("altitude"), &lasttime) == 1) {
			break;
		}
	}*/
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->pv_gesammt_leistung");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("pv_gesammt_leistung"), &lasttime) == 1) {
			disp->UpdatePVLeistung((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVLeistung((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->pv_ost_leistung");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("pv_ost_leistung"), &lasttime) == 1 ) {
			disp->UpdatePVLeistung_ost((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVLeistung_ost((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->pv_west_leistung");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("pv_west_leistung"), &lasttime) == 1 ) {
			disp->UpdatePVLeistung_west((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVLeistung_west((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->batterie_ladeleistung");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("batterie_ladeleistung"), &lasttime) == 1 ) {
			disp->UpdatePVLeistung_batt((long) Result.toDouble());
			break;
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->netzbezug");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("netzbezug"), &lasttime) == 1 ) {
			disp->UpdatePVLeistung_grid((long) Result.toDouble());
			break;
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->eigenverbrauch");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("eigenverbrauch"), &lasttime) == 1 ) {
			disp->UpdatePVVerbrauch((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVVerbrauch((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->batterie_fuellstand");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("batterie_fuellstand"), &lasttime) == 1 ) {
			disp->UpdatePVBatterie(Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVBatterie(Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("E3DC->Wallboxwatt");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("E3DC"), String("Wallboxwatt"), &lasttime) == 1 ) {
			disp->UpdatePVWallboxWatt((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVWallboxWatt((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("kg_hzg_pvheat->Leistung");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("kg_hzg_pvheat"), String("Leistung"), &lasttime) == 1 ) {
			disp->UpdatePVHeizstab((long) Result.toDouble());
			break;
		} else {
			Result="-1";
			disp->UpdatePVHeizstab((long) Result.toDouble());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("gt_carport_wetter->av_temp");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("gt_carport_wetter"), String("av_temp"), &lasttime) == 1 ) {
			disp->UpdateWTTemperatur(Result.toFloat());
			break;
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("gt_carport_wetter->humidity");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("gt_carport_wetter"), String("humidity"), &lasttime) == 1 ) {
			disp->UpdateWTFeuchte(Result.toFloat());
			break;
		} else {
			Result="-1";
			disp->UpdateWTFeuchte(Result.toFloat());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("gt_carport_wetter->pressure");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("gt_carport_wetter"), String("pressure"), &lasttime) == 1 ) {
			disp->UpdateWTDruck(Result.toFloat());
			break;
		} else {
			Result="-1";
			disp->UpdateWTDruck(Result.toFloat());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("kg_heizung_pellets->pellets");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("kg_heizung_pellets"), String("pellets"), &lasttime) == 1 ) {
			disp->UpdatePLGewicht(Result.toFloat());
			break;
		} else {
			Result="-1";
			disp->UpdatePLGewicht(Result.toFloat());
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("kg_hzg_brenner->lager_kg");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("kg_hzg_brenner"), String("lager_kg"), &lasttime) == 1 ) {
			disp->UpdateHzLager(Result.toFloat()/1000);
			break;
		} else {
			Result="-1";
			disp->UpdateHzLager(Result.toFloat()/1000);
		}
	}
	for(int i=0;i<FHEM_RETRY;i++) {
#ifdef DEBUG
		Serial.println("kg_hzg_brenner->Stoerungs_nummer");
        	Serial.flush();
#endif
		if (FhemGetData(&Result, String("kg_hzg_brenner"), String("Stoerungs_nummer"), &lasttime) == 1 ) {
			disp->UpdateHzFehler(Result);
			break;
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
#ifdef DEBUG
		Serial.println("Update Data done, Enable Display update");
        	Serial.flush();
#endif
	disp->EnableUpdate();
	while(!fhemclient->available()) {yield();}
	fhemclient->print("quit\n");
	//network::FhemDisconnect();
}

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

void network::handleInc(display *disp) {
	WiFiClient telnetclient;
	String command = "";
	String Argument = "";
	String currentline = "";

	telnetclient = netvalues->accept();
	telnetclient.print(F("$ "));
	while(telnetclient.connected()) {
		if (telnetclient.available()) {
			char c = telnetclient.read();
			header +=c;
			if ( c== ' ' ) {
				if (currentline.length() > 0) {
					command = currentline;
					currentline="";
				}
			} else if( c == '\n' ) {
				if (command.length() > 0 ) {
					Argument = currentline;
					if ( ! strcmp(command.c_str(),"SetLeistung" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Leistung to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVLeistung((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungOst" ) ){
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Verbrauch Ost to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVLeistung_ost((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungWest" ) ){
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Verbrauch West to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVLeistung_west((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungBatt" ) ){
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Verbrauch Batt to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVLeistung_batt((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungGrid" ) ){
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Verbrauch Grid to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVLeistung_grid((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetVerbrauch" ) ){
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Verbrauch to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVVerbrauch((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetBatterie" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Batterie to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVBatterie(Argument.toInt());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetPvWallboxWatt" )) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set WallboxWatt to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVWallboxWatt((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
						
					} else if ( ! strcmp(command.c_str(), "SetPVHeater" )) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set PVHeater to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePVHeizstab((long) Argument.toDouble());
							disp->EnableUpdate();
						}
					} else if ( ! strcmp(command.c_str(), "SetTemperatur" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Temperatur to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdateWTTemperatur(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetFeuchte" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Feuchte to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdateWTFeuchte(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetDruck" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Druck to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdateWTDruck(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetPellets" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Pellets to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdatePLGewicht(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetHzStatus" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Heater state to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->UpdateHzFehler(Argument);
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetDate" ) ) {
						if ( Argument.length() > 0 ) {
							telnetclient.print(F("I will set Date to: "));
							telnetclient.print(Argument);
							telnetclient.print("\n");
							disp->SetDate(Argument);
							disp->EnableUpdate();
						} else {
							telnetclient.print(F("Command "));
							telnetclient.print(command.c_str());
							telnetclient.print(" Need at least one Argument");
						}
					} else {
						telnetclient.print(F("Command "));
						telnetclient.print(command.c_str());
						telnetclient.print("not known!\n");
					} 
				} else if (currentline.length() > 0 ) {
					if ( ! strcmp(currentline.c_str(), "help" ) ) {
						telnetclient.print(F("IOT Display command ref\n"));
						telnetclient.print(F("SetLeistung Value\n"));
						telnetclient.print(F("SetLeistungOst Value\n"));
						telnetclient.print(F("SetLeistungWest Value\n"));
						telnetclient.print(F("SetLeistungBatt Value\n"));
					        telnetclient.print(F("SetPvWallboxWatt\n"));
						telnetclient.print(F("SetLeistungGrid Value\n"));
					        telnetclient.print(F("SetVerbrauch Value\n"));
						telnetclient.print(F("SetBatterie Value\n"));
						telnetclient.print(F("SetPVHeater Value\n"));
						telnetclient.print(F("SetTemperatur Value\n"));
						telnetclient.print(F("SetFeuchte Value\n"));
						telnetclient.print(F("SetDruck Value\n"));
						telnetclient.print(F("SetPellets Value\n"));
					        telnetclient.print(F("SetHzStatus\n"));
						telnetclient.print(F("SetDate Value\n"));
						telnetclient.print(F("exit\n"));
					} else if ( ! strcmp(currentline.c_str(), "exit" ) ) {
						telnetclient.print("\r\n");
						break;
					} else if ( ! strcmp(currentline.c_str(), "bye" ) ) {
						telnetclient.print("\r\n");
						break;
					} else {
						telnetclient.print(F("Command "));
						telnetclient.print(currentline.c_str());
						telnetclient.print(" not known!\n");
					}
				}
				//client.flush();
				currentline="";
				command="";
				Argument="";
				telnetclient.print(F("$ "));
				yield();
			} else if ( c != '\r' ) {
				//collect the line send by client char for char.
				currentline += c;
				yield();
			} else if ( c == EOF ) {
				telnetclient.print(EOF);
				telnetclient.print("\r\n");
				break;
			}
		}
	}
	//reset our header variable
	header = "";
	// and stop the client connection
	telnetclient.stop();
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
