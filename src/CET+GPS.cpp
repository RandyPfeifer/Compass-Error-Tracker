// Copyright (c) 2022, Randy Pfeifer
// All rights reserved.

/*
   Derived from review of: e-Gizmo QMC5883L GY-271 Compass

   Sample sketch for the GY-271 QMC5883L
   for getting the raw data of x, y, z and
   Radius in degrees.

   Codes by e-Gizmo Mechatronix Central
   http://www.e-gizmo.com
   July 10,2017

*/
/*********
  Also derived from review of:  Complete project details at https://randomnerdtutorials.com
  
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

// This program utilizes the NeoGPS library and utilizes some lines of code from a couple Examples included with that library...

//    NeoGPS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NeoGPS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <NMEAGPS.h>
#include <SoftwareSerial.h>
static const int RXPin = 9, TXPin = 8;
static const uint32_t GPSBaud = 9600;
// The serial connection to the GPS device
SoftwareSerial gpsPort(RXPin, TXPin);
// The serial connection to the GPS device

#define GPS_Heading_Hurdle 15


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define Cum_Error_Threshold 25 // start to display turn signals
#define ALARM_THRESHOLD 55 // increase font size and move error display to yellow line. 
#define HUGE_THRESHOLD 500 // display extra data when this cumulative error is reached. 
#define TURN_HEIGHT   38
#define TURN_WIDTH    38

// 'Go_Left', 38x38px
const unsigned char epd_bitmap_Go_Left [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0x8f, 0xff, 0xfc, 0xff, 
	0xff, 0x37, 0xff, 0xfc, 0xff, 0xfe, 0x7b, 0xff, 0xfc, 0xff, 0xfc, 0xfd, 0xff, 0xfc, 0xff, 0xf9, 
	0xfe, 0xff, 0xfc, 0xff, 0xf3, 0xff, 0x7f, 0xfc, 0xff, 0xe7, 0xff, 0xbf, 0xfc, 0xff, 0xcf, 0xff, 
	0xdf, 0xfc, 0xff, 0x9f, 0xff, 0xef, 0xfc, 0xff, 0x20, 0x07, 0xf7, 0xfc, 0xfe, 0x60, 0x07, 0xfb, 
	0xfc, 0xfc, 0xf0, 0x0f, 0xfd, 0xfc, 0xf9, 0xf0, 0x0f, 0xfe, 0xfc, 0xf3, 0xf0, 0x03, 0xff, 0x7c, 
	0xe7, 0xf8, 0x01, 0xff, 0xbc, 0xcf, 0xf8, 0x00, 0xff, 0xdc, 0xdf, 0xf8, 0x00, 0xff, 0xec, 0xdf, 
	0xfc, 0xe0, 0x7f, 0xec, 0xcf, 0xfc, 0xf0, 0x7f, 0xdc, 0xe7, 0xff, 0xf0, 0x7f, 0xbc, 0xfb, 0xff, 
	0xf0, 0x7f, 0x7c, 0xf9, 0xff, 0xf0, 0x7e, 0xfc, 0xfe, 0xff, 0xf0, 0x7d, 0xfc, 0xff, 0x7f, 0xf0, 
	0x7b, 0xfc, 0xff, 0x3f, 0xf0, 0x77, 0xfc, 0xff, 0xdf, 0xf0, 0x6f, 0xfc, 0xff, 0xef, 0xff, 0xdf, 
	0xfc, 0xff, 0xf7, 0xff, 0xbf, 0xfc, 0xff, 0xfb, 0xff, 0x7f, 0xfc, 0xff, 0xfd, 0xfe, 0xff, 0xfc, 
	0xff, 0xfe, 0xfd, 0xff, 0xfc, 0xff, 0xff, 0x7b, 0xff, 0xfc, 0xff, 0xff, 0xb7, 0xff, 0xfc, 0xff, 
	0xff, 0xcf, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc
};
// 'Go_Right', 38x38px
const unsigned char epd_bitmap_Go_Right [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0xff, 0xff, 0xb7, 0xff, 0xfc, 0xff, 
	0xff, 0x7b, 0xff, 0xfc, 0xff, 0xfe, 0xfd, 0xff, 0xfc, 0xff, 0xfd, 0xfe, 0xff, 0xfc, 0xff, 0xfb, 
	0xff, 0x7f, 0xfc, 0xff, 0xf7, 0xff, 0xbf, 0xfc, 0xff, 0xef, 0xff, 0xdf, 0xfc, 0xff, 0xdf, 0xff, 
	0xef, 0xfc, 0xff, 0xbf, 0x80, 0x17, 0xfc, 0xff, 0x7f, 0x00, 0x3b, 0xfc, 0xfe, 0xff, 0x80, 0x3d, 
	0xfc, 0xfd, 0xff, 0xc0, 0x3e, 0xfc, 0xfb, 0xff, 0x00, 0x7f, 0x7c, 0xf7, 0xfe, 0x00, 0x7f, 0xbc, 
	0xef, 0xfc, 0x00, 0x7f, 0xdc, 0xdf, 0xfc, 0x08, 0xff, 0xec, 0xdf, 0xf8, 0x18, 0xff, 0xec, 0xef, 
	0xf8, 0x3d, 0xff, 0xdc, 0xf7, 0xf8, 0x3f, 0xff, 0xbc, 0xfb, 0xf8, 0x3f, 0xff, 0x7c, 0xfd, 0xf8, 
	0x3f, 0xfe, 0xfc, 0xfe, 0xf8, 0x3f, 0xfd, 0xfc, 0xff, 0x78, 0x3f, 0xfb, 0xfc, 0xff, 0xb8, 0x3f, 
	0xf7, 0xfc, 0xff, 0xd8, 0x7f, 0xef, 0xfc, 0xff, 0xef, 0xff, 0xdf, 0xfc, 0xff, 0xf7, 0xff, 0xbf, 
	0xfc, 0xff, 0xfb, 0xff, 0x7f, 0xfc, 0xff, 0xfd, 0xfe, 0xff, 0xfc, 0xff, 0xfe, 0xfd, 0xff, 0xfc, 
	0xff, 0xff, 0x7b, 0xff, 0xfc, 0xff, 0xff, 0xb7, 0xff, 0xfc, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 416)
const int epd_bitmap_allArray_LEN = 2;
const unsigned char* epd_bitmap_allArray[2] = {
	epd_bitmap_Go_Left,
	epd_bitmap_Go_Right
};


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#include <QMC5883L.h>

QMC5883L compass;


NMEAGPS gps;
gps_fix fix;



//some global variables...
bool Start_flag = true;
bool BAT = false;
float TARGET=777;
float Error=77;
float Cumulative_Error = 0;
int GPS_Ready = 1;  // don't declare GPS mode until we have "GPS_Heading_Hurdle" valid sentences in a row. 
bool GPS_flag = false;  // Signal for GPS switch-over.
float GPS_Heading=0;
float REAL_TARGET=0;
bool REAL_TARGET_Flag=false;
float Measurement=0;
float New_Point_A_lat=0;
float New_Point_A_lon=0;
float Bearing_from_A_prime=0;
float Bearing_to_A_prime=0;
float Error_Angle=0;

void setup() {
 
  Wire.begin();     
  Serial.begin(9600);  
  gpsPort.begin(GPSBaud);
  compass.init();     
  //while(!Serial);
  
 
 //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();  // Display splash..

/* use the following to lower the brightness of the display - seems too dim for outdoor use though...
  display.ssd1306_command(0x81);  // point to contrast setting parameter
  display.ssd1306_command(0);     // set contrast to minimum value. 
  display.ssd1306_command(0xD9);   // point to pre-charge parameter
  display.ssd1306_command(0x11);   // set to minimum value for both phase 1 and phase 2
*/

