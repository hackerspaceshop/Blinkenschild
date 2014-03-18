import time
import serial




# this script READS a dump file and sends it out over serial 

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

anim = "testanim.tek"

with open(anim, "rb") as f:
    frame = f.read(1728)
    while frame:
        frame = f.read(1728)
        ser.write(frame);
	time.sleep(0.04)

ser.close()
