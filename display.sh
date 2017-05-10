#!/bin/bash

#MYLCD="/home/pi/1wire/29.69B60B000000"
MYLCD="FF.7F0800000100"

# LEER TEMPERATURA DE LOS SENSORES

#COMEDOR
#SENSOR1=`cat /home/pi/1wire/28.38E752030000/temperature10 | sed 's/^[ \t]*//'`
SENSOR1=`owread -C -s 192.168.1.7 /28.38E752030000/temperature10 | sed 's/^[ \t]*//'`
echo
echo COMEDOR
echo $SENSOR1
echo

#AZUL
SENSOR2=`owread -C -s 192.168.1.7 28.45E81A030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR2=`cat /home/pi/1wire/28.45E81A030000/temperature10 | sed 's/^[ \t]*//'`
echo AZUL
echo $SENSOR2
echo

#RECIBIDOR
SENSOR3=`owread -C -s 192.168.1.7 28.16EE52030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR3=`cat /home/pi/1wire/28.16EE52030000/temperature10 | sed 's/^[ \t]*//'`
echo RECIBIDOR
echo $SENSOR3
echo

#COCINA
SENSOR4=`owread -C -s 192.168.1.7 28.0E1D53030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR4=`cat /home/pi/1wire/28.0E1D53030000/temperature10 | sed 's/^[ \t]*//'`
echo COCINA
echo $SENSOR4
echo

#HAB MAT
SENSOR5=`owread -C -s 192.168.1.7 28.1B1E53030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR5=`cat /home/pi/1wire/28.1B1E53030000/temperature10 | sed 's/^[ \t]*//'`
echo MATRIMONIO
echo $SENSOR5
echo

#NARANJA
SENSOR6=`owread -C -s 192.168.1.7 28.C21C26030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR6=`cat /home/pi/1wire/28.C21C26030000/temperature10 | sed 's/^[ \t]*//'`
echo NARANJA
echo $SENSOR6
echo

#TERRAZA
SENSOR7=`owread -C -s 192.168.1.7 28.2D0B26030000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR7=`cat /home/pi/1wire/28.2D0B26030000/temperature10 | sed 's/^[ \t]*//'`
echo TERRAZA
echo $SENSOR7
echo

#RECIBIDOR
SENSOR8=`owread -C -s 192.168.1.7 28.75C2EA040000/temperature10 | sed 's/^[ \t]*//'`
#SENSOR8=`cat /home/pi/1wire/28.75C2EA040000/temperature10 | sed 's/^[ \t]*//'`
echo RECIBIDOR
echo $SENSOR8
echo

# screen is 2x16 lines
LINEA1="$MYLCD/line20.0"
LINEA2="$MYLCD/line20.1"
LINEA3="$MYLCD/line20.2"
LINEA4="$MYLCD/line20.3"

fecha=`date`
dia=`date +"%Y-%m-%d"`
hora=`date +"%H:%M:%S"`
mem=`./memlibre.sh`
servicios=`cat /home/pi/status/servicios.txt`
printservicios="?"

if [ $servicios = "0" ];
then printservicios="OK"
else printservicios="NG"
fi

#####################################################
# EN SHELL SCRIPT NO SE ACEPTAN NUMEROS DECIMALES
# EJEMPLO
#####################################################
#    RESULTADO=$( echo "6.423>0.5" | bc )
#    if [ $RESULTADO -eq 0 ]; then
#        echo "Es Mayor"
#    else
#        echo "Es menor o igual"
#    fi

#resultado=$( echo "$SENSOR2>20" | bc )

########################################################
# EMITIR UN SONIDO CON BUZZER, COMPROBAR TEMP HAB. DANIEL
########################################################
#if [ $resultado -eq 0 ];
#then `sudo python /home/pi/buzzer_single.py`
#########################################################

#echo -e "\e[1;31mALARMA AZUL\e[0m"
#echo $SENSOR2
#echo
#else
#echo "AZUL"
#echo -e "\e[1;32mSIN ALARMA\e[0m"
#echo
#fi

