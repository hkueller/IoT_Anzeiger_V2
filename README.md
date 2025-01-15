# IOT_Anzeiger

Firmware des IOT_Anzeiger devices, zur Anzeige einiger Daten aus FHEM per esp8266 und einem 4.2 Zoll E-Paper displays.<br>
## Hardware
### ESP8266 mit 4.2 Waveshare Display
Ich verwende für den Aufbau die D1 Boardvariante:<br>
https://www.amazon.de/AZDelivery-D1-Mini-NodeMcu-Parent/dp/B07ZQP9FSX

Als Display kommt ein Waveshare 4.2 E-Ink display zum Einsatz. z.B.:<br>
https://www.amazon.de/Waveshare-Kommunikation-Schnittstelle-Aufl%C3%B6sung-Embedded-Controller/dp/B074NR1SW2/ref=sr_1_1?keywords=waveshare+e-inc+4.2&qid=1705571386&sr=8-1

Obiges Display wird mit einem Adapterkabel geliefert, das sich direkt mit dem D1 Board verbinden lässt.<br><br>

```
Busy -> D2 (GPIO4)
RST  -> D1 (GPIO5)
DC   -> D3 (GPIO0)
CS   -> D8 (GPIO15, SS)
CLK  -> D5 (SCK)
DIN  -> D7 (MOSI)
GND  -> GND
3.3V -> 3.3V
```

Und damit ist die Schaltung auch schon Fertig.<br>

### ESP32
Die Software sollte grundsätzlich auch mit einem ESP32 Funktionieren. Hier werden allerdings noch einige Anpassungen bei den Defines und includes notwendig sein!<br>
### Andere Displays
Sofern diese mit den Waveshare Displays kompatibel sind, sollten diese Funktionieren.
wenn in `config.h` die korrekten Werte eingetragen werden.<br>
### Gehäuse
Im Verzeichniss `Case_3DDruck` befinden sich Freecad Daten für ein Gehäuse sammt Deckel und Ständer.
Der Ständer wird dabei am Geäuseboden angeschraubt - dazu sind kleine Selbstschneidende Schrauben Notwendig,
die auch für den Deckel verwendet werden sollten.
## konfiguration der Software
Vor dem Kompilieren der Firmware bitte einen Blick in `config.h` werfen.<br>
Hier sind einige Konfigurierbare Optionen.<br>
z.B. könnte man ein 7 Zoll mit üblicherweise 800x600 Auflösung verwenden.<br>
in dem Fall die Defines:<br>
```
#define DISPLAY_HEIGHT 400
#define DISPLAY_WIDTH 300
```
Entsprechend anpassen.<br>

Auch wird hier der Hostname des FHEM Servers, mqtt servers, NTP Server usw. eingestellt.<br>

Auch sollte ein Blick in `platformio.init` geworfen werden.<br>
Per default versucht das einen OTA Upload der Firmware. Ist das nicht erwünscht müssen die Zeilen:<br>
```
upload_protocol = espota
upload_port = display.wlan.compuextreme.de
upload_flags =
        --port=8266
```
auskommentiert werden!<br>

Nach dem anpassen, kann die Software unter Linux mit code/platformio übersetzt werden:
```
# pio run -t upload
```

## Netwerk Setup
In dem Projekt wird der WifiManager für das Netzwerk Setup verwendet.<br>
Eine Dokumentation hierzu gibt es im Internet:<br>

https://github.com/tzapu/WiFiManager.git

ist dem ESP keine Netzwerkkonfig bekannt, meldet dieser sich als AccessPoint mit der SSID "IOTAnzeiger".<br>
Mit diesem muss man sich Verbinden, und dann der Dokumentation des WiFiManager Projektes folgen.<br>

## configuration in FHEM
Zur Verwendung von IOT_Anzeiger_V2 ist ein MQTT Server notwendig.
Hier kann jeder beliebige MQTT Server verwendet werden. Der Einfachheit halber hier
das Setup eine solchen in FHEM (fhem liefert einen integrierten MQTT mit - dazu gibt es mehr im fhem wiki)
```
define mqtt2server MQTT_SERVER 1883 global
```

Wichtig: vor dem Übersetzen die Netzwerk Konfiguration (fhem-server, mqtt) anpassen!<br>
Dies wird in der config.h erledigt:<br>

`FHEM_HOST` -> hier wird die IP oder der Hostname der FHEM Instanz eingetragen<br>
`FHEM_PORT` -> Telnet Port Nummer der FHEM Instanz (default: 7072)<br>
`MQTT_SERVER` -> IP oder Hostname des MQTT Servers (FHEM Internet MQTT = Identisch to FHEM_HOST)<br>
`MQTT_REQUEST_TOPIC` -> Entspricht dem Topic, an welches ein `true` geschickt wird,<br>
                wenn das Display die Konfiguration von FHEM erfraegt.<br>
