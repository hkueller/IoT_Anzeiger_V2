#include <config.h>
#include <Display.h>

display::display() {
	//Used to get Memory from System
	void *pd;

	//Request memory for the framebuffer
	framebuffer=(unsigned char *) malloc(15000*sizeof(unsigned char));
	//Request Space for paint class
	pd=malloc(sizeof(Paint));
	//and generate one instance of the class
	paint=new(pd) Paint(framebuffer,400,300);
	lasttime = millis() - ( UPDATE_MINUTE * 1000 * 60 );
}

display::display(long leistung, long leistung_ost, long leistung_west, long leistung_batt, long leistung_grid, long verbrauch, long leistung_heizstab, float batterie, long leistung_wallbox, float temp, float feuchte, float druck, float gewicht, float hz_gewicht, String hz_fehler) {
	//Used to get Memory from System
	void *pd;

	//alloc space for the Framebuffer
	framebuffer=(unsigned char *) malloc(15000*sizeof(unsigned char));
	//alloc space for the painting class
	pd=malloc(sizeof(Paint));
	//create one instance of the painting class
	paint=new(pd) Paint(framebuffer,400,300);
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

void display::LoadFrame(smarthome *data) {
	//some later needed vars
	//for calculating the position
	//of Values
	String headline;
	//one char has the width of 17 pixel
	//int charsize=CHARSIZE;
	//clear the painting buffer
	paint->Clear(UNCOLORED);
	//draw the frames on the display
#ifdef LANDSCAPE
	paint->DrawVerticalLine(1,1,399,COLORED);
	paint->DrawVerticalLine(299,1,399,COLORED);
	paint->DrawHorizontalLine(1,1,299,COLORED);
	paint->DrawHorizontalLine(1,FIRSTLINE+(5*LINESPACE)-1,299,COLORED);
	paint->DrawHorizontalLine(1,FIRSTLINE+(9*LINESPACE)-1,299,COLORED);
	paint->DrawHorizontalLine(1,399,299,COLORED);
#else
	paint->DrawVerticalLine(1,1,299,COLORED);
	paint->DrawVerticalLine(399,1,299,COLORED);
	paint->DrawHorizontalLine(1,1,399,COLORED);
	paint->DrawHorizontalLine(1,FIRSTLINE+(5*LINESPACE)-1,399,COLORED);
	paint->DrawHorizontalLine(1,FIRSTLINE+(9*LINESPACE)-1,399,COLORED);
	paint->DrawHorizontalLine(1,299,399,COLORED);
#endif
	//draw the text context and values on the display
	data->SetFirst();
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
			headline=data->GetName() + date;
		} else {
#ifdef DEBUG
			Serial.println("Headline" + data->GetName() + "add");
			Serial.flush();
#endif
			headline=data->GetName();
		}
		PrintHeadLine(data->GetLine(),headline);
	} else {
#ifdef DEBUG
		Serial.println("Adding DataEntry for" + data->GetLine());
		Serial.flush();
#endif
		PrintValueLine(3,data->GetLine(),0,data->GetName(),data->GetEinheit(),data->GetData());
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
				headline=data->GetName() + date;
			} else {
#ifdef DEBUG
				Serial.println("Headline" + data->GetName() + "add");
				Serial.flush();
#endif
				headline=data->GetName();
			}
			PrintHeadLine(data->GetLine(),headline);
		} else {
			if( data->GetLine() > 0 ) {
#ifdef DEBUG
				Serial.println("Adding DataEntry for" + data->GetLine());
				Serial.flush();
#endif
				PrintValueLine(3,data->GetLine(),0,data->GetName(),data->GetEinheit(),data->GetData());
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

void display::PrintValueLine(int x, int linenum, int left, String name, String type, String value) {
	paint->DrawStringAt(x, FIRSTLINE + ( linenum * LINESPACE ), name.c_str(), &INFOFONT, COLORED);
	if( strcmp ( type.c_str(), "" ) ) {
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(type.length()*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), type.c_str(), &INFOFONT, COLORED);
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(2*CHARSIZE)-(value.length()*CHARSIZE)-(left*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), value.c_str(), &INFOFONT, COLORED);
	} else {
		paint->DrawStringAt((TYPEPOS*CHARSIZE)-(value.length()*CHARSIZE)-(left*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), value.c_str(), &INFOFONT, COLORED);
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
	
