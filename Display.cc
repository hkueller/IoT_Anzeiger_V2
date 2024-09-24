#include <config.h>
#include <Display.h>

display::display() {
	//Used to get Memory from System
	void *pd;

	//Request memory for the framebuffer
	framebuffer=(unsigned char *) malloc(((DISPLAY_HEIGHT * DISPLAY_WIDTH)/8) * sizeof(unsigned char));
	//Request Space for paint class
	pd=malloc(sizeof(Paint));
	//and generate one instance of the class
	paint=new(pd) Paint(framebuffer,DISPLAY_HEIGHT,DISPLAY_WIDTH);
	lasttime = millis() - ( UPDATE_MINUTE * 1000 * 60 );
}

void display::begin(smarthome *config) {
	//Initialize the E-Paper display
	epd.Init();
	//display Startup Message
	Message("Starting Up", config);
}

void display::Message(String message, smarthome *config) { 
	paint->Clear(UNCOLORED);
	paint->SetRotate(config->GetOrientation());
	paint->DrawStringAt(\
		((config->GetWidth() -2)/2) - ((String(APSID).length()*config->GetCharWidth(MSG))/2),\
		((config->GetHeight()-2)/2)-config->GetCharHeight(MSG),\
		APSID,\
		config->GetFont(MSG),\
		COLORED\
	);
	paint->DrawStringAt(\
		((config->GetWidth()-2)/2) - ((message.length()*config->GetCharWidth(MSG))/2),\
		(config->GetHeight()-2)/2,\
		message.c_str(),\
		config->GetFont(MSG),\
		COLORED\
	);
	epd.DisplayFrame(framebuffer);
}

void display::Message(String line1, String line2, smarthome *config) {
	paint->Clear(UNCOLORED);
	paint->SetRotate(config->GetOrientation());
	paint->DrawStringAt(\
		((config->GetWidth() -2)/2) - ((String(APSID).length()*config->GetCharWidth(MSG))/2),\
		((config->GetHeight()-2)/2)-config->GetCharHeight(MSG),\
		APSID,\
		config->GetFont(MSG),\
		COLORED\
	);
	paint->DrawStringAt(\
		((config->GetWidth()-2)/2) - ((line1.length()*config->GetCharWidth(MSG))/2),\
		(config->GetHeight()-2)/2,\
		line1.c_str(),\
		config->GetFont(MSG),\
		COLORED\
	);
	paint->DrawStringAt(\
		((config->GetWidth()-2)/2) - ((line2.length()*config->GetCharWidth(MSG))/2),\
		((config->GetHeight()-2)/2)+config->GetCharHeight(MSG),\
		line2.c_str(),\
		config->GetFont(MSG),\
		COLORED\
	);
	epd.DisplayFrame(framebuffer);
}

