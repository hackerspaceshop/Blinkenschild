#!/usr/bin/python
"""
# Requirements:
# sudo aptitude install python-bluetooth

# Information Sources: 
# http://code.google.com/p/pybluez/source/browse/trunk/examples/simple/rfc...
# http://people.csail.mit.edu/albert/bluez-intro/x290.html#py-rfcomm-serve...

How to run Bluetooth Chat between Android and PC
------------------------------------------------

$ apt-get install libbluetooth-dev
$ pip install pybluez

* Pair computer and Android phone
* Find Android hw addr.:

	$ hcitool scan

* Create a serial channel on the PC: 

	$ sudo sdptool add --channel=3 SP

* Bind Android BT hw addr. to rfcomm0 on our SPP channel: 

	$ sudo rfcomm bind rfcomm0 B0:EC:71:CF:5B:97 3

* Start python bluetooth chat SPP server:

	$ python src/python-spp-server/bluetooth_chat.py

"""
import bluetooth
import threading

name = "BluetoothChat"
#uuid = "fa87c0d0-afac-11de-8a39-0800200c9a66"
uuid = "00001101-0000-1000-8000-00805F9B34FB"

server_sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
#server_sock.bind(("", bluetooth.PORT_ANY))
server_sock.bind(("", 3))
server_sock.listen(1)
port = server_sock.getsockname()[1]

bluetooth.advertise_service( server_sock, name, uuid )

print "Waiting for connection on RFCOMM channel %d" % port

class echoThread(threading.Thread):
    def __init__ (self,sock,client_info):
        threading.Thread.__init__(self)
        self.sock = sock
        self.client_info = client_info
    def run(self):
        try:
            while True:
                data = self.sock.recv(1024)
                if len(data) == 0: break
                print self.client_info, ": received [%s]" % data
                self.sock.send(data)
                print self.client_info, ": sent [%s]" % data
        except IOError:
            pass
        self.sock.close()
        print self.client_info, ": disconnected"

while True:
    client_sock, client_info = server_sock.accept()
    print client_info, ": connection accepted"
    echo = echoThread(client_sock, client_info)
    echo.setDaemon(True)
    echo.start()

server_sock.close()
print "all done"
