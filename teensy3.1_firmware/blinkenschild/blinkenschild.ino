#include <SD.h>
#include <OctoWS2811.h>
#include "pixeltable.h"
#include "font_small.h"

File sdfile;

// Return values from the serial line
const int SERIAL_OK = 0; // All fine, no need to do anything
const int SERIAL_RELOAD_ANIMATION = 1; // Set an animation. Load new file from sd card

// are we looping trough all animations? or are we playing one specific selected animation?
int loop_animations = 1;

//which anim is playing?
int current_animation_index = 0;

// names of the animation file
String bluestring = ""; // bluetooth receive
char animation[13] = "0"; // used for SD open

// is a fixed animation playing? choosen over BT ?
int fixed_anim_playing = 0;

// which anim are we currently playingd
int animloopcount = 0;

const int cols = 40;
const int rows = 24;

const int total_leds = 960;

// OctoWS281  things..
const int ledsPerStrip = 128; // thats leds per pin.

DMAMEM int displayMemory[ledsPerStrip * 6];

int drawingMemory[ledsPerStrip * 6];
const int config = WS2811_RGB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

char animation_list[100][13] = { };
int animcounter = 0;

int inchar; // current char read
String cmd; // current command over bluetooth
String payload; // current payload of command

String textoverlay1 = "";
String textoverlay2 = "";
String textoverlay3 = "";

// global for font color
int text_r = 0;
int text_g = 0;
int text_b = 0;

//current color (0-255) / dimmer = brightness
int animdimmer = 1;
int textdimmer = 1;

int line1_ypadding = 0;
int line3_ypadding = 16;

void setup() {
    // safe is safe..
    delay(2000);

    // Output on USB serial
    Serial.begin(9600);

    // POctoWS2811 init()
    leds.begin();
    leds.show();

    // SD start
    pinMode(10, OUTPUT);
    if (!SD.begin(10)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");

    // Bluetooth serial init()
    Serial1.begin(9600);
    Serial.println("Bluetooth UART started successfully!");

    generate_anim_list();
}

void generate_anim_list() {
    Serial.println("Opening SD card to generate list");
    sdfile = SD.open("/");

    while (true) {

        File entry = sdfile.openNextFile();
        if (!entry) {
            // no more files
            Serial.print("Successfully added ");
            Serial.print(animcounter);
            Serial.println(" .TEK files to list");

            sdfile.close();

            break;
        }

        String filename = entry.name();

        if (filename.endsWith(".TEK")) {
            // add entry to animation list
            for (int i = 0; i < (filename.length()); i++) {
                animation_list[animcounter][i] = filename[i];
            }

            Serial.print("ok: ");
            Serial.println(filename);
            animcounter += 1;
        }

        entry.close();

    }
    return;

}

int handle_serial_data() {
    // handle data overbluetooth
    while (Serial1.available()) {
        inchar = Serial1.read();

        Serial.print("0x");
        Serial.print(inchar, HEX);
        Serial.print(" ");

        //  If line ends (with CR LF), interpret command
        if ((inchar == 0xD) || (inchar == 0xA)) {
            Serial.print("got a line: ");
            Serial.println(bluestring);

            cmd = bluestring.substring(0, bluestring.indexOf(":"));
            payload = bluestring.substring(bluestring.indexOf(":") + 1);
            bluestring = bluestring.substring(cmd.length() + payload.length() + 2);

            Serial.print("instruction: ");
            Serial.println(cmd);
            Serial.print("payload: ");
            Serial.println(payload);

            if (cmd == "+list") {
                Serial.println("Listing files on card");

                for (int i = 0; i < animcounter; i++) {
                    Serial1.print("+list:");
                    Serial1.println(animation_list[i]);
                }

                Serial1.println("-list");
            }

            if (cmd == "+text") {
                Serial.println("we got text");
                // TODO is there a special char (;) in the text? then we would linewrap  an set multiple text values..

                line1_ypadding = 0;
                line3_ypadding = 16;

                if (payload.indexOf(",") != -1) {
                    textoverlay1 = payload.substring(0,
                            payload.indexOf(","));

                    if (payload.indexOf(",", (payload.indexOf(",") + 1))
                            != -1) {

                        // 3 lines

                        textoverlay2 = payload.substring(
                                payload.indexOf(",") + 1,
                                payload.indexOf(",",
                                        (payload.indexOf(",") + 1)));
                        textoverlay3 = payload.substring(
                                payload.lastIndexOf(",") + 1);
                    } else {

                        // two lines

                        line1_ypadding = 4;
                        line3_ypadding = 13;

                        textoverlay3 = payload.substring(
                                payload.indexOf(",") + 1);
                        textoverlay2 = "";

                    }
                } else {

                    // one line in the middle
                    textoverlay2 = payload;

                    textoverlay1 = "";
                    textoverlay3 = "";
                }

            } // text

            if (cmd == "+text-brightness") {
                Serial.println("text-brightness");
                Serial.println(payload);
                textdimmer = payload.toInt();

            }

            if (cmd == "+anim-brightness") {
                Serial.println("anim-brightness");
                Serial.println(payload);
                animdimmer = payload.toInt();

            }

            if (cmd == "+text-color") {
                Serial.println("text-color");
                Serial.println(payload);
                text_g = payload.substring(0, payload.indexOf(",")).toInt();

                text_r = payload.substring(payload.indexOf(",") + 1,
                        payload.lastIndexOf(",")).toInt();

                text_b =
                        payload.substring(payload.lastIndexOf(",") + 1).toInt();

                Serial.print("R: ");
                Serial.print(text_r);
                Serial.print(" G: ");
                Serial.print(text_g);
                Serial.print(" B: ");
                Serial.println(text_b);
            }

            if (cmd == "+anim") {
                // only play that one specific animation
                loop_animations = 0;
                //current_animation_index=0;

                Serial.println("animation!");

                for (int i = 0; i < 13; i++)
                    animation[i] = 0;

                for (int idx = 0; idx < payload.length(); idx++) {
                    animation[idx] = payload[idx]; //payload;
                }

                Serial.print("Animation set to: ");
                Serial.println(animation);

                Serial1.println("-anim");
                //Serial1.println(animation);
                //Serial1.println("- DONE");

                fixed_anim_playing = 1;
                return SERIAL_RELOAD_ANIMATION;
            }

        } else {
            // middle of line: append character to buffer
            bluestring += (char) inchar;
        }
    } // end while Serial1.available
    return SERIAL_OK;
}

void loop() {
    if (Serial1.available()) {
        handle_serial_data();

    } // end if Serial1 available

    if (animation != 0) {
        sdfile = SD.open(animation); // hier muss i hin

        if (sdfile) {
            Serial.print("Opened file : ");
            Serial.println(animation);

            // read from the file until there's nothing else in it:
            while (sdfile.available()) {
                // Quit playing the animation if serial data is incoming
                if (Serial1.available()) {
                    if (handle_serial_data() == SERIAL_RELOAD_ANIMATION) {
                        sdfile.close();
                        return;
                    }
                }

                // find start of frame
                int c_b = sdfile.read();

                if (c_b == 1) {
                    for (int j = 0; j < total_leds; j++) {
                        int r, g, b = 0;

                        r = sdfile.read();
                        r = r / animdimmer;
                        if (r < 0)
                            r = 0;

                        g = sdfile.read();
                        g = g / animdimmer;
                        if (g < 0)
                            g = 0;

                        b = sdfile.read();
                        b = b / animdimmer;
                        if (b < 0)
                            b = 0;

                        //Serial.println(j);

                        leds.setPixel(pxl_table[j], r, g, b);
                    }

                    if (textoverlay1 != "") {
                        drawText(textoverlay1, line1_ypadding);
                    }

                    if (textoverlay2 != "") {
                        drawText(textoverlay2, 8);
                    }

                    if (textoverlay3 != "") {
                        drawText(textoverlay3, line3_ypadding);
                    }

                    leds.show();
                    delay(10);

                } // c_b == 1  ???

            }
            // close the file:
            sdfile.close();

            Serial.print("Closed file : ");
            Serial.println(animation);

        } // if animation != ""

    } else {
        // if the file didn't open, print an error:
        Serial.print("error opening file:");
        Serial.println(animation);
        delay(1000);
    }

    return;
} // loop() close


// for list of files over bluetooth
void printDirectory(File dir) {
    while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            //Serial.println("**nomorefiles**");
            break;
        }
        Serial1.println(entry.name());
        entry.close();
    }
}