echo -e "Asi se escribe \e[1;34mG\e[0m\e[1;31mo\e[0m\e[1;33mo\e[0m\e[1;34mg\e[0m\e[1;32ml\e[0m\e[1;31me\e[0m"

##########################################################################
##########################################################################################################
# A short explanation of the example: first we check if the file somefile is readable (.if [ -r somefile ].).
# If so, we read it into a variable. If not, we check if it actually exists (.elif [ -f somefile ].).
# If that.s true, we report that it exists but isn.t readable (if it was, we would have read the content).
# If the file doesn.t exist, we report so, too. The condition at elif is only executed if the condition at
# if was false. The commands belonging to else are only executed if both conditions are false.
##########################################################################################################
#if [ -r somefile ]; then
#content=$(cat somefile)
#elif [ -f somefile ]; then
#echo .The file .somefile. exists but is not readable to the script..
#else
#echo .The file .somefile. does not exist..
#fi

##########################################################################################################
#count=0
#while read BUFFER
#do
#    count=$((++count))
#    echo "$count $BUFFER" >> lectura_variables.txt
#done < variables.txt

#if (( $SENSOR2 > 27 && ac == 0 ));then
#	if (( n == 5 ));then
#   		ac=1
#   		n=0
# 	else
#		n=n+1
#	fi

#elif (( $SENSOR2 < 27 && ac == 1 ));then
#	if (( n == 5 ));then
#		n=0
#		ac=0
#	else
#		n=n+1
#fi

#########################################################################



mysql -D temperaturas -u root -pchaosium -e "INSERT INTO historia VALUES ('$dia','$hora',$SENSOR1,$SENSOR3,$SENSOR7,$SENSOR6,$SENSOR2,$SENSOR5,$SENSOR4,'0','0','0','0','0','0');"



########################################################################
# message is 1x80 line in 4 bit mode
# DISPLAY EN PANTALLA LCD
########################################################################

#LCD2="$MYLCD/LCD_H/message"

# Sensor values
MSG1="$SENSOR1"
MSG2="$SENSOR2"
MSG3="$SENSOR3"
MSG4="$SENSOR4"
MSG5="$SENSOR5"
MSG6="$SENSOR6"
MSG7="$SENSOR7"
MSG8="$SENSOR8"

# Sensor names can be whatever you like
LABEL1="COME"
LABEL2="AZUL"
LABEL3="RECI"
LABEL4="COCI"
LABEL5="MATR"
LABEL6="NARA"
LABEL7="TERR"
LABEL8="RECI"

# Initialise the display
#printf "\x" > $LINEA1
#sleep 1
#printf "\x" > $LINEA2
#sleep 1
#printf "1" > $MYLCD/LCDon
#owwrite -s 192.168.1.2 FF.7F0800000100/LCDon 0
#owwrite -s 192.168.1.2 FF.7F0800000100/LCDon 1

owwrite "$MYLCD"/LCDon 0
owwrite "$MYLCD"/LCDon 1
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
	owwrite "$MYLCD"/line20.0 "Memoria="" ""$mem"
	owwrite "$MYLCD"/line20.1 "$fecha"
	owwrite "$MYLCD"/line20.2 "SERVICIOS --> $printservicios"
	#owwrite "$MYLCD"/line20.3 "                    "
	sleep 2
	#owwrite /home/pi/1wire/"$MYLCD"/LCDon 0
	#owwrite /home/pi/1wire/"$MYLCD"/LCDon 1

	#printf "$LABEL1"":""$MSG1"" ""$LABEL2"":""$MSG2" > $LINEA1
	owwrite "$MYLCD"/line20.0 "$LABEL1"":""$MSG1"" ""$LABEL2"":""$MSG2" 
	owwrite "$MYLCD"/line20.1 "$LABEL3"":""$MSG3"" ""$LABEL4"":""$MSG4"
	owwrite "$MYLCD"/line20.2 "$LABEL5"":""$MSG5"" ""$LABEL6"":""$MSG6"
	owwrite "$MYLCD"/line20.3 "$LABEL8"":""$MSG8"" ""$LABEL7"":""$MSG7"

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

