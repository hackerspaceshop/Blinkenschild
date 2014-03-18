#!/bin/bash

# this script creates a viryual serial device to dump data form glediator into a file 



port=/dev/pts/2


dumpdir=./dumps





if [ "$#" -eq 0 ];then
	echo "dump.sh <ANIMNAME>"
	else
	file=$dumpdir/$1.tek

	echo -n "Dumping to: "
	echo $file 

	touch $file


        sudo ln -s $port /dev/ttyACM0 
        sudo chmod 777 /dev/ttyACM0
        echo "OUTPUT SHOULD GO TO /dev/ttyACM0"
	echo "run java as root!!!!! java -d32 -jar Glediator_V2.jar"
	echo "OUTPUT setting: 115200, order HL_BR Colormixing GRB"


        sudo socat -d -d  pty,raw,echo=0 - > $file





fi

