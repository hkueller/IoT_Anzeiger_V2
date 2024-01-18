# IOT_Anzeiger

Firmware des IOT_Anzeiger devices, zur Anzeige einiger Daten aus FHEM per esp8266 und einem 4.2 Zoll E-Paper displays.<br>
Ich verwende für den Aufbau die D1 Boardvariante:<br>
https://www.amazon.de/AZDelivery-D1-Mini-NodeMcu-Parent/dp/B07ZQP9FSX

Als Display kommt ein Waveshare 4.2 E-Ink display zum Einsatz. z.B.:<br>
https://www.amazon.de/Waveshare-Kommunikation-Schnittstelle-Aufl%C3%B6sung-Embedded-Controller/dp/B074NR1SW2/ref=sr_1_1?keywords=waveshare+e-inc+4.2&qid=1705571386&sr=8-1

Obiges Display wird mit einem Adapterkabel geliefert, das sich direkt mit dem D1 Board verbinden lässt.<br><br>

Busy -> D2 (GPIO4)<br>
RST  -> D1 (GPIO5)<br>
DC   -> D3 (GPIO0)<br>
CS   -> D8 (GPIO15, SS)<br>
CLK  -> D5 (SCK)<br>
DIN  -> D7 (MOSI)<br>
GND  -> GND<br>
3.3V -> 3.3V<br><br>

Und damit ist die Schaltung auch schon Fertig.<br>

IOT_Anzeiger_V2 Verwendet ein MQTT2 Device in FHEM, sowie die Telnet Schnittstelle von FHEM.
MQTT1 wird verwendet um die Initiale Konfiguration zu laden, Telnet um Readings von Geräten abzufragen.
Aktuell wird nur eine Textausgabe ohne Grafiken unterstützt.<br>

Das Fhem Gerät lässt ich folgendermassen erstellen:<br><br>

define \<displaname\> MQTT2_DEVICE <your mqtt server><br>
attr \<displayname\> readingList display/get_config:* RequestConfig<br>
attr \<displayname\> userReadings SetConfig:RequestConfig.* \{<br>
    if ( ReadingsVal($name,RequestConfig",0) eq "true") \{<br>
        fhem("set \<mqtt2server\> publish display/line_0 "\\"Name, \<fhem_device\>, \<reading\>, \<Einheit\>, \<position\>\\"");<br>
    \}<br>
\}<br><br>

Natürlich kann hier jedes Kommando (achtung: Perl Syntax) eingesetzt werden, das einem mqtt server Daten schickt.<br>
Ich verwende in meinem Setup fhem als mqtt server. Dieser wird in fhem z.B. mit:<br>


define mqtt2server MQTT_SERVER 1883 global<br>

aufgesetzt.<br>

display/line_0 ist dabei das sogenannte Topic.<br>

dieses wird neben anderen wichtigen informationen in "config.h" konfiguriert.<br>
Dabei gild:<br>

FHEM_HOST -> hier wird IP oder Hostname der FHEM Instanz<br>
FHEM_PORT -> Telnet Port Nummer der FHEM Instanz (default: 7072)<br>
MQTT_SERVER -> IP oder Hostname des mqtt servers<br>
               wird der FHEM eigene verwendet ist dieser identisch mit dem FHEM_HOST<br>
MQTT_REQUEST_TOPIC -> Entspricht dem Topic, an welches ein "true" geschickt wird,<br>
                      wenn das Display die Configuration Requested<br>
MQTT_SUBSCRIBE_TOPIC -> Wichtig, dass alle nachrichten des basic Topic (vor dem "/") abgeolt werden ("+")<br>

Aktuell wird das Display noch 3 geteilt.<br>
Im oberen Drittel ist Platz für 5 Zeilen<br>
im mittleren Drittel ist Platz für 4 Zeilen<br>
im unteren Driteel ist platz für 3 Zeilen<br>

Ich plane das noch dynamisch zu gestalten.<br>

Aktuell können nur Text Daten dargestellt werden.<br>
Diese werden als Topic vom mqtt server übertragen. Dabei wird vom topic der Teil nach dem ersten "/" ausgewertet.<br>
Beginnt dieser mit "line_" wird das Topic ausgewertet.<br>
die anschliessende Nummer gibt die line number an, auf welchem der Wert dargestellt werden soll.<br>
Wird jetzt nur ein String in den Daten übertragen (wie z.B. "Solarinfo"), wird dieser als Headline (mittig) dargestellt.<br>
In Zeile 0 wird dabei noch Datum und Uhrzeit des letzten DatenRequest von fhem hinzugefügt.<br>
Enthält der Sting mehrere Daten getrennt durch "," Werden diese folgend ausgewertet:<br><br>
\<Name\>\[,\<fhem_device\>,\<reading\>,\[\<Einheit\>\],\<position\>\]<br><br>
Name = Titel der auf dem Display Linksbündig (position beachten) in der Zeile dargestellt wird.<br>
fhem_device = Device name in FHEM von welchem das Reading verwendet werden soll <br>
reading = Reading des fhem_device, welches die gewünschten Daten liefert<br>
Einheit = Wenn angegeben wird der String hinter den Daten angezeigt (Einheit des Wertes)<br>
position = Name wird um anzahl <position> zeichen nach Rechts eingerückt<br><br>
Wird nur Name angegeben, wertet IOT_Anzeiger_v2 das als Überschrift aus.<br><br>

Noch ein wichtiger Punkt zur Zeile 0:<br>
Zeile 0 Zeigt nur eine Überschrift an!<br>
IOT_Anzeiger_V2 startet zusätzlich einen webserver auf port 80. Hier können die Anzeigen des Displays zusätzlich ein einemn Browser dargetellt werden.<br>
Definitionen für die Zeile 0, die KEINE Überschrift sind, werden ebenfalls auf der Webseite dargestellt, nicht jedoch auf dem e-Ink display.<br>
