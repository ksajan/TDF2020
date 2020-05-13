/*
  Adafruit Prop-Maker Featherwing
  NeoPixel Example

  Rainbow example for 30-Pixel NeoPixel Strip
*/
// Add the Neopixel library
#include <Adafruit_NeoPixel.h>

// Add the accelerometer library
#include <Adafruit_LIS3DH.h>

// Input the number of neopixels in your system. For our system, you can use up to 7.
#define NUM_PIXELS 7

// Next define which pin is going to control the neopixels. We're using pin 5 (check pinout sheet).
#define NEOPIXEL_PIN 5

// The prop-maker board uses a pin to turn neopixels on and off in case we don't want them to draw any power. This will be pin 10.
#define POWER_PIN    10

//This code is only used if we want the "tapping" functionality of the board. Let's leave it on for now.
// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 40


// this line adds the i2c accelerometer
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// this line creates a neopixel strip we'll use
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// These variables will help us keep track of some accelerometer thesholds.
// The first variable could help us keep track of how many times the board is "tilted"
int tiltChangeCounter = 0;

// The second variable helps us track the current state of the board (whether or not it's been tilted)
int tiltState = 0;

// The third variable helps us check whether the current state = the last state of the board
// AKA whether or not something has changed
int lastTiltState = 0;

// Let's get started!
void setup() {
  // We initialize our serial monitor at a baudrate of 115200, and then print a line of text to make sure it worked
  Serial.begin(115200);
  Serial.println("\nProp-Maker Wing: NeoPixel AND Accelerometer");

  // We'll set our power pin as an output
  pinMode(POWER_PIN, OUTPUT);
  // Then we'll actually enable the power pin so we can write to neopixels.
  digitalWrite(POWER_PIN, HIGH);

  // This line initializes the NeoPixel library (getting the lights ready to light).
  strip.begin();

  // Here we're just going to store an arbitrary variable as a number so we can use it to count in a loop
  int i = 0;

  // This sets a particular color to each of the neopixels at start, using our number of pixels as a variable
  for (i = 0; i < strip.numPixels(); i++) {
    // The color "0, 255, 0" is "green" because "Red, Green, Blue." Blue would be: "0, 0, 255" and purple would be "255, 0, 255".
    strip.setPixelColor(i, 0, 255, 0);
    // We have to run "strip.show()" every time we change the color of a neopixel to populate that color to the LED
    strip.show();

    // This bit just gets the accelerometer going
    Serial.println("Starting LIS3DH...");
    if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
      Serial.println("Couldnt start LIS3DH");
      while (1);
    }
    Serial.println("LIS3DH Started!");

    // Set Accelerometer Range (2, 4, 8, or 16 G!)
    lis.setRange(LIS3DH_RANGE_4_G);

    /* Set Accelerometer Click Detection
      0 = turn off click detection & interrupt
      1 = single click only interrupt output
      2 = double click only interrupt output
      NOTE: Higher numbers are less sensitive
    */
    lis.setClick(1, CLICKTHRESHHOLD);
  }
}

// We finished our setup. Now let's work on the code that loops forever:

void loop() {

  // This bit can help us detect "clicks."
  // Detect a click
  uint8_t click = lis.getClick();

  // This bit reads the built-in accelerometer and returns values we can use.
  // Get a new accel. sensor event
  sensors_event_t event;
  lis.getEvent(&event);

  // If at any point we detect a click, turn blue for a second
  // detect click
  if (click & 0x10) {
    uint16_t k, m;
    for (k = 0; k < strip.numPixels(); k++) {
      strip.setPixelColor(k, 0, 0, 255);
      strip.show();
    }
    delay(500);
    for (m = 0; m < strip.numPixels(); m++) {
    strip.setPixelColor(m, 0, 0, 0);
    strip.show();
    }
  }

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");
  Serial.println();

  // Creating some arbitrary variable intergers under "i" and "j". You could also do "int i = 0" "int j = 0"
  uint16_t i, j;

  // This is where we read the acceleration of "z". Under normal circumstances, "z" will read as accelerating at 9.67m/s^2.
  // So if we read a number < 5, we know we're tilting.
  if (event.acceleration.z < 5) {
    // Mark our "tilt state" as "1", or "tilting"
    tiltState = 1;
    // Now check if "tilt state" is the same as our last tilt state. If they're different, it means we were upright, and then tilted.
    if (tiltState != lastTiltState) {
      Serial.println("Overturned");
      // Fade from "green" to "red" using two for loops: one for addressing each pixel, the other for shifting the colors
      for (j = 0; j < 255; j++) {
        for (i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, j, 255 - j, 0);
          strip.show();
        }
      }
    }
    // Register this tilt state as the one that now happened in the past
    lastTiltState = tiltState;
    delay(500);
  }

  // If acceleration is GREATER than 5, or anything between 5 and some large number (20, here), consider us upgright
  else if (event.acceleration.z < 20) {
    // Set tilt state to "0", or "not tilting"
    tiltState = 0;
    // If this is different than the last tilt state, print "upright" to the serial monitor and fade red to green
    if (tiltState != lastTiltState) {
      Serial.println("Upright");
      for (j = 0; j < 255; j++) {
        for (i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, 255 - j, j, 0);
          strip.show();
        }
      }
    }
    // Register this tilt state as the one that now happened in the past
    lastTiltState = tiltState;
    delay(500);
  }
  //Just slowing things down a tiny bit
  delay(100);
}






// Below here is some code you could use later, if you want. Otherwise you can ignore it.

//uint32_t Wheel(byte WheelPos) {
//  // Input a value 0 to 255 to get a color value.
//  // The colours are a transition r - g - b - back to r.
//  WheelPos = 255 - WheelPos;
//  if(WheelPos < 85) {
//    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  }
//  if(WheelPos < 170) {
//    WheelPos -= 85;
//    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//  WheelPos -= 170;
//  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//}
//
//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    // turn on the Prop-Maker FeatherWing's power pin
//    digitalWrite(POWER_PIN, HIGH);
//    // write the pixel values
//    strip.show();
//    delay(wait);
//  }
//}
