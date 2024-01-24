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
        fhem("set \<mqtt2server\> publish display/conf "\\"\<Orientation\>,\<FontSize\>\"");<br>
        fhem("set \<mqtt2server\> publish display/dash "\\"\<numofdashes\>,\<linenumber\>,\<linenumer\>,...\"");<br>
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

mqtt topic display/conf:<br>
"0,24" -> Vertikale Ausrichtung, Fontgrösse 24<br>

-> die erste Ziffer gibt die Ausrichtung des Displays an. 0 Entspricht dabei Querformat, 3 Hochformat.<br>
-> die zweite Ziffer gibt die Fontgrösse an. Verwendet werden können 8,12,16,20 und 24.<br>

mqtt topic display/dash:<br>
Hiermit kann das Display mit Horizontalen Linien Unterteilt werden.<br>
"2,5,9" -> Es werden 2 Horizontalen Linien gezeichnet, Eine zwischen der 5. und 6. Zeile, eine zwischen der 9. und 10. Zeile<br>
dabei wird mit der ersten Zahl die Anzahl der horizontalen Linien angegeben. Danach müssen kommagetrennt ganuso viele Ziffern folgen,<br>
die jeweils Zeilennummern, nach denen die Linie gezeichnet werden soll, entsprechen.<br>

display/line_\<linenumber\><br>
"Solarertrag,E3DC,power,W,3" <br>
Solarertrag -> Bezeichner wird linksbündig in der Zeile (mit 3 Pixel abstand vom Rand) dargestellt<br>
E3DC->Gerätename in fhem welcher abgefragt wird.<br>
power -> Reading des Gerätes, das den Gewünschten Wert Enthält<br>
Das Reading wird rechtbündig angezeigt - mit Platz für die Einheit.<br>
W -> Einheit (sollte nicht mehr als 3 Zeichen enthalten)<br><br>

Enthält der Sting mehrere Daten getrennt durch "," Werden diese folgend ausgewertet:<br><br>
\<Name\>\[,\<fhem_device\>,\<reading\>,\[\<Einheit\>\],\<position\>\]<br><br>
Name = Titel der auf dem Display Linksbündig (position beachten) in der Zeile dargestellt wird.<br>
fhem_device = Device name in FHEM von welchem das Reading verwendet werden soll <br>
reading = Reading des fhem_device, welches die gewünschten Daten liefert<br>
Einheit = Wenn angegeben wird der String hinter den Daten angezeigt (Einheit des Wertes)<br>
position = Name wird um anzahl <position> zeichen nach Rechts eingerückt<br><br>

"Wetterinformationen" <br>
Enthält der Eintrag nur einen String, wird dieser als Headline erkannt, und in der Zeile Mittig Platziert.<br><br>

Dazu gild noch, das linenumber 0 (also display/line_0) eine Sonderfunktion hat. <br>
Dargestellt werden auf dem Display nur Headline Einträge. In dieser Zeile wird der String Linksbündig dargestellt,
und Rechtbündig noch Datum und Uhzeit der letzten Datenaktualisierung hinzugefügt.<br>
Andere Einträge, die mit fhem Device und Reading angegeben werden, werden ausschliesslich auf der <br>
Website (nur http (port 80)), die von dem Device ausgeliefert wird, dargestellt.<br>

