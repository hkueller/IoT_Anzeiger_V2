#!/bin/bash

#Here is an example how to sync the IOT_Anzeiger with fhem
#just call this with an cronjob somewhere in your network.
#it will ask fhem for some devices and values,
#and create a script which is working with netcat
#and interact witht he IOT_Anzeiger to set the Values there.

set -x

DisplayHost=display.wlan.compuextreme.de
FHEMHost=rahanas.wlan.compuextreme.de

date=$(date +%d"."%m" "%H":"%M)

#E3DC is a S10 SolarSystem - sunwat is the value of the energy collected with the PV Panels
Leistung=$(echo "list E3DC sunwatt"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - s2_p show the pannel power on the East Side
Leistung_ost=$(echo "list E3DC s2_p"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - s1_p show the pannel power oin the West Side
Leistung_west=$(echo "list E3DC s1_p"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - battwat show the energie SendTO/Received from Batterysystem
Leistung_batt=$(echo "list E3DC battwatt"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - gridwatt show the energie SendTo/Received from external
Leistung_grid=$(echo "list E3DC gridwatt"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - homewatt show the amount of local used Energie
Verbrauch=$(echo "list E3DC homewatt"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#E3DC is a S10 SolarSystem - battsoc show the amount of load in the battery system.
Batt=$(echo "list E3DC battsoc"|nc $FHEMHost -w 5 7072|awk '{print $NF}')
#gt_aWetter is a arduino based (AskSin++ Based) Sensorbord located outside
#temperature -> the Temperature outside
Temp=$(echo list "gt_aWetter temperature"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#humidity -> the Humidity outside
Feucht=$(echo list "gt_aWetter humidity"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#Pressure -> the Air Pressure
Druck=$(echo list "gt_aWetter pressure"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')
#kg_heizung_pellets is also a ESP8266 device which uses Ultrasonic distance sensores,
#to get the amount of pellets stored in the heaterroom
Pellets=$(echo list "kg_heizung_pellets pellets"|nc -w 5 $FHEMHost 7072|awk '{print $NF}')

TMPFILE=$(mktemp)
cat >$TMPFILE <<EOF
#!/bin/bash

while read -n 1 PROMPT; do
	if [ \$PROMPT == "\$" ]; then
		echo "SetDate $date"
		read ANSWER
		echo "SetLeistung $Leistung"
		read ANSWER
		echo "SetLeistungOst $Leistung_ost"
		read ANSWER
		echo "SetLeistungWest $Leistung_west"
		read ANSWER
		echo "SetLeistungBatt $Leistung_batt"
		read ANSWER
		echo "SetLeistungGrid $Leistung_grid"
		read ANSWER
		echo "SetVerbrauch $Verbrauch"
		read ANSWER
		echo "SetBatterie $Batt"
		read ANSWER
		echo "SetTemperatur $Temp"
		read ANSWER
		echo "SetFeuchte $Feucht"
		read ANSWER
		echo "SetDruck $Druck"
		read ANSWER
		echo "SetPellets $Pellets"
		read ANSWER
		exit
	fi
done
EOF
chmod 755 $TMPFILE
nc -w 10 $DisplayHost 5023 -c $TMPFILE
rm $TMPFILE