delay(500); // Pause for .5 seconds to display splashscreen
 
  // Clear the display buffer
  display.clearDisplay();

  // set up Pin D0 as an input for a pushbutton. May be used someday.... Pulled down with 10K resistor.
  pinMode(0,INPUT);
    
  REAL_TARGET_Flag=false;

}

void loop() {

int Button = digitalRead(0);
  if (Start_flag || Button) {  //Sample target heading on first pass through the loop or anytime the button is pressed
  unsigned long time = millis();
    while (Button) {
      if (!BAT) Serial.println("BUTTON PUSHED");
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.setTextColor(WHITE,BLACK);
      display.println("BUTTON");
      display.println("PUSHED");
      if (millis() >= (time+3000)) { // long hold on button - initiate recording of heading for GPS later
        REAL_TARGET = read3();
        REAL_TARGET_Flag=true;
        display.println(REAL_TARGET);
      }
      display.display();      
      /* Don't reset flag if previously set - keep REAL_TARGET setting even if button is short-pushed later.  
      This will allow user to short-push button to initiate compass-based navigation until GPS is online.  
      Only a power-cycle will reset REAL_TARGET_Flag to flase. 
      
      else {
       // REAL_TARGET_Flag=false;   
      }
      */

      delay(200);
      Button = digitalRead(0);
    }

    if (!BAT) Serial.print("   Reading TARGET ");
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(WHITE,BLACK);
    display.println("ACQUIRING");
    display.println("TARGET");
    display.display();
    delay(1000);
    TARGET = (read3() + read3() + read3()) / 3;  // read 9 samples and average them.   NOTE!: this code doesn't account for the 0/360 boundary properly

    Cumulative_Error=0; 

    // Signal on the display that we have a target to use.     
    display.clearDisplay();
    display.setCursor(0,20);
    display.println("TARGET");
    display.println("ACQUIRED!");
    display.display();
    Start_flag = false;  // don't do the above again unless reset button is pushed. 
    GPS_flag = false;  // start with Compass navigation (reset to false if button is pushed)
    GPS_Ready = 1;
    delay(600);  // get ready for cycles of measurements to assess error from target course.
  }

if(!GPS_flag) { // navigate using the compass
  // read 3 compass samples averaged to get current heading.
  Measurement = read3();

  Error = Measurement - TARGET;
   // look for 360 degree wrap around and correct if ncessary  before accumulating error.
  if (Error > 180)
        Error -= 360.0;
  if (Error < -180)
        Error += 360.0;

  Cumulative_Error += Error;  // accumulate overall error

}
else { // GPS_flag must be true.  Navigate using the GPS...
// still grab compass direction and update Measurement
Measurement = read3(); //for no particular reason other than debug printing and consistent loop timing...

/* calculate  the total +/- deviation from the intended course - assign that to Cumulative_Error
this calculation should be based on distance from Point_A and the angle deviation from TARGET
*/
// The base location (A_prime), in degrees * 10,000,000???
NeoGPS::Location_t A_prime( New_Point_A_lat, New_Point_A_lon ); // the new Point_A
float D = fix.location.DistanceMiles( A_prime )* 5280; //Distance to Point_A in feet.
Bearing_to_A_prime = fix.location.BearingToDegrees(A_prime);

// calclate reverse bearing (bearing from A prime)
if (Bearing_to_A_prime >180)
   Bearing_from_A_prime = Bearing_to_A_prime -180;
  else
   Bearing_from_A_prime = Bearing_to_A_prime +180;

Error_Angle=Bearing_from_A_prime-TARGET; // calculate current diff in  the angle of current position vs Target
// correct for 0/360 degree border issues. 
if (Error_Angle > 180)
  Error_Angle -= 360;
if (Error_Angle <-180)
  Error_Angle += 360;
Serial.println();
Serial.print("Bearing to / from A prime=  ")/
Serial.print(Bearing_to_A_prime);
Serial.print("  /   ");
Serial.print(Bearing_from_A_prime);
Serial.print(" Error_Angle = ");
Serial.print(Error_Angle);
Serial.print(" D= ");
Serial.println(D);


  // Calculate the distance we are off from the Target course. 
Cumulative_Error=D*sin(Error_Angle*PI/180); 
/*
calculate the current course angle minus the TARGET heading - assign to Error.
this calculation should be based on current heading compared with TARGET
*/
Error = fix.heading() - TARGET;  

 // look for 360 degree wrap around and correct if ncessary
  if (Error > 180)
        Error -= 360.0;
  if (Error < -180)
        Error += 360.0;
}


// display target/measurment/error/Cumulative_Error calculated by either compass or GPS.
if (!BAT) {
  Serial.print("Target: ");
  Serial.print(TARGET);
  Serial.print(",Measurement: ");
  Serial.print(Measurement);
  Serial.print(",Error: ");
  Serial.print(Error);
  Serial.print(",Cumulative Error: ");
  Serial.println(Cumulative_Error);
}
// Updat display with error, cumulative error, graphics for steering, etc once per loop
Update_Display();  


// Sample GPS information 

//Allow GPS parser a full 1.5 seconds to get at least one fix
unsigned long time = millis();
while (millis() <= (time+1500)) {
    while (gps.available( gpsPort )) {
    fix = gps.read();
    bool  validDT         = fix.valid.date & fix.valid.time;

    print(             fix.satellites       , fix.valid.satellites, 3             );
  //  print(             fix.hdop/1000.0      , fix.valid.hdop      , 6, 2          );
    print(             fix.latitude ()      , fix.valid.location  , 10, 6         );
    print(             fix.longitude()      , fix.valid.location  , 11, 6         );
    print(             fix.dateTime         , validDT             , 20            );
    print(             fix.altitude ()      , fix.valid.altitude  , 7, 2          );
    print(             fix.speed_kph()      , fix.valid.speed     , 7, 2          );
    print(             fix.heading  ()      , fix.valid.heading   , 7, 2          );
    print( gps.statistics.chars , true, 10 );
    print( gps.statistics.ok    , true,  6 );
    print( gps.statistics.errors, true,  6 );
    Serial.println();
 
 }

} // end of 1500 ms GPS phase for this cycle. Let's see how we did...

// Look for a string of good heading fixes IFF we are close to Compass-based guidance (low Error & Cum_Error)
Check_for_FIX();

 if (GPS_Ready > GPS_Heading_Hurdle && !GPS_flag) {// record GPS ready for use!!
   GPS_flag=true;  // we're done with the GPS acquisition phase (shorter phase if REAL_TARGET was assigned via long push)

   // assign new target for GPS navigation. 
    if (REAL_TARGET_Flag)
      TARGET = REAL_TARGET; // Use this if Long BUTTON PUSH was used to provide the actual target heading.
      else 
       TARGET = GPS_Heading;  // Use this if REAL_TARGET wasn't set via long BUTTON PUSH.
   New_Point_A_lat = fix.latitude();
   New_Point_A_lon = fix.longitude();
   Serial.println();
   Serial.print("Switching to GPS Mode!   New_Target=");
   Serial.print(TARGET);
   Serial.print(" Point A' = ");
   Serial.print(New_Point_A_lat,6);
   Serial.print("  /  ");
   Serial.println(New_Point_A_lon,6);
   Serial.println();

 }

}  // End of Loop