`MQTT_SUBSCRIBE_TOPIC` -> Wichtig, dass alle nachrichten des basic Topic (for dem "/") abgeholt werden ("+")<br>

IOT_Anzeiger_V2 Verwendet ein MQTT2 Device in FHEM, sowie die Telnet Schnittstelle von FHEM.<br>
MQTT1 wird verwendet um die Initiale Konfiguration zu laden, Telnet um Readings von Geräten abzufragen.<br>
Aktuell wird nur eine Textausgabe ohne Grafiken unterstützt.<br>

Das Fhem Gerät lässt ich folgendermassen erstellen:<br>
```
define <displaname> MQTT2_DEVICE <your mqtt server>
attr <displayname> readingList display/get_config:* RequestConfig
attr <displayname> userReadings SetConfig:RequestConfig.* {
    if ( ReadingsVal($name,RequestConfig",0) eq "true") {
        fhem("set <mqtt2server> publish display/conf "\"<Orientation>,<FontSize>\"");
        fhem("set <mqtt2server> publish display/dash "\"<numofdashes>,<linenumber>,<linenumer>,...\"");
        fhem("set <mqtt2server> publish display/line_0 "\"Name, <fhem_device>, <reading>, <Einheit>, <position>\"");
        #put additional config lines here
        fhem("set <mqtt2server> publish display/conf_done \"true\"");
    }
}
```
Natürlich kann hier jedes Kommando (achtung: Perl Syntax) eingesetzt werden, das einem mqtt server Daten schickt.<br>

`display/line_0` ist dabei das sogenannte Topic.<br>

### mqtt topic `display/conf`:<br>
`"0,24"` -> Vertikale Ausrichtung, Fontgrösse 24<br>

-> die erste Ziffer gibt die Ausrichtung des Displays an. 0 Entspricht dabei Querformat, 3 Hochformat.<br>
-> die zweite Ziffer gibt die Fontgrösse an. Verwendet werden können 8,12,16,20 und 24.<br>

### mqtt topic `display/dash`:<br>
Hiermit kann das Display mit Horizontalen Linien Unterteilt werden.<br>
`"2,5,9"` -> Es werden 2 Horizontalen Linien gezeichnet, Eine zwischen der 5. und 6. Zeile, eine zwischen der 9. und 10. Zeile<br>
dabei wird mit der ersten Zahl die Anzahl der horizontalen Linien angegeben. Danach müssen kommagetrennt ganuso viele Ziffern folgen,<br>
die jeweils Zeilennummern, nach denen die Linie gezeichnet werden soll, entsprechen.<br>

### mqtt topic `display/line_\<linenumber\>`:<br>
`"Solarertrag,E3DC,power,W,3"` <br>
Solarertrag -> Bezeichner wird linksbündig in der Zeile (mit 3 Pixel abstand vom Rand) dargestellt<br>
E3DC->Gerätename in fhem welcher abgefragt wird.<br>
power -> Reading des Gerätes, das den Gewünschten Wert Enthält<br>
Das Reading wird rechtbündig angezeigt - mit Platz für die Einheit.<br>
W -> Einheit (sollte nicht mehr als 3 Zeichen enthalten)<br><br>

Enthält der Sting mehrere Daten getrennt durch "," Werden diese folgend ausgewertet:<br><br>
`<Name>[,<fhem_device>,<reading>,[<Einheit>],<position>]`<br>
Name = Titel der auf dem Display Linksbündig (position beachten) in der Zeile dargestellt wird.<br>
fhem_device = Device name in FHEM von welchem das Reading verwendet werden soll <br>
reading = Reading des fhem_device, welches die gewünschten Daten liefert<br>
Einheit = Wenn angegeben wird der String hinter den Daten angezeigt (Einheit des Wertes)<br>
position = Name wird um anzahl <position> zeichen nach Rechts eingerückt<br><br>

`"Wetterinformationen"`<br>
Enthält der Eintrag nur einen String, wird dieser als Headline erkannt, und in der Zeile Mittig Platziert.<br><br>

Dazu gild noch, das linenumber 0 (also `display/line_0`) eine Sonderfunktion hat. <br>
Dargestellt werden auf dem Display nur Headline Einträge. In dieser Zeile wird der String Linksbündig dargestellt,
und Rechtbündig noch Datum und Uhzeit der letzten Datenaktualisierung hinzugefügt.<br>
Andere Einträge, die mit fhem Device und Reading angegeben werden, werden ausschliesslich auf der <br>
Website (nur http (port 80)), die von dem Device ausgeliefert wird, dargestellt.<br>

### mqtt topic `display/conf_done`:<br>
Das sollte nach abschluss aller konfigurations Daten gesendet werden.<br>
Das sorgt dafür das der Config Modus beendet wird, Die Werte von FHEM geholt werden,
und das E-Ink Display mit den Daten upgedated wird.