void display::LoadFrame(smarthome *data) {
	paint->Clear(UNCOLORED);
	paint->SetRotate(data->GetOrientation());
	//draw the frames on the display
	paint->DrawVerticalLine(1,1,data->GetHeight()-1, COLORED);
	paint->DrawHorizontalLine(1,data->GetHeight()-1,data->GetWidth()-1,COLORED);
	paint->DrawVerticalLine(data->GetWidth()-1,1,data->GetHeight()-1,COLORED);
	paint->DrawHorizontalLine(1,1,data->GetWidth()-1,COLORED);
	for ( int i=0; i < data->GetDashNum(); i++) {
		paint->DrawHorizontalLine(1,FIRSTLINE+((data->GetDashPos(i)*data->GetCharHeight(DISP))),data->GetWidth()-1,COLORED);
	}
	//draw the text context and values on the display
	data->SetFirst();
	if ( data->IsHeadline() ) {
#ifdef DEBUG
		Serial.println("Adding Headline");
		Serial.flush();
#endif
		if ( data->GetLine() == 0 ) {
#ifdef DEBUG
			Serial.println("Headline is first line, adding date" + date);
			Serial.flush();
#endif
			PrintHeadLine(data->GetName(), date, data);
		} else {
#ifdef DEBUG
			Serial.println("Headline" + data->GetName() + "add");
			Serial.flush();
#endif
			PrintHeadLine(data->GetLine(),data->GetName(),data);
		}
	} else {
#ifdef DEBUG
		Serial.println("Adding DataEntry for" + data->GetLine());
		Serial.flush();
#endif
		PrintValueLine(3,data->GetLine(),0,data->GetName(),data->GetEinheit(),data->GetData(),data);
	}
	while(data->SetNext()) {
		if ( data->IsHeadline() ) {
#ifdef DEBUG
			Serial.println("Adding Headling");
			Serial.flush();
#endif
			if ( data->GetLine() == 0 ) {
#ifdef DEBUG
				Serial.println("Headline is first line, adding date" + date);
				Serial.flush();
#endif
				PrintHeadLine(data->GetName(),date,data);
			} else {
#ifdef DEBUG
				Serial.println("Headline" + data->GetName() + "add");
				Serial.println("Headlinepos: " + String(data->GetLine()));
				Serial.flush();
#endif
				PrintHeadLine(data->GetLine(),data->GetName(),data);
			}
		} else {
			if( data->GetLine() > 0 ) {
#ifdef DEBUG
				Serial.println("Adding DataEntry for" + data->GetLine());
				Serial.flush();
#endif
				PrintValueLine(3,data->GetLine(),0,data->GetName(),data->GetEinheit(),data->GetData(),data);
			}
		}
	}
	//Send the Buffer to the Display, and do a refresh with the contents
	//of this buffer (attention this will switch internal (display) the buffers
	epd.DisplayFrame(framebuffer);
	Update=false;
}

void display::PrintHeadLine(String head, String date, smarthome *config) {
	paint->DrawFilledRectangle(0,FIRSTLINE,config->GetWidth(),config->GetCharHeight(DISP),COLORED);
	paint->DrawStringAt(3,FIRSTLINE,head.c_str(),config->GetFont(DISP),UNCOLORED);
	paint->DrawStringAt((config->GetWidth()-2)-(date.length() * config->GetCharWidth(DISP)),FIRSTLINE,date.c_str(),config->GetFont(DISP),UNCOLORED);
}

void display::PrintHeadLine(int linenum, String head, smarthome *config) {
	paint->DrawFilledRectangle(0,FIRSTLINE+(linenum*config->GetCharHeight(DISP)),config->GetWidth(),((linenum+1)*config->GetCharHeight(DISP)),COLORED);
	paint->DrawStringAt((((config->GetWidth()-2)/2)-((head.length()/2))*config->GetCharWidth(DISP)),FIRSTLINE+(linenum*config->GetCharHeight(DISP)),head.c_str(),config->GetFont(DISP), UNCOLORED);
}

void display::PrintValueLine(int x, int linenum, int left, String name, String type, String value, smarthome *config) {
	paint->DrawStringAt(x, FIRSTLINE + (linenum * config->GetCharHeight(DISP)),name.c_str(), config->GetFont(DISP), COLORED);
	if ( strcmp ( type.c_str(), "" ) ) {
		paint->DrawStringAt(config->GetWidth()-2-(type.length()*config->GetCharWidth(DISP)), FIRSTLINE + (linenum * config->GetCharHeight(DISP)), type.c_str(), config->GetFont(DISP), COLORED);
		paint->DrawStringAt(config->GetWidth()-2-(2*config->GetCharWidth(DISP))-(value.length()*config->GetCharWidth(DISP)) - (left*config->GetCharWidth(DISP)), FIRSTLINE + (linenum * config->GetCharHeight(DISP)), value.c_str(), config->GetFont(DISP), COLORED);
	} else {
		paint->DrawStringAt(config->GetWidth()-2-(value.length()*config->GetCharWidth(DISP))-(left*config->GetCharWidth(DISP)), FIRSTLINE + (linenum * config->GetCharHeight(DISP)), value.c_str(), config->GetFont(DISP), COLORED);
	}
}

void display::EnableUpdate() {
	Update=true;
}

bool display::GetUpdate() {
	return(Update);
}

void display::updatelast() {
	lasttime=millis();
}

unsigned int display::getLastTime() {
	return(lasttime);
}

void display::SetDate(String datestring) {
	date=datestring;
}

String display::GetDate() {
	return(date);
}
	