/*  blinkenschild functions */

// setXY sets a pixel at position x/y to the values (r, g, b)
// accepts   x = 1 .. max(cols)
// accepts   y = 1 .. max(rows)
//  TODO same funtion with 3 parameters as rgb (uint32_t)
void setXY(int x, int y, int r, int g, int b) {
    // the shield starts BOTTOM RIGHT not top left!

    // this would be ok for top left
    //int pos =  ((x-1) * rows) + (y-1);

    // get row
    int pos = (rows * cols) - ((y - 1) * cols) - x;

    // set value
    leds.setPixel(pxl_table[pos], r, g, b);
}

// Reset all pixels to 0
void erase() {
    for (int i = 0; i < total_leds; i++)
        leds.setPixel(i, 0);
}

void drawText(String displaytext, int offset_y) {
    int offset_x = 0;

    displaytext.toUpperCase();

    int stringwidth = 0;

    // find total stringlength
    for (int j = 0; j < displaytext.length(); j++) {

        int charindex = 26; // SPACE is default
        int charnum = (int) displaytext[j];

        if (charnum >= 65 && charnum <= 90) {
            charindex = charnum - 65;
        }

        int charwidth = font_small_lookup[charindex];
        stringwidth = stringwidth + charwidth + 1;
        offset_x = (40 - stringwidth) / 2;

    }

    stringwidth = 0;

    //  render the text
    for (int j = 0; j < displaytext.length(); j++) {

        int charindex = 26; // SPACE is default
        int charnum = (int) displaytext[j];

        if (charnum >= 65 && charnum <= 90) {
            charindex = charnum - 65;
        }

        int charwidth = font_small_lookup[charindex];

        int r = text_r / textdimmer;
        int g = text_g / textdimmer;
        int b = text_b / textdimmer;
        if (r < 0)
            r = 0;
        if (g < 0)
            g = 0;
        if (b < 0)
            b = 0;

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < charwidth; x++) {
                if (font_small[charindex][(y * charwidth) + x]) {
                    setXY(x + 1 + offset_x + stringwidth, y + 1 + offset_y, r,
                            g, b);
                }
            }
        }

        stringwidth = stringwidth + charwidth + 1;
    }

    return;

}

