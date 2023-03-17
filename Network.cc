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
	if( ! wifimanager.autoConnect("IOT_Display_AP") ) {
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
	server->begin();
	netvalues->begin();
	server->setNoDelay(true);
	netvalues->setNoDelay(true);
	client=network::FhemConnect();
	yield();
}

void network::testNet(display *disp) {
	int wifi_retry = 0;
	while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
		disp->NetOffline();
		WiFi.disconnect();
		wifimanager.setConfigPortalTimeout(300);
		wifimanager.autoConnect("AutoConnectAP");
		delay(100);
		wifi_retry++;
	}
	if(wifi_retry >= 5) {
		ESP.restart();
	}
}

//the minimalistic webserver
void network::handleWeb(display *disp) {
	//some variables
	//here for the actual client connection
	WiFiClient client;
	//and a buffer used to build the 
	//buffer to store the data send bei the client

	//Let us show if some client 
	//is available
	client = server->available();
	if (client) {
		//do this only if we have a client
		String currentline = "";
		while(client.connected()) {
			//and if the client connection was successfull
			if (client.available()) {
				//and the client is still there
				//first of all we must take the initial GET call
				//of the client anywhere (http protocoll definition)
				//so let us read the send string char by char
				char c = client.read();
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
						temp+="<html><head><meta http-equiv=\"refresh\" content=\"30\"></head><body>\n";
						//stylesheed for a better table display
						temp+="<style>\ntable {\nborder-collapse: seperate;\nborder-spacing: 0.2em;\n}\ntbody tr:nth-child(even) td {\nbackground-color: #e4ebf2;\ncolor: #000;\n}\n\n";
						temp+="</style>\n\n";
						//start with the HTML HEader Information
						//display a Head Line and add some empty lines
						temp+="<H1><center>IOT Display Device</center>\n";
						temp+="</H1><br><br><center><table border=0 cellpadding=\"8\" cellspacing=\"0\" width=\"80%\"><tbody>\n";
						//get the information for the last data update
						temp+="<tr><td>Last Update:</td><td style=\"text-align:right\">\n";
						temp+=String(disp->GetDate());
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
						client.print(temp);
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
		client.flush();
		client.stop();
	}
	//give ESP8266 time to to wlan stuff
	yield();
}

WiFiClient * network::FhemConnect() {
	void *ptr;
	ptr=(WiFiClient *) malloc(sizeof(WiFiClient));
	client = (WiFiClient *) new(ptr) WiFiClient();

	if (!client->connect(FHEM_HOST, FHEM_PORT)) {
		yield();
		return NULL;
	}
	return client;
}

void network::UpdateData(display *disp) {
	String Result;
	unsigned long lasttime;

	if(!client->connected()) {
		if(!client->connect(FHEM_HOST, FHEM_PORT)) {
			yield();
			return;
		}
	}
	while(client->available()) {
		Result = client->readStringUntil('\n');
	}
	client->print("{qx(date +%d\".\"%m\" \"%H\":\"%M)}\n");
	lasttime=millis();
	while(!client->available() && millis() - lasttime < 1000) {yield();}
	if(client->available()) {
		Result = client->readStringUntil('\n');
		Result = Result.substring(0,11);
		disp->SetDate(Result);
	}
	while(client->available()) {
		Result = client->readStringUntil('\n');
	}
	if (FhemGetData(&Result, String("E3DC"), String("pv_gesammt_leistung"), &lasttime) == 1)  {
		disp->UpdatePVLeistung((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("pv_ost_leistung"), &lasttime) == 1 ) {
		disp->UpdatePVLeistung_ost((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("pv_west_leistung"), &lasttime) == 1 ) {
		disp->UpdatePVLeistung_west((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("batterie_ladeleistung"), &lasttime) == 1 ) {
		disp->UpdatePVLeistung_batt((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("netzbezug"), &lasttime) == 1 ) {
		disp->UpdatePVLeistung_grid((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("eigenverbrauch"), &lasttime) == 1 ) {
		disp->UpdatePVVerbrauch((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("E3DC"), String("batterie_fuellstand"), &lasttime) == 1 ) {
		disp->UpdatePVBatterie(Result.toInt());
	}
	if (FhemGetData(&Result, String("E3DC"), String("Wallboxwatt"), &lasttime) == 1 ) {
		disp->UpdatePVWallboxWatt((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("kg_hzg_pvheat"), String("Leistung"), &lasttime) == 1 ) {
		disp->UpdatePVHeizstab((long) Result.toDouble());
	}
	if (FhemGetData(&Result, String("gt_carport_wetter"), String("temperature"), &lasttime) == 1 ) {
		disp->UpdateWTTemperatur(Result.toFloat());
	}
	if (FhemGetData(&Result, String("gt_carport_wetter"), String("humidity"), &lasttime) == 1 ) {
		disp->UpdateWTFeuchte(Result.toFloat());
	}
	if (FhemGetData(&Result, String("gt_carport_wetter"), String("pressure"), &lasttime) == 1 ) {
		disp->UpdateWTDruck(Result.toFloat());
	}
	if (FhemGetData(&Result, String("kg_heizung_pellets"), String("pellets"), &lasttime) == 1 ) {
		disp->UpdatePLGewicht(Result.toFloat());
	}
	if (FhemGetData(&Result, String("kg_hzg_brenner"), String("lager_kg"), &lasttime) == 1 ) {
		disp->UpdateHzLager(Result.toFloat()/1000);
	}
	if (FhemGetData(&Result, String("kg_hzg_brenner"), String("Stoerungs_nummer"), &lasttime) == 1 ) {
		disp->UpdateHzFehler(Result);
	}

	disp->EnableUpdate();
	while(!client->available() && millis() - lasttime < 1000) {yield();}
	client->print("quit\n");
	while(!client->available() && millis() - lasttime < 1000) {yield();}
}

int network::FhemGetData(String *result, const String device, const String reading, long unsigned int *lasttime) {
	String buffer;
	int pos;

	*result="";
	while(client->available()) {
		buffer = client->readStringUntil('\n');
	}
	buffer = "list " + device + " " +  reading + '\n';
	client->print(buffer);
	*lasttime=millis();
	while(!client->available() && millis() - *lasttime < 1000) {yield();}
	if(client->available()) {
		*result = client->readStringUntil('\n');
		pos=result->length();
		if(pos == 0) {
			return(0);
		}
		result->trim();
		pos=result->lastIndexOf(" ");
		*result=result->substring(pos+1);
	}
	while(client->available()) {
		buffer = client->readStringUntil('\n');
	}
	return(1);
}

void network::handleInc(display *disp) {
	WiFiClient client;
	String command = "";
	String Argument = "";
	String currentline = "";

	client = netvalues->available();
	client.print(F("$ "));
	while(client.connected()) {
		if (client.available()) {
			char c = client.read();
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
							client.print(F("I will set Leistung to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVLeistung((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungOst" ) ){
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Verbrauch Ost to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVLeistung_ost((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungWest" ) ){
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Verbrauch West to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVLeistung_west((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungBatt" ) ){
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Verbrauch Batt to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVLeistung_batt((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetLeistungGrid" ) ){
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Verbrauch Grid to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVLeistung_grid((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetVerbrauch" ) ){
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Verbrauch to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVVerbrauch((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetBatterie" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Batterie to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVBatterie(Argument.toInt());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetPvWallboxWatt" )) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set WallboxWatt to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVWallboxWatt((long) Argument.toDouble());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
						
					} else if ( ! strcmp(command.c_str(), "SetPVHeater" )) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set PVHeater to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePVHeizstab((long) Argument.toDouble());
							disp->EnableUpdate();
						}
					} else if ( ! strcmp(command.c_str(), "SetTemperatur" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Temperatur to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdateWTTemperatur(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetFeuchte" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Feuchte to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdateWTFeuchte(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetDruck" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Druck to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdateWTDruck(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetPellets" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Pellets to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdatePLGewicht(Argument.toFloat());
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetHzStatus" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Heater state to: "));
							client.print(Argument);
							client.print("\n");
							disp->UpdateHzFehler(Argument);
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else if ( ! strcmp(command.c_str(), "SetDate" ) ) {
						if ( Argument.length() > 0 ) {
							client.print(F("I will set Date to: "));
							client.print(Argument);
							client.print("\n");
							disp->SetDate(Argument);
							disp->EnableUpdate();
						} else {
							client.print(F("Command "));
							client.print(command.c_str());
							client.print(" Need at least one Argument");
						}
					} else {
						client.print(F("Command "));
						client.print(command.c_str());
						client.print("not known!\n");
					} 
				} else if (currentline.length() > 0 ) {
					if ( ! strcmp(currentline.c_str(), "help" ) ) {
						client.print(F("IOT Display command ref\n"));
						client.print(F("SetLeistung Value\n"));
						client.print(F("SetLeistungOst Value\n"));
						client.print(F("SetLeistungWest Value\n"));
						client.print(F("SetLeistungBatt Value\n"));
					        client.print(F("SetPvWallboxWatt\n"));
						client.print(F("SetLeistungGrid Value\n"));
					        client.print(F("SetVerbrauch Value\n"));
						client.print(F("SetBatterie Value\n"));
						client.print(F("SetPVHeater Value\n"));
						client.print(F("SetTemperatur Value\n"));
						client.print(F("SetFeuchte Value\n"));
						client.print(F("SetDruck Value\n"));
						client.print(F("SetPellets Value\n"));
					        client.print(F("SetHzStatus\n"));
						client.print(F("SetDate Value\n"));
						client.print(F("exit\n"));
					} else if ( ! strcmp(currentline.c_str(), "exit" ) ) {
						client.print("\r\n");
						break;
					} else if ( ! strcmp(currentline.c_str(), "bye" ) ) {
						client.print("\r\n");
						break;
					} else {
						client.print(F("Command "));
						client.print(currentline.c_str());
						client.print(" not known!\n");
					}
				}
				//client.flush();
				currentline="";
				command="";
				Argument="";
				client.print(F("$ "));
				yield();
			} else if ( c != '\r' ) {
				//collect the line send by client char for char.
				currentline += c;
				yield();
			} else if ( c == EOF ) {
				client.print(EOF);
				client.print("\r\n");
				break;
			}
		}
	}
	//reset our header variable
	header = "";
	// and stop the client connection
	client.stop();
}

void network::handleOTA() {
	ArduinoOTA.handle();
}
