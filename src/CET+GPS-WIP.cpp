
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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

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


/*
   Use the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   9600-baud serial GPS device hooked up on pins 9(rx) and 8(tx).
*/
static const int RXPin = 9, TXPin = 8;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


//some global variables...
bool flag = true;
bool BAT = false;
float TARGET=777;
float Error=77;
float Cumulative_Error = 0;

void setup() {
 
  Wire.begin();     
  Serial.begin(9600);  
  ss.begin(GPSBaud);
  compass.init();     
  

 
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

delay(500); // Pause for .5 seconds
 
  // Clear the display buffer
  display.clearDisplay();

  // set up Pin D0 as an input for a pushbutton. May be used someday.... Pulled down with 10K resistor.
  pinMode(0,INPUT);


}

void loop() {

int Button = digitalRead(0);
  if (flag || Button) {  //Sample target heading on first pass through the loop or anytime the button is pressed
 
    while (Button) {
      if (!BAT) Serial.println("BUTTON PUSHED");
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.setTextColor(WHITE,BLACK);
      display.println("BUTTON");
      display.println("PUSHED");
      display.display();
      delay(500);
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
    TARGET = (read3() + read3() + read3()) / 3;  // read 9 samples and average them. 
    Cumulative_Error=0; 

    // Signal on the display that we have a target to use.     
    display.clearDisplay();
    display.setCursor(0,20);
    display.println("TARGET");
    display.println("ACQUIRED!");
    display.display();
    flag = false;  // don't do the above again unless reset. 
    delay(600);  // get ready for cycles of measurements to assess error from target course.
  }

  // read 3 compass samples averaged to get current heading.
  float Measurement = read3();

  Error = Measurement - TARGET;

  // look for 360 degree wrap around and correct if ncessary
  if (Error > 180)
        Error -= 360.0;
  if (Error < -180)
        Error += 360.0;

  Cumulative_Error += Error;  // accumulate overall error

if (!BAT) {
  Serial.print(",Target: ");
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


delay(1000); // snooze 1 sec til next cycle.


// Check for GPS INS.  If valid data from GPS switch to using it. 
// each cycle update Error and Cumulative_Error to use by the display routine (using either GPS or compass)
// display something when we switch to using GPS data...


// This sketch displays information every time a new sentence is correctly encoded.

  while (ss.available() > 0)
    if(gps.encode(ss.read()))  
      displayInfo();

delay(2000);  // let the encoder get the first available sentence and print the next one mself. 

  while (ss.available() > 0) {
    char character = ss.read();
      Serial.print(character);
  } 
  

/*
 //if (gps.location.isValid())
//  {
    Serial.print(F("LOCATION   Fix Age="));
    Serial.print(gps.location.age());
    Serial.print(F("ms Raw Lat="));
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.print(gps.location.rawLat().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLat().billionths);
    Serial.print(F(" billionths],  Raw Long="));
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.print(gps.location.rawLng().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLng().billionths);
    Serial.print(F(" billionths],  Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.println(gps.location.lng(), 6);
 // }

 // else if (gps.date.isValid())
 // {
    Serial.print(F("DATE       Fix Age="));
    Serial.print(gps.date.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.date.value());
    Serial.print(F(" Year="));
    Serial.print(gps.date.year());
    Serial.print(F(" Month="));
    Serial.print(gps.date.month());
    Serial.print(F(" Day="));
    Serial.println(gps.date.day());
 // }

//  else if (gps.time.isValid())
//  {
    Serial.print(F("TIME       Fix Age="));
    Serial.print(gps.time.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.time.value());
    Serial.print(F(" Hour="));
    Serial.print(gps.time.hour());
    Serial.print(F(" Minute="));
    Serial.print(gps.time.minute());
    Serial.print(F(" Second="));
    Serial.print(gps.time.second());
    Serial.print(F(" Hundredths="));
    Serial.println(gps.time.centisecond());
  //}
  */
  
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
    //Refresh screen for current cycle
    display.display(); // send it. 
 
 }
//end of Update_Display()




float read3() {
   int x, y, z, x1, y1, z1, x2, y2, z2; // these are 4 byte entities on Seeeduino XIAO
   
   short sx, sy, sz, sx1, sy1, sz1, sx2, sy2, sz2; // compass will deliver 2 byte entities (signed)

   //int16_t sx, sy, sz, sx1, sy1, sz1, sx2, sy2, sz2; // compass will deliver 2 byte entities (signed)

   // take 3 samples 300ms apart
   compass.read(&x, &y, &z);
   delay(300);
   compass.read(&x1, &y1, &z1);
   delay(300);
   compass.read(&x2, &y2, &z2);
   delay(300);

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
   float heading = atan2(sy, sx);

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
 }



void displayInfo() // routine to display GPS data (called from Loop)
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