float Update_Display() {  // routine to display messages on OLED display

// draw a "upper half" circle whose radius reflects the overall size of accumulated error
  display.clearDisplay();
  int radius = int(fabs(Cumulative_Error))+7;  // why +7?  Don't let radius get too small - always want something visible
  display.fillCircle(SCREEN_WIDTH/2,SCREEN_HEIGHT-1,radius,WHITE);
  
  // if accumulated error is postive, wipe out left half of screen. Otherwise wipe out right half.  This produces a 1/4 circle. 
  if (Cumulative_Error >= 0) {
    display.fillRect(0,0,display.width()/2,display.height(),BLACK);
    // draw left turn sign on left side of screen if the accumulated error is larger than a threshold. 
    if (Cumulative_Error > Cum_Error_Threshold) display.drawBitmap(
    (display.width()/2  - TURN_WIDTH ) / 2,
    (display.height() - TURN_HEIGHT) / 2+10,
    epd_bitmap_Go_Left, TURN_WIDTH, TURN_HEIGHT, 1);
  }    
  else { // accumulated error is zero or negative
    display.fillRect(display.width()/2,0,display.width()/2,display.height(),BLACK);
    // draw right turn on right side of screen.
    if (Cumulative_Error < -Cum_Error_Threshold) display.drawBitmap(
    (display.width()/2  + TURN_WIDTH/2),
    (display.height() - TURN_HEIGHT) / 2+10,
    epd_bitmap_Go_Right, TURN_WIDTH, TURN_HEIGHT, 1);
  }

    // Draw a vertical line to bisect the display to make left/right error more clear.
    display.drawLine(SCREEN_WIDTH/2,SCREEN_HEIGHT/4+4,SCREEN_WIDTH/2,SCREEN_HEIGHT-1,WHITE);
  
  // Display current cycle Error amount.  If cumulative error is large, raise to yellow portion of screen.
    display.setTextSize(1); // Draw 1X-scale text
    display.setTextColor(WHITE,BLACK);
   
    if (fabs(Cumulative_Error) >= ALARM_THRESHOLD){
      display.setTextSize(2); // Draw 2X-scale text
      display.setCursor(display.width()/2-10, 0); // draw on top (yellow) line w/tweak to start of text to center on screen
    }
    else {
      display.setTextSize(1); 
      display.setCursor(display.width()/2-6,16); // draw smaller text on 2nd line
    }

    display.println(int(Error*10));  // simplify expression of error to (usually)single digit by multiplying by 10 and making an int.
    
// If cumulaltive error is huge display the cumulative error amount in the big blue square right or left.
  if (Cumulative_Error >= HUGE_THRESHOLD) {   // write cumulaltive error on right side
    display.setTextSize(2); 
      display.setCursor(display.width()/2+6,30);
      display.println(int(Cumulative_Error));
  }

  if (Cumulative_Error <= -HUGE_THRESHOLD) {  // write cumulative error on the left side.
    display.setTextSize(2);
      display.setCursor(2,30);
      display.println(int(Cumulative_Error));
    }

    // Display GPS status
    
    display.setTextSize(1);
    display.setCursor(display.width()-12,0);
    if (GPS_flag){
       display.print("G!"); // now using GPS
       display.setCursor(0,0);
       display.println(fix.heading()); //display current heading
    }       
    else {
    if (GPS_Ready>1)
      display.println("~G");  // GPS is "in training"
      else
      display.println("!G"); // No GPS data available yet.
    }
    //Refresh screen for current cycle
    display.display(); // send it. 
 
 } //end of Update_Display()


