#include <config.h>
#include <Display.h>

display::display() {
	//Used to get Memory from System
	void *pd;

	//init the privat values with zero
	PV_Leistung=0;
	PV_Leistung_ost=0;
	PV_Leistung_west=0;
	PV_Leistung_batt=0;
	PV_Leistung_grid=0;
	PV_Verbrauch=0;
	PV_Heizstab=0;
	PV_Batterie=0;
	PV_WallboxWatt=0;
	WT_Temperatur=0;
	WT_Feuchte=0;
	WT_Druck=0;
	PL_Gewicht=0;
	HZ_Lager=0;
	HZ_Fehler="";
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

	//Initialize the private Vars with the given Values
	PV_Leistung=leistung;
	PV_Leistung_ost=leistung_ost;
	PV_Leistung_west=leistung_west;
	PV_Leistung_batt=leistung_batt;
	PV_Leistung_grid=leistung_grid;
	PV_Verbrauch=verbrauch;
	PV_Heizstab=leistung_heizstab;
	PV_Batterie=batterie;
	PV_WallboxWatt=leistung_wallbox;
	WT_Temperatur=temp;
	WT_Feuchte=feuchte;
	WT_Druck=druck;
	PL_Gewicht=gewicht;
	HZ_Lager=hz_gewicht;
	HZ_Fehler=hz_fehler;
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

void display::NetDone() {
	paint->Clear(UNCOLORED);
#ifdef LANDSCAPE
	paint->SetRotate(3);
#define WIDTH 200
#else
#define WIDTH 150
#endif
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("IOT Display").length()/2))*INITCHARSIZE,WIDTH-INITCHARSIZE,"IOT Display", &INITFONT,COLORED);
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("Network Online").length()/2))*INITCHARSIZE,WIDTH+INITCHARSIZE,"Network Online", &INITFONT,COLORED);
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}

void display::NetOffline() {
	paint->Clear(UNCOLORED);
#ifdef LANDSCAPE
	paint->SetRotate(3);
#define WIDTH 200
#else
#define WIDTH 150
#endif
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("IOT Display").length()/2))*INITCHARSIZE,WIDTH-INITCHARSIZE,"IOT Display", &INITFONT,COLORED);
	paint->DrawStringAt(((INITTYPEPOS/2)-(String("Network Offline").length()/2))*INITCHARSIZE,WIDTH+INITCHARSIZE,"Network Offline", &INITFONT,COLORED);
	//and Display the frame
	epd.DisplayFrame(framebuffer);
}


void display::LoadFrame() {
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
	headline="Solarinfo " + date;
	PrintHeadLine(0,headline);
	PrintValueLine(3,1,0,String("PV-Ertrag:"),String("W"),String(PV_Leistung));
	PrintValueLine(3,2,0,String("Verbrauch:"),String("W"),String(PV_Verbrauch));
	PrintValueLine(3,3,0,String("Batterie:"),String("%"),String(PV_Batterie));
	PrintValueLine(3,4,0,String("Wallbox:"),String("W"),String(PV_WallboxWatt));
	PrintHeadLine(5,String("Wetter Informationen"));
	PrintValueLine(3,6,0,String("Temperatur:"),String("C"),String(WT_Temperatur,2));
	PrintValueLine(3,7,0,String("Feuchte:"),String("%"),String(WT_Feuchte,2));
	PrintValueLine(3,8,1,String("Luftdruck:"),String("hP"),String(WT_Druck,1));
	PrintHeadLine(9,String("Pellets Heizung"));
	//PrintValueLine(3,10,0,String("Gew. Bestand:"),String("T"),String(PL_Gewicht,3));
	PrintValueLine(3,10,0,String("Bestand:"),String("T"),String(HZ_Lager,3));
	PrintValueLine(3,11,0,String("Hzg Status:"),String(HZ_Fehler),String(""));
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
	paint->DrawStringAt((TYPEPOS*CHARSIZE)-(type.length()*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), type.c_str(), &INFOFONT, COLORED);
	paint->DrawStringAt((TYPEPOS*CHARSIZE)-(2*CHARSIZE)-(value.length()*CHARSIZE)-(left*CHARSIZE), FIRSTLINE + (linenum * LINESPACE), value.c_str(), &INFOFONT, COLORED);
}

void display::LoadFrame(long leistung, long verbrauch, float batterie, long wallbox, float temp, float feuchte, float druck, float gewicht, String hz_status) {
	//Just assign the given Values
	PV_Leistung=leistung;
	PV_Verbrauch=verbrauch;
	PV_Batterie=batterie;
	PV_WallboxWatt=wallbox;
	WT_Temperatur=temp;
	WT_Feuchte=feuchte;
	WT_Druck=druck;
	PL_Gewicht=gewicht;
	HZ_Fehler=hz_status;
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
void display::UpdatePVBatterie(float batterie) {
	//Just assign new value
	PV_Batterie=batterie;
}
void display::UpdatePVWallboxWatt(long WallboxWatt) {
	PV_WallboxWatt=WallboxWatt;
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
void display::UpdateHzLager(float lager) {
	HZ_Lager=lager;
}
void display::UpdateHzFehler(String fehler) {
	HZ_Fehler=fehler;
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
float display::GetPVBatterie() {
	return(PV_Batterie);
}
long display::GetPVWallboxWatt() {
	return(PV_WallboxWatt);
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
float display::GetHzLager() {
	return(HZ_Lager);
}
String display::GetHzFehler() {
	return(HZ_Fehler);
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
	
