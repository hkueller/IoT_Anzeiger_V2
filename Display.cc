#include <config.h>
#include <Display.h>

display::display() {
	//Used to get Memory from System
	void *pd;

	//Request memory for the framebuffer
	//framebuffer=(unsigned char *) malloc(15000*sizeof(unsigned char));
	framebuffer=(unsigned char *) malloc(((DISPLAY_HEIGHT * DISPLAY_WIDTH)/8) * sizeof(unsigned char));
	//Request Space for paint class
	pd=malloc(sizeof(Paint));
	//and generate one instance of the class
	paint=new(pd) Paint(framebuffer,DISPLAY_HEIGHT,DISPLAY_WIDTH);
	lasttime = millis() - ( UPDATE_MINUTE * 1000 * 60 );
}

display::display(long leistung, long leistung_ost, long leistung_west, long leistung_batt, long leistung_grid, long verbrauch, long leistung_heizstab, float batterie, long leistung_wallbox, float temp, float feuchte, float druck, float gewicht, float hz_gewicht, String hz_fehler) {
	//Used to get Memory from System
	void *pd;

	//alloc space for the Framebuffer
	//framebuffer=(unsigned char *) malloc(15000*sizeof(unsigned char));
	framebuffer=(unsigned char *) malloc(((DISPLAY_HEIGHT * DISPLAY_WIDTH)/8) * sizeof(unsigned char));
	//alloc space for the painting class
	pd=malloc(sizeof(Paint));
	//create one instance of the painting class
	paint=new(pd) Paint(framebuffer,DISPLAY_HEIGHT,DISPLAY_WIDTH);
	lasttime = millis() - ( UPDATE_MINUTE * 1000 * 60 );
}

void display::begin() {
	//Initialize the E-Paper display
	epd.Init();
	//Fillup framebuffer with startup information
	paint->Clear(UNCOLORED);
#ifdef LANDSCAPE
	paint->SetRotate(3);
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("IOT Display").length()/2))*INITCHARSIZE,200-INITCHARSIZE,"IOT Display", &INITFONT,COLORED);
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("Starting Up").length()/2))*INITCHARSIZE,200+INITCHARSIZE,"Starting Up", &INITFONT,COLORED);
#else
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("IOT Display").length()/2))*INITCHARSIZE,150-INITCHARSIZE,"IOT Display", &INITFONT,COLORED);
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("Starting Up").length()/2))*INITCHARSIZE,150+INITCHARSIZE,"Starting Up", &INITFONT,COLORED);
#endif
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}

void display::begin(smarthome *config) {
	//Initialize the E-Paper display
	epd.Init();
	//Fillup framebuffer with startup information
	paint->Clear(UNCOLORED);
	
	paint->SetRotate(config->GetOrientation());
	paint->DrawStringAt(\
		(((config->GetWidth()-2)/config->GetCharWidth(MSG))/2) - ((String("IOT Display").length()/2)*config->GetCharWidth(MSG)),\
		((config->GetHeight()-2)/2)-config->GetCharHeight(MSG),\
		"IOT Display",\
		config->GetFont(MSG),\
		COLORED\
	);
	paint->DrawStringAt(\
		(((config->GetWidth()-2)/config->GetCharWidth(MSG))/2) - ((String("Starting Up").length()/2)*config->GetCharWidth(MSG)),\
		(config->GetHeight()-2)/2,\
		"Starting Up",\
		config->GetFont(MSG),\
		COLORED\
	);
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}

void display::Message(String message) {
	paint->Clear(UNCOLORED);
#ifdef LANDSCAPE
	paint->SetRotate(3);
#define WIDTH 200
#else
#define WIDTH 150
#endif
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("IOT Display").length()/2))*INITCHARSIZE,WIDTH-INITCHARSIZE,"IOT Display", &INITFONT,COLORED);
	paint->DrawStringAt(((INITTYPEPOS/2)-(message.length()/2))*INITCHARSIZE,WIDTH+INITCHARSIZE,message.c_str(), &INITFONT,COLORED);
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}

void display::Message(String message, smarthome *config) { 
	paint->Clear(UNCOLORED);
	paint->SetRotate(config->GetOrientation());
	paint->DrawStringAt(\
		(((config->GetWidth()-2)/config->GetCharWidth(MSG))/2) - ((String("IOT Display").length()/2)*config->GetCharWidth(MSG)),\
		((config->GetHeight()-2)/2)-config->GetCharHeight(MSG),\
		"IOT Display",\
		config->GetFont(MSG),\
		COLORED\
	);
	paint->DrawStringAt(\
		(((config->GetWidth()-2)/config->GetCharWidth(MSG))/2) - ((message.length()/2)*config->GetCharWidth(MSG)),\
		(config->GetHeight()-2)/2,\
		message.c_str(),\
		config->GetFont(MSG),\
		COLORED\
	);
	epd.DisplayFrame(framebuffer);
}

void display::LoadFrame(smarthome *data) {
	//some later needed vars
	//for calculating the position
	//of Values
	String headline;
	//one char has the width of 17 pixel
	//int charsize=CHARSIZE;
	//clear the painting buffer
	paint->SetRotate(data->GetOrientation());
	paint->Clear(UNCOLORED);
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

void display::PrintHeadLine(int linenum, String head) {
	paint->DrawStringAt(((TYPEPOS/2)-(head.length()/2))*CHARSIZE,FIRSTLINE+(linenum*LINESPACE),head.c_str(), &INFOFONT, COLORED);
}

void display::PrintHeadLine(String head, String date, smarthome *config) {
	paint->DrawStringAt(3,FIRSTLINE,head.c_str(),config->GetFont(DISP),COLORED);
	paint->DrawStringAt((config->GetWidth()-2)-(date.length() * config->GetCharWidth(DISP)),FIRSTLINE,date.c_str(),config->GetFont(DISP),COLORED);
}

void display::PrintHeadLine(int linenum, String head, smarthome *config) {
	paint->DrawStringAt((((config->GetWidth()-2)/2)-((head.length()/2))*config->GetCharWidth(DISP)),FIRSTLINE+(linenum*config->GetCharHeight(DISP)),head.c_str(),config->GetFont(DISP), COLORED);
}

void display::PrintValueLine(int x, int linenum, int left, String name, String type, String value) {
	paint->DrawStringAt(x, FIRSTLINE + ( linenum * LINESPACE ), name.c_str(), &INFOFONT, COLORED);
	if( strcmp ( type.c_str(), "" ) ) {
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(type.length()*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), type.c_str(), &INFOFONT, COLORED);
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(2*CHARSIZE)-(value.length()*CHARSIZE)-(left*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), value.c_str(), &INFOFONT, COLORED);
	} else {
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(value.length()*CHARSIZE)-(left*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), value.c_str(), &INFOFONT, COLORED);
	}
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
	
