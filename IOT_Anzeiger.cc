/**
 *  @filename   :   IOT_Anzeiger.cc
 *  @brief      :   Display some Values gethared from FHEM (WLAN)
 *  @author     :   Harald Küller
 *
 *  Copyright (C) Waveshare     November 6 2021
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SPI.h>
#include <Display.h>
#include <Network.h>
#include <Data.h>

display disp;
network net;
smarthome sm;


void DataSetup() {
	//Add a head line ("Solarinfo" on line 0 (only on line 0 Date and time of last dataload will be added)
	sm.add("Solarinfo   ",0);
	//Add Entry for Line 1 (row 3) to Display, which will be labeld "PV Ertrag", and
	//taken from fhem device "E3DC" and Reading "pv_gesammt_leistung".
	//the value will be displayed as "W" (Watts)
	sm.add("PV Ertrag","E3DC","pv_gesammt_leistung","W",1,3);
	yield();
	//Add Entry, which will only be Displayed on Web site. Labeled "Ertrag Osten", data
	//from fhem Device "E3DC" and reading "pv_ost_leistung"
	sm.add("Ertrag Osten","E3DC","pv_ost_leistung","W");
	yield();
	sm.add("Ertrag Westen","E3DC","pv_west_leistung","W");
	yield();
	sm.add("Verbrauch","E3DC","eigenverbrauch","W",2,3);
	yield();
	sm.add("Wallbox","E3DC","Wallboxwatt","W",4,3);
	yield();
	//Add a headline on line 5
	sm.add(" Wetter Informationen",5);
	yield();
	sm.add("Externer Versorger","E3DC","netzbezug","W");
	yield();
	sm.add("Batterie Strom","E3DC","batterie_ladeleistung","W");
	yield();
	sm.add("Batterie","E3DC","batterie_fuellstand","%",3,3);
	yield();
	sm.add("PV Heizstab","kg_hzg_pvheat","Leistung","W");
	yield();
	sm.add("Temperatur","gt_carport_wetter","av_temp","C",6,3);
	yield();
	sm.add("Feuchte","gt_carport_wetter","humidity","%",7,3);
	yield();
	sm.add("Luftdruck","gt_carport_wetter","pressure","hp",8,3);
	yield();
	sm.add("Pellets Heizung",9);
	yield();
	sm.add("Gew. Bestand","kg_hzg_brenner","lager_kg","kg",10,3);
	yield();
	sm.add("Hzg Status","kg_hzg_brenner","Stoerungs_nummer","",11,3);
	yield();
}

void setup() {
	// put your setup code here, to run once:
	// add Display Data
	//bool add(String show_name, String fhem_dev, String fhem_reading, int disp_line); 
#ifdef DEBUG
	//delay(10000);
	Serial.begin(115200);
	Serial.println("Setup starting");
#endif
	disp.begin();
#ifdef DEBUG
	Serial.begin(115200);
#endif
#ifdef DEBUG
	Serial.println("Setting up smarthome class");
	Serial.flush();
	delay(1000);
#endif
	disp.Message("Display Data Setup");
	DataSetup();
#ifdef DEBUG
	Serial.println("smarthome class setup done");
	Serial.flush();
#endif
#ifdef DEBUG
	Serial.println("Starting Network");
	Serial.flush();
#endif
	disp.Message("Starting up Network");
	net.begin();
	disp.Message("Network Up");
	yield();
	delay(1000);
	yield();
#ifdef DEBUG
	Serial.println("Starting Data Reading");
	Serial.flush();
#endif
	disp.Message("Reading Home Data");
	net.UpdateData(&disp,&sm);
	disp.updatelast();
#ifdef DEBUG
	Serial.println("Setup completed");
	Serial.flush();
#endif
}

void loop() {
	// put your main code here, to run repeatedly:
	if ( net.testNet(&disp) == 1 ) {
#ifdef DEBUG
		Serial.println("Network Restarted\nLoading NetScreen");
		Serial.flush();
#endif
		disp.Message("Network Up");
#ifdef DEBUG
		Serial.println("Loading FHEM Data");
		Serial.flush();
#endif
		net.UpdateData(&disp,&sm);
#ifdef DEBUG
		Serial.println("Updating Display");
		Serial.flush();
#endif
		disp.updatelast();
	}
#ifdef DEBUG
	Serial.println("Running OTA");
	Serial.flush();
#endif
	net.handleOTA();
	yield();
#ifdef DEBUG
	Serial.println("Running WEB");
	Serial.flush();
#endif
	net.handleWeb(&sm);
	yield();
	if ( millis() - disp.getLastTime() > UPDATE_MINUTE * 1000 * 60) {
#ifdef DEBUG
		Serial.println("Updating Data");
		Serial.flush();
#endif
		net.UpdateData(&disp,&sm);
		yield();
	}
	if ( disp.GetUpdate() == true) {
#ifdef DEBUG
		Serial.println("Update Display");
		Serial.flush();
#endif
		disp.LoadFrame(&sm);
#ifdef DEBUG
		Serial.println("Update Updatetime");
		Serial.flush();
#endif
		disp.updatelast();
		yield();
	}
}
