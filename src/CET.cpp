
/*
   Derived from: e-Gizmo QMC5883L GY-271 Compass

   Sample sketch for the GY-271 QMC5883L
   for getting the raw data of x, y, z and
   Radius in degrees.

   Codes by e-Gizmo Mechatronix Central
   http://www.e-gizmo.com
   July 10,2017

*/
/*********
  Also derived from:  Complete project details at https://randomnerdtutorials.com
  
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#include <QMC5883L.h>

QMC5883L compass;
bool flag = true;
float Cumulative_Error = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  compass.init();
  
    //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);

 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(200); // Pause for .2 seconds

  // Clear the buffer
  display.clearDisplay();

}

void loop() {

  float TARGET;

  if (flag) {  //Sample target heading on first pass through the loop
 
    Serial.print("   Reading TARGET ");
    TARGET = read3();  
    flag = false;
    delay(100);
  }

  // read 3 compass samples averaged to get current heading.
  float Measurement = read3();

  float Error = Measurement - TARGET;
  // look for 360 degree wrap around and correct if ncessary
  if (Error > 180)
        Error -= 360.0;
  if (Error < -180)
        Error += 360.0;


  Cumulative_Error += Error;

  Serial.print("    Target: ");
  Serial.print(TARGET);

  Serial.print("    Measurement: ");
  Serial.print(Measurement);
  Serial.print("   Error: ");
  Serial.print(Error);
  Serial.print("   Cumulative Error: ");
  Serial.println(Cumulative_Error);
  


// draw a "upper half" circle that reflects the overall size of accumulated error
  display.clearDisplay();
    
  int radius = int(fabs(Cumulative_Error))+7;  // Don't let radius get too small - always want something visible
  display.fillCircle(SCREEN_WIDTH/2,SCREEN_HEIGHT-1,radius,WHITE);
  
  // if accumulated error is postive, wipe out left half of screen. Otherwise wipe out right half.
  if (Cumulative_Error >= 0) 
    display.fillRect(0,0,display.width()/2,display.height(),BLACK);
  else 
    display.fillRect(display.width()/2,0,display.width()/2,display.height(),BLACK);

    // Draw a vertical line to bisect the display to make left/right error more clear.
    display.drawLine(SCREEN_WIDTH/2,SCREEN_HEIGHT/4+4,SCREEN_WIDTH/2,SCREEN_HEIGHT-1,WHITE);
  
  // Display current cycle Error amount.  If cumulative error is large, raise to yellow portion of screen.
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(WHITE,BLACK);
   
    if (fabs(Cumulative_Error) >= 60)
      display.setCursor(55, 0);
    else 
      display.setCursor(55,20);
    
    display.println(int(Error*10));  // simplify expression of error to (usually)single digit.
    
    //Refresh screen
    display.display(); // send it. 

  delay(1000); // snooze 1 sec til next cycle.
}



float read3() {
  int x, y, z, x1, y1, z1, x2, y2, z2;

  // take 3 samples 300ms apart
  compass.read(&x, &y, &z);
  delay(300);
  compass.read(&x1, &y1, &z1);
  delay(300);
  compass.read(&x2, &y2, &z2);
  // average the three samples to remove a bit of noise.
  x = (x + x1 + x2) / 3;
  y = (y + y1 + y2) / 3;
  z = (z + z1 + z2) / 3;

  // Calculate heading when the magnetometer is level, then correct for signs of axis.
 
  float heading = atan2(y, x);

  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Convert radians to degrees for readability.
  float Degrees = heading * 180 / PI;
 
  return Degrees;
}
