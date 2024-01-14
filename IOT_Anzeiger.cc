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

display disp;
network net;


void setup() {
	// put your setup code here, to run once:
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Setup starting");
	Serial.flush();
#endif
	disp.begin();
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Starting Net");
	Serial.flush();
#endif
	net.begin();
#ifdef DEBUG
	Serial.println("Net started");
	Serial.flush();
#endif
	disp.NetDone();  
#ifdef DEBUG
	Serial.println("Starting Data Reading");
	Serial.flush();
#endif
	net.UpdateData(&disp);
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
		disp.NetDone();
#ifdef DEBUG
		Serial.println("Loading FHEM Data");
		Serial.flush();
#endif
		net.UpdateData(&disp);
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
#ifdef DEBUG
	Serial.println("Running WEB");
	Serial.flush();
#endif
	net.handleWeb(&disp);
	if ( millis() - disp.getLastTime() > UPDATE_MINUTE * 1000 * 60) {
#ifdef DEBUG
		Serial.println("Updating Data");
		Serial.flush();
#endif
		net.UpdateData(&disp);
	}
#ifdef DEBUG
		Serial.println("Running Telnet");
		Serial.flush();
#endif
	net.handleInc(&disp);
	if ( disp.GetUpdate() == true) {
#ifdef DEBUG
		Serial.println("Update Display");
		Serial.flush();
#endif
		disp.LoadFrame();
#ifdef DEBUG
		Serial.println("Update Updatetime");
		Serial.flush();
#endif
		disp.updatelast();
	}
}
