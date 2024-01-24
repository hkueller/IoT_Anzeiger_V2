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
bool mutex=false;
bool firstrun=true;

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
	Serial.println("Starting Network");
	Serial.flush();
#endif
	disp.Message("Starting up Network");
	net.begin();
	disp.Message("Network Up");
/*#ifdef DEBUG
	Serial.println("Starting Data Reading");
	Serial.flush();
#endif
	disp.Message("Reading Home Data");
	net.UpdateData(&disp,&sm);
	disp.updatelast();*/
	if(! sm.SetFirst()) {
		disp.Message("Waiting for config data");
	}
#ifdef DEBUG
	Serial.println("Setup completed");
	Serial.flush();
#endif
}

void loop() {
	if ( mutex ) {
		yield();
		return;
	}
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
	Serial.println("Handle Net");
	Serial.flush();
#endif
	net.handleNet(&sm, &disp);
	if(sm.SetFirst() == true) {
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
	if(firstrun) {
#ifdef DEBUG
		Serial.println("Requesting transfer of config data");
		Serial.flush();
#endif
		net.RequestConfig();
		firstrun=false;
	}
}
