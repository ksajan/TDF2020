#include <Adafruit_LIS3DH.h>
#include <Servo.h>
const int BUTTON_PIN = 10;     // the number of the pushbutton pin
#define CLICKTHRESHHOLD 80
Servo myservo;
Servo myservo1;
int pos = 0;
int motorPin = 5;
int buttonState = 0;         // variable for reading the pushbutton status
//bool oldState = HIGH;
int motorCtrlVal=0;
int motorCtrlVal2=255;
int BuzzTime=750;

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Prop-Maker Wing: Accelerometer and Servo working");
  myservo.attach(A0);
  myservo1.attach(A1);
  pinMode(BUTTON_PIN, INPUT);
  //pinMode(buttonPin, INPUT);
  // This bit just gets the accelerometer going
  Serial.println("Starting LIS3DH...");
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start LIS3DH");
    while (1);
  }
  Serial.println("LIS3DH Started!");

  // Set Accelerometer Range (2, 4, 8, or 16 G!)
  lis.setRange(LIS3DH_RANGE_4_G);
  lis.setClick(1, CLICKTHRESHHOLD);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t click = lis.getClick();

  // This bit reads the built-in accelerometer and returns values we can use.
  // Get a new accel. sensor event
  // read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);
  //bool newState = digitalRead(BUTTON_PIN);

  sensors_event_t event;
  lis.getEvent(&event);
  // If at any point we detect a click, turn blue for a second
  // detect click
 
    if (buttonState == HIGH) {
        Serial.println("clicked");
        analogWrite(motorPin, motorCtrlVal);
      delay(BuzzTime);
      analogWrite(motorPin, motorCtrlVal2);
      delay(BuzzTime);
      analogWrite(motorPin, motorCtrlVal);
      delay(BuzzTime);
      analogWrite(motorPin, motorCtrlVal2);
      delay(BuzzTime);
    }
  
  if (click & 0x10) {
    uint16_t k, m;
    Serial.println("Click recieved");
    for (pos = 50; pos <= 110; pos += 1) {
    myservo1.write(pos);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 90; pos >= 30; pos -= 1) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
    for (pos = 110; pos >= 50; pos -= 1) {
    myservo1.write(pos);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 30; pos <= 90; pos += 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  }
  //oldState = newState;
}
