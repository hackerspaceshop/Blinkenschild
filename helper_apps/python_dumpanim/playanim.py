import time
import serial
import sys 

ser = serial.Serial(sys.argv[1], 115200, timeout=1)

anim = sys.argv[2] #"/media/usbstick/anims/testanim.dmp"

with open(anim, "rb") as f:
    frame = f.read((960*3)+1)
    while frame:
        frame = f.read((960*3)+1)
        ser.write(frame);
	time.sleep(0.04)

ser.close()
