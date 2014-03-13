/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SD.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
const int chipSelect = 10;

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}


int done=0;

void loop()
{
  
  
  if(done) return;
  
  
  File dataFile = SD.open("1.tek", FILE_WRITE);





  // if the file is available, write to it:
  if (dataFile) {

  for(int i=0;i<(((960*3)+1)*5);i++)
  {
    dataFile.write(SerialReadBlocking());
  }
  
  
    dataFile.close();
    Serial.println("wrote data");
    digitalWrite(13,LOW);
    delay(1000);
    
    
    
    done=1;
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening 1.tek");
  } 
}






// hacky hack implements blocking read() from Uart.
int SerialReadBlocking() {
 // while (!Serial.available()) {}
 // return Serial.read(); // one byte


  while (!Serial.available()) {}
  return Serial.read(); // one byte

 
 
 
}






