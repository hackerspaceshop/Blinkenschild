
#include <SD.h>
#include <OctoWS2811.h>
#include "pixeltable.h"
#include "font_small.h"

File sdfile;





// are we looping trough all animations? or are we playing one specific selected animation?
int loop_animations =1;

//which anim is playing?
int current_animation_index=0;


// names of the animation file
String bluestring= "";   // bluetooth receive
char animation[13]="0";   // used for SD open


// is a fixed animation playing? choosen over BT ?
int fixed_anim_playing =0;

// which anim are we currently playingd
int animloopcount =0;


const int cols = 40;
const int rows = 24;



const int total_leds=960;

// OctoWS281  things..
const int ledsPerStrip = 128; // thats leds per pin.

DMAMEM int displayMemory[ledsPerStrip*6];

int drawingMemory[ledsPerStrip*6];
const int config = WS2811_RGB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);







char animation_list[100][13]={};
int animcounter=0;

void setup() {
  
  
  
  // safe is safe..
 delay(2000);
 
 // bluetooth module CMD pin 
 //cmd pin
 // pinMode(3, OUTPUT);
 // digitalWrite(3, LOW);
  


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






void generate_anim_list()
{
    
  
  Serial.println("Opening SD card to generate list");
  sdfile = SD.open("/");
  
  while(true) {
     
     File entry =  sdfile.openNextFile();
     if (! entry) {
       // no more files
       Serial.print("Successfully added ");
       Serial.print(animcounter);
       Serial.println(" .TEK files to list");   
   
   
       sdfile.close();
       
       break;
     }
     
     
     String filename =  entry.name();

     if(filename.endsWith(".TEK")) 
     {
        // add entry to animation list
        for(int i=0;i<(filename.length());i++){  animation_list[animcounter][i]=filename[i]; }
        
        Serial.print("ok: ");
        Serial.println(filename);        
        animcounter+=1;   
     }
     
     
     entry.close();
     
   }
return;

}








int delim = 0xD; // newline
int inchar; // current char read
String cmd; // current command over bluetooth
String payload; // current payload of command


String textoverlay1 ="";
String textoverlay2 ="";
String textoverlay3 ="";

// global for font color
int text_r=0;
int text_g=0;
int text_b=0;




//current color (0-255) / dimmer = brightness
int animdimmer=1;
int textdimmer=1;


void loop() {
/*
  
  String hurra="wtf";
  
  
  
  drawText(hurra,0,0);
  drawText(hurra,0,8);
  drawText(hurra,0,16);
  
  return;
  
 */ 
  
 
  
 // TESTCODE for serial
  /*
// 0x01  head of frame..
 while (SerialReadBlocking() != 1) {} // head of frame ...
   
   

   for (int i=0; i < total_leds; i++) {

     
   
     int r,g,b =0;

     r = SerialReadBlocking();
     g = SerialReadBlocking();
     b = SerialReadBlocking();
     
     
       leds.setPixel( pxl_table[i],r,g,b);
  


   }

  leds.show();  
         

 return;


*/
  
  

  
  
// TODO put bluetooth communication in separate tab
/*
bluetooth com looks like this:


c:list = get list of files
a:<animname.tek> = choose animation <name>
t:text
*/






if(Serial1.available())
{
  
  
  /*
    for (int j=0; j < total_leds; j++) { 

           leds.setPixel( pxl_table[j],random(0,255),0,0);

    }  
             leds.show();
  */

  
  
  
  
  // handle data overbluetooth
 while(Serial1.available())
 {
  inchar=  Serial1.read(); 

  Serial.print("0x");;  
  Serial.print(inchar,HEX);
  Serial.print(" ");   
  
  
  
  //  CR LF, ...
  if((inchar == 0xD) || (inchar == 0xA) )
  {
    Serial.print("got a line: ");
    Serial.println(bluestring);
    
    
    
    
    cmd=bluestring.substring(0,bluestring.indexOf(":"));
    payload=bluestring.substring(bluestring.indexOf(":")+1);
    bluestring=bluestring.substring(cmd.length() + payload.length() + 2);
    
  

    Serial.print("instruction: ");
    Serial.println(cmd);
    

   // Serial.print("payload: ");
   // Serial.println(payload);



    if(cmd == "+list")  // c (command)
    {
      Serial.println("command");
      Serial.println(payload); 
      
      Serial.println("Listing files on card"); 

       for(int i=0;i<animcounter;i++)
       {
         Serial1.print("+list:");
         Serial1.println(animation_list[i]);
       }


       Serial1.println("-list"); 
    }


    
    if(cmd == "+text")  // t
    {
      Serial.println("we got text");
      Serial.println(payload);      
     // TODO is there a special char (;) in the text? then we would linewrap  an set multiple text values..
     
     
     if(payload.indexOf(",") != -1)
     {
       textoverlay1=payload.substring(0,payload.indexOf(","));
   
   
   
   
   
   
       if(payload.indexOf(",", (payload.indexOf(",")+1)) != -1) 
       {
         
         // 3 lines 
         
         textoverlay2=payload.substring(payload.indexOf(",")+1, payload.indexOf(",", (payload.indexOf(",")+1)) );
         textoverlay3=payload.substring(payload.lastIndexOf(",")+1);
       }  
       else
       {
         
         // two lines
         
         textoverlay3=payload.substring(payload.indexOf(",")+1);
         textoverlay2="";
         
         
       }
     }
     else
     {
       
       
       
       // one line in the middle
      textoverlay2=payload;
      
      textoverlay1="";
      textoverlay3="";
     }
  
    }  // text
    
    


    
    if(cmd == "+text-brightness")  // t
    {
      Serial.println("text-brightness");
      Serial.println(payload);      
      textdimmer=payload.toInt();   
    
      
   
  
      
      
      
    }


    
    if(cmd == "+anim-brightness")  // t
    {
      Serial.println("anim-brightness");
      Serial.println(payload);      
      animdimmer=payload.toInt();   
    
      
    }



    
    
    
    if(cmd == "+text-color")  // t
    {
      Serial.println("text-color");
      Serial.println(payload);      
     text_g=payload.substring(0,payload.indexOf(",")).toInt();   
   
   
   
     
     text_r=payload.substring(payload.indexOf(",")+1,payload.lastIndexOf(",")).toInt(); 
  
  
     
     text_b=payload.substring(payload.lastIndexOf(",")+1).toInt();         
     
     
     
     Serial.print("R: ");
     Serial.print(text_r); 
      Serial.print(" G: ");
     Serial.print(text_g); 
      Serial.print(" B: ");
     Serial.println(text_b);     
    }
    
    
    
  
            
    if(cmd == "+anim") // "a"
    {
      
      
      // only play that one specific animation
      loop_animations=0;
      //current_animation_index=0;
      
      
      Serial.println("animation!");
      Serial.println(payload);     
     
      for(int i=0;i<13;i++) animation[i]=0;
      
   
      for(int idx=0;idx<payload.length();idx++)
      {
       animation[idx]=payload[idx];//payload;
      }

      Serial.print("Animation set to: ");
      Serial.println(animation);
      
      
      
      Serial1.println("-anim");
      //Serial1.println(animation);
      //Serial1.println("- DONE");     
      
      
      fixed_anim_playing=1;
      
      
      
    }        
            
            
    
    
    //bluestring="";
  
  } // end of line
  else // middle of line append
   bluestring += (char)  inchar;

   
 } // end while Serial1.available
  
} // end if Serial1 available 
  







//Serial.println("da: ");
//Serial.println(animcounter);

 //delay(1000);

    // re-open the file for reading:
    

/*
   if(!fixed_anim_playing)
   {
        
     
     
   
     
    // play one after the other.
   for(int i=0;i<13;i++) animation[i]=0;      

   
   

    for(int idx=0;idx<sizeof(animation_list[idx]);idx++)
    {
       animation[idx]=animation_list[animloopcount][idx];//payload;
    }

 
 
 
 
 
 
   // set anim loop pointer to next entry
   animloopcount++;
   if(animloopcount>animcounter) animloopcount=0;
  }  
      
    */  
  if(animation != 0)
  {
    
  sdfile = SD.open(animation); // hier muss i hin
  
  
  
  if (sdfile) {
    Serial.print("Opened file : ");
    Serial.println(animation);
    
    
    // read from the file until there's nothing else in it:
    while (sdfile.available()) {
         
      if(Serial1.available()) {
        sdfile.close();
        return;
      }
      
      
      
      // find start of frame
      int c_b=0;
      c_b= sdfile.read();
  
  
  
      if(c_b == 1) {    
        
        
         for (int j=0; j < total_leds; j++) { 
           int r,g,b =0;
           
           
           
           
           r = sdfile.read();
           r=r/animdimmer;
           if(r<0) r=0;

           g = sdfile.read();
           g=g/animdimmer;     
           if(g<0) g=0;      
           
           b = sdfile.read();
           b=b/animdimmer;
           if(b<0) b=0;
           
           
           //Serial.println(j);
           
        
           leds.setPixel( pxl_table[j],r,g,b);
         }



if(textoverlay1 !="" ) 
{
 drawText(textoverlay1,0);  
}


if(textoverlay2 !="" ) 
{
 drawText(textoverlay2,8);  
}



if(textoverlay3 !="" ) 
{
 drawText(textoverlay3,16);  
}





       leds.show();  
    delay(10);
   
   
       
      }  // c_b == 1  ???   
      
      
      
  
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

 
  
  
  
  
  
  
  
  /*
  
    for (int i=0; i < total_leds; i++) { 
      leds.setPixel(pxl_table[i],   0x0000ff);
      leds.show();

    }
     
     
       
    for (int i=0; i < total_leds; i++) { 
      leds.setPixel(pxl_table[i],   0);

    }
         
*/
  
  
  
  
  
  
  
  
  
  


} // loop() close














// hacky hack implements blocking read() from Uart.
int SerialReadBlocking() {
 // while (!Serial.available()) {}
 // return Serial.read(); // one byte


 
  while (!Serial.available()) {}
  return Serial.read(); // one byte
 
}






// for list of files over bluetooth

void printDirectory(File dir) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     Serial1.println(entry.name());
     entry.close();
   }
}











/*  blinkenschild functions */

//  TODO same funtion with 3 parameters as rgb (uint32_t)
// accepts   x = 1 .. max(cols)
// accepts   y = 1 .. max(rows)
void setXY(int x,int y, int r, int g, int b)
{

 // the shield starts BOTTOM RIGHT not top left!
  
 // this would be ok for top left 
 //int pos =  ((x-1) * rows) + (y-1); 
 
 
  // get row
 int pos =  (rows * cols ) - ((y-1) * cols) -x;
 

 
  // set value 
 leds.setPixel( pxl_table[pos],r,g,b);
  
}

void erase()
{
  for(int i=0;i<total_leds;i++)
    leds.setPixel(i,0);
}








void drawText(String displaytext, int offset_y)
{
 
  
  int offset_x=0;
  
 // string testcode
// displaytext="abc xx"; 
  
displaytext.toUpperCase();
  
 
  
  
//int offset_x=0;  
//int offset_y=0;    

int stringwidth=0;
  
  
// find total stringlength  
for(int j=0;j<displaytext.length();j++)
{

  int charindex = 26;  // SPACE is default
  int charnum = (int) displaytext[j];
  
  if(charnum >=65  && charnum <=90)
  {
     charindex = charnum-65;  
  }
  

  int charwidth = font_small_lookup[charindex];
  stringwidth = stringwidth + charwidth+1;
  offset_x =(40-stringwidth)/2;
   
  
}  

    
    stringwidth =0;
    
    
    //  render tht text
    
 for(int j=0;j<displaytext.length();j++)
{

  int charindex = 26;  // SPACE is default
  int charnum = (int) displaytext[j];
  
  if(charnum >=65  && charnum <=90)
  {
     charindex = charnum-65;  
  }
  

  int charwidth = font_small_lookup[charindex];

       
    
    
  
 // Serial.print(displaytext[j]);  
 // Serial.print(" ");
//  Serial.print(charnum);  
//  Serial.print(" ");      
//  Serial.println(charindex);  
  
  
  

  
  //  erase();
  //setXY testcode




       
  int r = text_r/textdimmer;
  int g = text_g/textdimmer;
  int b = text_b/textdimmer;
  if(r<0) r=0;   
  if(g<0) g=0;   
  if(b<0) b=0;   




  for(int y=0; y< 8 ; y++)
  {
   for(int x=0; x<charwidth; x++)
   { 
    if(font_small[charindex][(y*charwidth)+x]) 
    {      
       setXY(x+1+offset_x+stringwidth,y+1+offset_y,r,g,b);
    }
   }
  }  
  
  
  stringwidth = stringwidth + charwidth+1;
  //offset_x=offset_x+stringwidth; 
  
  
  // leds.show();

 




//delay(400);


}


  


  
  
  return;
  
  
   
  
}