float read3() {
   int x, y, z, x1, y1, z1, x2, y2, z2; // these are 4 byte entities on Seeeduino XIAO
   
   short sx, sy, sz, sx1, sy1, sz1, sx2, sy2, sz2; // compass will deliver 2 byte entities (signed)

   //int16_t sx, sy, sz, sx1, sy1, sz1, sx2, sy2, sz2; // compass will deliver 2 byte entities (signed)

   // take 3 samples 300ms apart
   compass.read(&x, &y, &z);
   delay(200);
   compass.read(&x1, &y1, &z1);
   delay(200);
   compass.read(&x2, &y2, &z2);
   delay(200);

   //convert all samples to 16bit signed numbers
  sx=x; sx1=x1; sx2=x2;
  sy=y; sy1=y1; sy2=y2;
  sz=z; sz1=z1; sz2=z2;
  
   // average the three samples to remove a bit of noise. 
   sx=(sx+sx1+sx2)/3;
   sy=(sy+sy1+sy2)/3;
   sz=(sz+sz1+sz2)/3;

   // Calculate heading when the magnetometer is level, then correct for signs of axis.
   // Atan2() automatically check the correct formula taking care of the quadrant you are in
   float heading = atan2(-sx, sy);

   // Correct for when signs are reversed.
   if (heading < 0)
    heading += 2 * PI;

   // Convert radians to degrees for readability.
   float Degrees = heading * 180 / PI;
 if (!BAT) {
   Serial.print("  x: ");
  // Serial.print(x);
  Serial.print(sx);
  //Serial.print(x2);
   Serial.print(",y: ");
  // Serial.print(y);
  Serial.print(sy);
  // Serial.print(y2);
   Serial.print(",z: ");
  // Serial.print(z);
   //Serial.print(z1);
   Serial.print(sz);
 }
   return Degrees;
 } // End of Read3()


 // Look for a string of good heading fixes IFF we are close to Compass-based guidance (low Error & Cum_Error)
