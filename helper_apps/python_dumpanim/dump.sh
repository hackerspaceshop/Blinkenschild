#!/bin/bash

# TODO: USBhub, USBstick, dump there

#dumpdir=/home/ubuntu/hackerspaceshop/animations/dumps
dumpdir=/media/usbstick/anims

if [ "$#" -eq 0 ];then
	echo "dump.sh <ANIMNAME>"
	else
	file=$dumpdir/$1.dmp

	echo -n "Dumping to: "
	echo $file 

	touch $file

	rm -f socat.out
        socat -d -d -lf socat.out pty,raw,echo=0 - > $file


things are broken here .. we can not launch socat in the backgound with & or it will return .. because it wants to READ from STDIO
needs to read doku :(


	sleep 1 
        pty="`cat socat.out | grep PTY | cut -d " " -f 7`"
        echo "PSEUDO SERIAL GENERATED SUCCESSFULLY" 

	echo -n "Starting: "
        echo -n "java -Dgnu.io.rxtx.SerialPorts="
        echo -n $pty
        echo -n " -jar /home/ubuntu/hackerspaceshop/glediator1.1/Glediator.jar"

        java -Dgnu.io.rxtx.SerialPorts=$pty -jar /home/ubuntu/hackerspaceshop/glediator1.1/Glediator.jar


fi

