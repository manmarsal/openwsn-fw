#!/bin/bash
#MYLCD="/home/pi/1wire/29.69B60B000000"
MYLCD="FF.7F0800000100"

# Get sensor values

#COMEDOR
#SENSOR1=`cat /home/pi/1wire/28.38E752030000/temperature10 | sed 's/^[ \t]*//'`
#AZUL
#SENSOR2=`owread -C -s 192.168.1.2 28.45E81A030000/temperature9 | sed 's/^[ \t]*//'`
#SENSOR2=`cat /home/pi/1wire/28.45E81A030000/temperature10 | sed 's/^[ \t]*//'`
#RECIBIDOR
#SENSOR3=`owread -C -s 192.168.1.2 28.16EE52030000/temperature9 | sed 's/^[ \t]*//'`
#SENSOR3=`cat /home/pi/1wire/28.16EE52030000/temperature10 | sed 's/^[ \t]*//'`
#COCINA
#SENSOR4=`owread -C -s 192.168.1.2 28.0E1D53030000/temperature9 | sed 's/^[ \t]*//'`
#SENSOR4=`cat /home/pi/1wire/28.0E1D53030000/temperature10 | sed 's/^[ \t]*//'`
#HAB MAT
#SENSOR5=`owread -C -s 192.168.1.2 28.1B1E53030000/temperature9 | sed 's/^[ \t]*//'`
#SENSOR5=`cat /home/pi/1wire/28.1B1E53030000/temperature10 | sed 's/^[ \t]*//'`
#NARANJA
#SENSOR6=`cat /home/pi/1wire/28.C21C26030000/temperature10 | sed 's/^[ \t]*//'`
#TERRAZA
#SENSOR7=`cat /home/pi/1wire/28.2D0B26030000/temperature10 | sed 's/^[ \t]*//'`


# screen is 2x16 lines
LINEA1="$MYLCD/line20.0"
LINEA2="$MYLCD/line20.1"
LINEA3="$MYLCD/line20.2"
LINEA4="$MYLCD/line20.3"

fecha=`date`
dia=`date +"%Y-%m-%d"`
dia_anterior=`date +"%Y-%m-%d" --date='-1 day'`
hora=`date +"%H:%M:%S"`
#mem=`./memlibre.sh`
#servicios=`cat /home/pi/status/servicios.txt`
#printservicios="?"

#if [ $servicios = "0" ];
#then printservicios="OK"
#else printservicios="NG"
#fi



#mysql -D temperaturas -u root -pchaosium -e "INSERT INTO historia VALUES ('$dia','$hora',$SENSOR1,$SENSOR3,$SENSOR7,$SENSOR6,$SENSOR2,$SENSOR5,$SENSOR4,'0','0','0','0','0','0');"
#mysql -D temperaturas -u root -pchaosium -e "INSERT INTO historia VALUES ('$dia','$hora',$SENSOR1,$SENSOR3,$SENSOR7,$SENSOR6,$SENSOR2,$SENSOR5,$SENSOR4,'0','0','0','0','0','0');"

