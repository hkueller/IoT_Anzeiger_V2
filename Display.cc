#include <Display.h>
#include <config.h>

display::display() {
	//Used to get Memory from System
	void *pd;

	//init the privat values with zero
	PV_Leistung=0;
	PV_Verbrauch=0;
	PV_Batterie=0;
	WT_Temperatur=0;
	WT_Feuchte=0;
	WT_Druck=0;
	PL_Gewicht=0;
	//Request memory for the framebuffer
	framebuffer=(unsigned char *) malloc(15000*sizeof(unsigned char));
	//Request Space for paint class
	pd=malloc(sizeof(Paint));
	//and generate one instance of the class
	paint=new(pd) Paint(framebuffer,400,300);
	lasttime = millis() - ( UPDATE_MINUTE * 1000 * 60 );
}

display::display(long leistung, long verbrauch, int batterie, float temp, float feuchte, float druck, float gewicht) {
	//Used to get Memory from System
	void *pd;

	//Initialize the private Vars with the given Values
	PV_Leistung=leistung;
	PV_Verbrauch=verbrauch;
	PV_Batterie=batterie;
	WT_Temperatur=temp;
	WT_Feuchte=feuchte;
	WT_Druck=druck;
	PL_Gewicht=gewicht;
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
	paint->DrawStringAt(110, 110, "IOT Display", &Font24, COLORED);
	paint->DrawStringAt(110, 150, "Starting Up", &Font24, COLORED);
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}

void display::LoadFrame() {
	//some later needed vars
	//for calculating the position
	//of Values
	int strlen;
	//one char has the width of 17 pixel
	int charsize=17;
	//clear the painting buffer
	paint->Clear(UNCOLORED);
	//draw the frames on the display
	paint->DrawVerticalLine(1,1,299,COLORED);
	paint->DrawVerticalLine(399,1,299,COLORED);
	paint->DrawHorizontalLine(1,1,399,COLORED);
	paint->DrawHorizontalLine(1,108,399,COLORED);
	paint->DrawHorizontalLine(1,222,399,COLORED);
	paint->DrawHorizontalLine(1,299,399,COLORED);
	//draw the text context and values on the display
	paint->DrawStringAt(25, 5, "SolarInfo", &Font24, COLORED);
	paint->DrawStringAt(190, 5, String(date).c_str(), &Font24, COLORED);
	paint->DrawStringAt(3, 30, "PV-Ertrag:            W", &Font24, COLORED);
	//Get the length of the string to draw
	strlen=String(PV_Leistung).length();
	//Calculate the starting position (end must be on 343+charsize pixel pos
	strlen=343+charsize-(strlen*charsize);
	//and now draw the value at the calculated position
	paint->DrawStringAt(strlen, 30, String(PV_Leistung).c_str(),&Font24,COLORED);
	//this will no be redone for every line to display
	paint->DrawStringAt(3, 54, "Verbrauch:            W", &Font24, COLORED);
	strlen=String(PV_Verbrauch).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 54, String(PV_Verbrauch).c_str(),&Font24,COLORED);
	paint->DrawStringAt(3, 78, "Batterie:             %", &Font24, COLORED);
	strlen=String(PV_Batterie).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 78, String(PV_Batterie).c_str(),&Font24,COLORED);
	paint->DrawStringAt(30, 120, "Wetter Informationen", &Font24, COLORED);
	paint->DrawStringAt(3, 144, "Temperatur            C", &Font24, COLORED);
	strlen=String(WT_Temperatur).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 144, String(WT_Temperatur).c_str(),&Font24,COLORED);
	paint->DrawStringAt(3, 168, "Feuchte               %", &Font24, COLORED);
	strlen=String(WT_Feuchte).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 168, String(WT_Feuchte).c_str(),&Font24,COLORED);
	paint->DrawStringAt(3, 192, "Luftdruck            hP", &Font24, COLORED);
	strlen=String(WT_Druck).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 192, String(WT_Druck,1).c_str(),&Font24,COLORED);
	paint->DrawStringAt(85,234,"Pellets Lager", &Font24, COLORED);
	paint->DrawStringAt(3,258,"Gewicht Bestand:      T", &Font24, COLORED);
	strlen=String(PL_Gewicht,3).length();
	strlen=343+charsize-(strlen*charsize);
	paint->DrawStringAt(strlen, 258, String(PL_Gewicht,3).c_str(),&Font24,COLORED);
	//Send the Buffer to the Display, and do a refresh with the contents
	//of this buffer (attention this will switch internal (display) the buffers
	epd.DisplayFrame(framebuffer);
	Update=false;
}


void display::LoadFrame(long leistung, long verbrauch, int batterie, float temp, float feuchte, float druck, float gewicht) {
	//Just assign the given Values
	PV_Leistung=leistung;
	PV_Verbrauch=verbrauch;
	PV_Batterie=batterie;
	WT_Temperatur=temp;
	WT_Feuchte=feuchte;
	WT_Druck=druck;
	PL_Gewicht=gewicht;
	//and redraw the screen on E-Paper
	LoadFrame();
}
void display::UpdatePVLeistung(long leistung) {
	//Just assign new value
	PV_Leistung=leistung;
}
void display::UpdatePVLeistung_ost(long leistung) {
	//Just assign new value
	PV_Leistung_ost=leistung;
}
void display::UpdatePVLeistung_west(long leistung) {
	//Just assign new value
	PV_Leistung_west=leistung;
}
void display::UpdatePVLeistung_batt(long leistung) {
	//Just assign new value
	PV_Leistung_batt=leistung;
}
void display::UpdatePVLeistung_grid(long leistung) {
	//Just assign new value
	PV_Leistung_grid=leistung;
}
void display::UpdatePVVerbrauch(long verbrauch) {
	//Just assign new value
	PV_Verbrauch=verbrauch;
}
void display::UpdatePVBatterie(int batterie) {
	//Just assign new value
	PV_Batterie=batterie;
}
void display::UpdatePVHeizstab(long leistung) {
	PV_Heizstab=leistung;
}
void display::UpdateWTTemperatur(float temperatur) {
	//Just assign new value
	WT_Temperatur=temperatur;
}
void display::UpdateWTFeuchte(float feuchte) {
	//Just assign new value
	WT_Feuchte=feuchte;
}
void display::UpdateWTDruck(float Druck) {
	//Just assign new value
	WT_Druck=Druck;
}
void display::UpdatePLGewicht(float Gewicht) {
	//Just assign new value
	PL_Gewicht=Gewicht;
}
long display::GetPVLeistung() {
	return(PV_Leistung);
}
long display::GetPVLeistung_ost() {
	return(PV_Leistung_ost);
}
long display::GetPVLeistung_west() {
	return(PV_Leistung_west);
}
long display::GetPVLeistung_batt() {
	return(PV_Leistung_batt);
}
long display::GetPVLeistung_grid() {
	return(PV_Leistung_grid);
}
long display::GetPVVerbrauch() {
	return(PV_Verbrauch);
}
int display::GetPVBatterie() {
	return(PV_Batterie);
}
long display::GetPVHeizstab() {
	return(PV_Heizstab);
}
float display::GetWTTemperatur() {
	return(WT_Temperatur);
}
float display::GetWTFeuchte() {
	return(WT_Feuchte);
}
float display::GetWTDruck() {
	return(WT_Druck);
}
float display::GetPLGewicht() {
	return(PL_Gewicht);
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
	
