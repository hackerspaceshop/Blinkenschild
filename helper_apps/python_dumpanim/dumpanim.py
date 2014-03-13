#!/usr/bin/env python

import serial
import threading
import time
import sys

class SerialReceiver(threading.Thread):
    """ This class has been written by
        Philipp Klaus and can be found on
        https://gist.github.com/4039175 .  """
    def __init__(self, device, *args):
        self._target = self.read
        self._args = args
        self.__lock = threading.Lock()
        self.ser = serial.Serial(device,sys.argv[3])
        self.data_buffer = ""
        self.closing = False # A flag to indicate thread shutdown
        self.sleeptime = 0.000000005
        threading.Thread.__init__(self)

    def run(self):
        self._target(*self._args)

    def read(self):
        while not self.closing:
            time.sleep(self.sleeptime)
            if not self.__lock.acquire(False):
                continue
            try:
                self.data_buffer += self.ser.read(6)
            finally:
                self.__lock.release()
        self.ser.close()

    def pop_buffer(self):
        # If a request is pending, we don't access the buffer
        if not self.__lock.acquire(False):
            return ""
        buf = self.data_buffer
        self.data_buffer = ""
        self.__lock.release()
        return buf

    def write(data):
        self.ser.write(data)

    def close(self):
        self.closing = True


if __name__ == "__main__":
    device =sys.argv[2] #'/dev/tty.NoZAP-PL2303-00002014' 

    s1 = SerialReceiver(device)
    s1.start()
    f = open(sys.argv[1], "wb")

    try:
        while True:
            data = s1.pop_buffer()
            f.write(data)

#            if data != "": print repr(data)
    except KeyboardInterrupt:
        s1.close()
	f.close()
    finally:
        s1.close()
    s1.join()
    f.close()