media_azul=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(azul) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_azul=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(azul) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_azul=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(azul) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_terraza=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(terraza) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_terraza=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(terraza) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_terraza=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(terraza) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_comedor=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(comedor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_comedor=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(comedor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_comedor=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(comedor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_cocina=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(cocina) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_cocina=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(cocina) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_cocina=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(cocina) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_naranja=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(naranja) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_naranja=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(naranja) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_naranja=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(naranja) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_matrimonio=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(matrimonio) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_matrimonio=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(matrimonio) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_matrimonio=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(matrimonio) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

media_recibidor=`mysql -D temperaturas -u root -pchaosium -e "SELECT AVG(recibidor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
max_recibidor=`mysql -D temperaturas -u root -pchaosium -e "SELECT MAX(recibidor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`
min_recibidor=`mysql -D temperaturas -u root -pchaosium -e "SELECT MIN(recibidor) FROM historia WHERE dia = DATE(DATE_SUB(NOW(), INTERVAL 1 DAY))" | sed '1d'`

#echo $dia_anterior
#echo $max_azul
#echo $min_azul
#echo $media_azul

#echo $media_terraza
#echo $media_comedor
#echo $media_cocina
#echo $media_naranja
#echo $media_matrimonio
#echo $media_recibidor

mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_azul VALUES ('$dia_anterior',$max_azul,$min_azul,$media_azul);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_terraza VALUES ('$dia_anterior',$max_terraza,$min_terraza,$media_terraza);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_comedor VALUES ('$dia_anterior',$max_comedor,$min_comedor,$media_comedor);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_cocina VALUES ('$dia_anterior',$max_cocina,$min_cocina,$media_cocina);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_naranja VALUES ('$dia_anterior',$max_naranja,$min_naranja,$media_naranja);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_matrimonio VALUES ('$dia_anterior',$max_matrimonio,$min_matrimonio,$media_matrimonio);"
mysql -D temperaturas -u root -pchaosium -e "INSERT INTO maxmin_recibidor VALUES ('$dia_anterior',$max_recibidor,$min_recibidor,$media_recibidor);"


# message is 1x80 line in 4 bit mode
#LCD2="$MYLCD/LCD_H/message"

# Sensor values
MSG1="$max_terraza"
MSG2="$min_terraza"
MSG3="$media_terraza"

#MSG4="$SENSOR4"
#MSG5="$SENSOR5"
#MSG6="$SENSOR6"
#MSG7="$SENSOR7"

# Sensor names can be whatever you like
LABEL1="COME"
LABEL2="AZUL"
LABEL3="RECI"
LABEL4="COCI"
LABEL5="MATR"
LABEL6="NARA"
LABEL7="TERR"

# Initialise the display
#printf "\x" > $LINEA1
#sleep 1
#printf "\x" > $LINEA2
#sleep 1
#printf "1" > $MYLCD/LCDon
#owwrite -s 192.168.1.2 FF.7F0800000100/LCDon 0
#owwrite -s 192.168.1.2 FF.7F0800000100/LCDon 1

owwrite /home/pi/1wire/FF.7F0800000100/LCDon 0
owwrite /home/pi/1wire/FF.7F0800000100/LCDon 1
owwrite "$MYLCD"/line20.0 "                    "
owwrite "$MYLCD"/line20.1 "                    "
owwrite "$MYLCD"/line20.2 "                    "
owwrite "$MYLCD"/line20.3 "                    "

#sleep 1
#printf "0" > $MYLCD/backlight
#owwrite -s 192.168.1.2 FF.7F0800000100/backlight 0
owwrite "$MYLCD"/backlight 0
#sleep 1
#printf "1" > $MYLCD/backlight
#owwrite -s 192.168.1.2 "$MYLCD"/backlight 1
owwrite "$MYLCD"/backlight 1

# Print the display lines in the
# order 1 3 2 4 for 4 line by 20
# character display as it is 2x40
# in 4 bit mode

#for INDEX in 1 2

#do
	owwrite "$MYLCD"/line20.0 "Terraza $dia"
	owwrite "$MYLCD"/line20.1 "MAX --> $max_terraza"
	owwrite "$MYLCD"/line20.2 "MIN --> $min_terraza"
	owwrite "$MYLCD"/line20.3 "MEDIA --> $media_terraza"
	sleep 3
	#owwrite /home/pi/1wire/"$MYLCD"/LCDon 0
	#owwrite /home/pi/1wire/"$MYLCD"/LCDon 1

	#printf "$LABEL1"":""$MSG1"" ""$LABEL2"":""$MSG2" > $LINEA1
	#owwrite "$MYLCD"/line20.0 "$LABEL1"":""$MSG1"" ""$LABEL2"":""$MSG2" 
	#owwrite "$MYLCD"/line20.1 "$LABEL3"":""$MSG3"" ""$LABEL4"":""$MSG4"
	#owwrite "$MYLCD"/line20.2 "$LABEL5"":""$MSG5"" ""$LABEL6"":""$MSG6"
	#owwrite "$MYLCD"/line20.3 "     $LABEL7"":""$MSG7"

	#sleep 10
	#owwrite FF.7F0800000100/LCDon 0
	#owwrite FF.7F0800000100/LCDon 1
	#owwrite "$MYLCD"/line16.0 "$LABEL5"":""$MSG5"" ""$LABEL6"":""$MSG6"
	#owwrite "$MYLCD"/line16.1 "$LABEL7"":""$MSG7"
	#sleep 10
	#owwrite "$MYLCD"/backlight 0


	#printf "$LABEL3"":""$MSG3"" ""$LABEL4"":""$MSG4" > $LINEA2
	#printf "$LABEL2"":""MSG2" > $LINEA1

	#printf "$MSG1" > $LINEA1
	#printf " " > $LINEA1
	#printf "$LABEL2" > $LINEA1
        #printf ":" > $LINEA1
        #printf "$MSG2" > $LINEA1

	#printf "$LABEL2" > $LINEA1
	#printf "$MSG1" > $LINEA2
	#sleep 1

#done
