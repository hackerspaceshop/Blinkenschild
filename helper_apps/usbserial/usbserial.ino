


unsigned long baud = 9600;
HardwareSerial Uart = HardwareSerial();


const int cmd_mode_pin = 3;
const int led_pin = 13;  // 11=Teensy 2.0, 6=Teensy 1.0, 16=Benito
const int led_on = HIGH;
const int led_off = LOW;

void setup()
{
	pinMode(led_pin, OUTPUT);
	digitalWrite(led_pin, led_off);
//	pinMode(cmd_mode_pin, OUTPUT);
//        digitalWrite(cmd_mode_pin, HIGH);
        

        
        
	Serial.begin(baud);	// USB, communication to PC or Mac
	Uart.begin(baud);	// UART, communication to Dorkboard





Serial.println("start..");
//        delay(2000);      
//        digitalWrite(cmd_mode_pin, LOW);
Serial.println("Interesting ocmmands include: \nAT+UART=9600,0,0\nAT+PSWD=<PIN>\nAT+");   
  
        
        
        
}

long led_on_time=0;

void loop()
{
	unsigned char c, dtr;
	static unsigned char prev_dtr = 0;

	if (Serial.available()) {
		c = Serial.read();
		Uart.write(c);
		digitalWrite(led_pin, led_on);
		led_on_time = millis();
		return;
	}
	if (Uart.available()) {
		c = Uart.read();
		Serial.write(c);
		digitalWrite(led_pin, led_on);
		led_on_time = millis();
		return;
	}
	
	if (Serial.baud() != baud) {
		baud = Serial.baud();
		if (baud == 57600) {
			// This ugly hack is necessary for talking
			// to the arduino bootloader, which actually
			// communicates at 58824 baud (+2.1% error).
			// Teensyduino will configure the UART for
			// the closest baud rate, which is 57143
			// baud (-0.8% error).  Serial communication
			// can tolerate about 2.5% error, so the
			// combined error is too large.  Simply
			// setting the baud rate to the same as
			// arduino's actual baud rate works.
			Uart.begin(58824);
		} else {
			Uart.begin(baud);
		}
	}
}