float Check_for_FIX() {   //           V <<< Evaluate sizes of >>>  V 
 /* this routine calculates the average of a series of headings.  It might be much easier (and maybe more accurate)
 to simply calculate a course from the lat/lon at beginning and end of this loop (GPS_Ready=1 -> 15) 
 */
 
  if (fix.valid.location && abs(Error)<5 && abs(Cumulative_Error) < 20  && !GPS_flag && GPS_Ready <=GPS_Heading_Hurdle)  {  //   <<<<<<---------- (change to check for fix.valid.heading for real code)
      //         ^^^^^    calculate average heading so far
      if (GPS_Ready == 1) 
          GPS_Heading = fix.heading();  //first one stands alone...
          else {
          if (abs(fix.heading()-GPS_Heading) >= 330) { // deal with 360/zero wrap).
            if (fix.heading() < 30) // this sample is on the + side of 360. GPS_Heading must be just under 360
              GPS_Heading = (GPS_Heading * (GPS_Ready-1) + fix.heading()+360) / GPS_Ready;
              else  // this sample is on the - side of 360 & GPS_Heading must be above 0.
                GPS_Heading = ((GPS_Heading+360) * (GPS_Ready-1) + fix.heading()) / GPS_Ready;
                
          }
          else  // not a 360/0 border issue calculate average as usual
            GPS_Heading = (GPS_Heading * (GPS_Ready-1) + fix.heading()) / GPS_Ready;
          }
      if (GPS_Heading>360)  // Correct for over 360 condition on current average.
        GPS_Heading -= 360;  

        Serial.println();
        Serial.print("GPS_Ready= ");
        Serial.print(GPS_Ready);
        Serial.print(" GPS_Heading= ");
        Serial.print(GPS_Heading,3);
        Serial.print(" fix.heading= ");
        Serial.println(fix.heading());
        GPS_Ready=GPS_Ready+1;  // Get ready for next cycle.

    }
    else { // one or more conditions (i.e., error, Cum_error, valid fix) has failed
      GPS_Ready=1;  // reset hurdle condition - restart if we miss one or not close to Compass derived line.
     // GPS_flag=false; 
      GPS_Heading = 0;
    }
    
    if (fix.valid.location && REAL_TARGET_Flag)  // <<<<<< Changed to .heading
      GPS_Ready=GPS_Heading_Hurdle+1;  // don't waste time calculating an average heading if you already have a heading. 
} // End of Check_for_FIX


//-----------------
//  Print utilities

static void repeat( char c, int8_t len )
{
  for (int8_t i=0; i<len; i++)
    Serial.write( c );
}

static void printInvalid( int8_t len )
{
  Serial.write( ' ' );
  repeat( '*', abs(len)-1 );
}

static void print( float val, bool valid, int8_t len, int8_t prec )
{
  if (!valid) {
    printInvalid( len );
  } else {
    char s[16];
    dtostrf( val, len, prec, s );
    Serial.print( s );
  }
}

static void print( int32_t val, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    char s[16];
    ltoa( val, s, 10 );
    repeat( ' ', len - strlen(s) );
    Serial.print( s );
  }
}

static void print( const __FlashStringHelper *str, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    int slen = strlen_P( (const char *) str );
    repeat( ' ', len-slen );
    Serial.print( str );
  }
}

static void print( const NeoGPS::time_t & dt, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    Serial.write( ' ' );
    Serial << dt; // this "streaming" operator outputs date and time
  }
}
