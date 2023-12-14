/*
This program allows you to drive a car using a bluetooth joystick and collect data using different sensing modes. 
Bluetooth Joystick is able to control the motors using my phone app. I can also send texts to arduino, use a button, and drive.
The Ultrasonic sensor is able to determine the distance between the car and an object in its way. 
The Accelerometer is able to calculate acceleration, but must calibrate first. Maximum x and y axis values are saved in EEPROM at the end of the demonstration.
I will send a text to the Arduino for different driving modes, save maxmimum x and y acceleration values onto EEPROM. 
*/

//NEED TO ADD ULTRASONIC SENSOR STUFF

#include <SoftwareSerial.h> //Header File for serial communication on other digital pins of an Arduino
#include <ArduinoBlue.h> //Header File for Arduino Blue for Bluetooth Joystick
#include <stdbool.h>  //Header File for boolean states
#include <EEPROM.h>  //Header File for EEPROM library

//Digital Variables
const int buttonPin = 3;    // Calibrate Button
const int trigPin = 4;   // Ultrasonic sensor trigger pin
#define enA 5
#define in1 6
#define in2 7
#define in3 8
#define in4 9
#define enB 10 
const int BLUETOOTH_TX = 11; // Bluetooth TX -> PIN 11
const int echoPin = 12;  // Ultrasonic sensor echo pin
const int BLUETOOTH_RX = 13; // Bluetooth RX -> PIN 13

//Analog Variables
const int xInput = A0;      //X-out is in analog pin A0
const int yInput = A1;      //Y-out is in analog pin A1
const int zInput = A2;      //Z-out is in analog pin A2
const int leftLine = A3;    //Left line Sensor A0 is in analog pin A3
const int rightLine = A4;   //Right line Sensor A0 is in analog pin A4

// Raw Ranges:
// Min and Max will be initialized in the calibration
// routine, which will find the true minimum and
// maximum for each axis
int xRawMin;
int xRawMax;
int xRaw, xNorm;

int yRawMin;
int yRawMax;
int yRaw, yNorm;

int zRawMin;
int zRawMax;
int zRaw, zNorm;

unsigned long timenow;    // Stores current time in milliseconds
unsigned long timelater;  // Stores previous time in milliseconds
unsigned long changeintime;  // Stores the time difference
long duration, distance; //ultrasonic sensor
const int sampleSize = 10; // samplesize to reduce noise

//EEPROM address
int addr1 = 5;   // EEPROM address
int addr2 = 6;   // EEPROM address 

String str;
int prevThrottle = 49;
int prevSteering = 49;
int throttle, steering, sliderVal, button, sliderId;
int spd, turnSpdRight, Rspd, Lspd, IRvalueA, IRvalueB;
int maxAccelY, maxAccelX;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);
ArduinoBlue phone(bluetooth);  // pass reference of bluetooth object to ArduinoBlue constructor.

// Setup code runs once after program starts.
void setup() {
  // initializing pins as inputs and outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(rightLine,INPUT);
  pinMode(leftLine,INPUT);
  pinMode(buttonPin,INPUT);

  // set direction once
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // Start serial monitor at 9600 bps.
  Serial.begin(9600);

  // Start bluetooth serial at 9600 bps.
  bluetooth.begin(9600);

  // delay just in case bluetooth module needs time to "get ready".
  //delay(500); //delay of 500ms

  //Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);  //Set trigPin pin as output
  pinMode(echoPin, INPUT);   //Set echoPin pin as input

  //EEPROM debug
  //int storagex = EEPROM.read(addr1);  // debug for EEPROM
  //int storagey = EEPROM.read(addr2); // debug for EEPROM
  //Serial.begin(11520);  // debug for EEPROM

  Calibrate();
  Serial.print(xRawMin); Serial.print(" ");
  Serial.print(xRawMax); Serial.print(" ");
  Serial.print(yRawMin); Serial.print(" ");
  Serial.print(yRawMax); Serial.print(" ");
  Serial.print(zRawMin); Serial.print(" ");
  Serial.print(zRawMax); Serial.print("\n");
  
  Serial.println("setup complete"); 
}

// Put your main code here, to run repeatedly:
void loop() {
  // ID of the button pressed pressed.
  button = phone.getButton();

  // Returns the text data sent from the phone.
  // After it returns the latest data, empty string "" is sent in subsequent.
  // calls until text data is sent again.
  str = phone.getText();

  // Throttle and steering values go from 0 to 99.
  // When throttle and steering values are at 99/2 = 49, the joystick is at center.
  throttle = phone.getThrottle();
  steering = phone.getSteering();

  // ID of the slider moved.
  sliderId = phone.getSliderId();

  // Slider value goes from 0 to 200.
  sliderVal = phone.getSliderVal();

  // Display button data whenever its pressed.
  if (button != -1) {
    Serial.print("Button: ");
    Serial.println(button);
  }

  // Display slider data when slider moves
  if (sliderId != -1) {
    Serial.print("Slider ID: ");
    Serial.print(sliderId);
    Serial.print("\tValue: ");
    Serial.println(sliderVal);
  }

  // Display throttle and steering data if steering or throttle value is changed
  if (prevThrottle != throttle || prevSteering != steering) {
    Serial.print("Throttle: ");
    Serial.print(throttle);
    Serial.print("\tSteering: ");
    Serial.println(steering);
    prevThrottle = throttle;
    prevSteering = steering;
  }

  // If a text from the phone was sent print it to the serial monitor
  if (str != "") {
    Serial.println(str);
  }

  // Send string from serial command line to the phone. This will alert the user.
  if (Serial.available()) {
    Serial.write("send: ");
    String str = Serial.readString();
    phone.sendMessage(str);  // phone.sendMessage(str) sends the text to the phone.
    Serial.print(str);
    Serial.write('\n');
  }

  if (str == "1") {
    autoMode();
  } else if (str == "2") {
    forward();
    stop();
    reverse();
  }
  if(str == "x"){
    EEPROM.put(addr1, maxAccelX); // find the maximum acceleration x direction
  }
  if(str=="y"){
    EEPROM.put(addr2, maxAccelY); // find the maximum acceleration y direction
  }

  ReadAllAxis(&xRaw, &yRaw, &zRaw);
  timenow = millis();
  Serial.print(timenow); Serial.print(", ");
  Serial.print(xRaw); Serial.print(", ");
  Serial.print(yRaw); Serial.print(", ");
  Serial.print(zRaw);
  // Convert raw values to 'milli-Gs"
  long xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000);
  long yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
  long zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);
  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;
  Serial.print(" :: ");
  Serial.print(xAccel); Serial.print("G, ");
  Serial.print(yAccel); Serial.print("G, ");
  Serial.print(zAccel); Serial.println("G");
// delay(1000);

  forward(); // Call the forward function
  stop(); // Call the stopp function
  reverse(); // Call the reverse function
  ultrasonicSensor();  // Call the ultrasonicSensor function
}

//Collaboration with Nicolas who helped create this function
//Function allows the car to go forward
void forward() {
  if (throttle < 45) {                     //when joystick is in one direction
    spd = map(throttle, 45, 0, 0, 255);    //sets speed based on how far the joystick is from the center
    Rspd = map(steering, 45, 0, 0, 220);   //turn speed for the right wheel, it is inverse to the normal speed, and later gets subtracted
    Lspd = map(steering, 45, 99, 0, 220);  //turn speed for the left wheel, same function as Rspd.
    digitalWrite(in1, HIGH);               //sets direction
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    if (steering <= 45) {    //when the joystick is turned right, calculates how much turn is needed
      if (spd - Rspd < 0) {  //ensures that the subtraction of speed on the right wheel is not negative
        Rspd = 90;           //if negative, then set as max
      }
      analogWrite(enA, (spd - Rspd) * 2);  //sets right motor speed to be a lot slower than left, which makes it turn right
      analogWrite(enB, spd);               //sets left motor as usual speed.
    } else if (steering >= 55) {           //when joystick is turned left, calculates how much turn is needed
      if (spd - Lspd < 0) {                //ensures that the subtraction of speed on the left wheel is not negative
        Lspd = 90;                         //if negative, then set as max
      }
      analogWrite(enB, (spd - Lspd) * 2);  //sets left motor speed to be a lot slower than right, which makes it turn left
      analogWrite(enA, spd);               //sets right motor as usual speed.
    } else {
      analogWrite(enB, spd);  //if no turn is seen, then proceed normal, go forward.
      analogWrite(enA, spd);
    }
  }
}

//Collaboration with Nicolas who helped create this function
//Function allows the car to go backwards
void reverse() {
  if (throttle >= 55) {                    //when joystick is in one direction
    spd = map(throttle, 55, 99, 0, 255);   //sets speed based on how far the joystick is from the center
    Rspd = map(steering, 55, 0, 0, 220);   //turn speed for the right wheel, it is inverse to the normal speed, and later gets subtracted
    Lspd = map(steering, 55, 99, 0, 220);  //turn speed for the left wheel, same function as Rspd.
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    if (steering <= 45) {    //when the joystick is turned right, calculates how much turn is needed
      if (spd - Rspd < 0) {  //ensures that the subtraction of speed on the right wheel is not negative
        Rspd = 90;           //if negative, then set as max
      }
      analogWrite(enA, (spd - Rspd) * 2);  //sets right motor speed to be a lot slower than left, which makes it turn right
      analogWrite(enB, spd);               //sets left motor as usual speed.
    } else if (steering >= 55) {           //when joystick is turned left, calculates how much turn is needed
      if (spd - Lspd < 0) {                //ensures that the subtraction of speed on the left wheel is not negative
        Lspd = 90;                         //if negative, then set as max
      }
      analogWrite(enB, (spd - Lspd) * 2);  //sets left motor speed to be a lot slower than right, which makes it turn left
      analogWrite(enA, spd);               //sets right motor as usual speed.
    } else {
      analogWrite(enB, spd);  //if no turn is seen, then proceed normal, go forward.
      analogWrite(enA, spd);
    }
  }
}

void stop()  //Collaboration with Nicolas and Kenneth who helped me use the joystick
{  
  if ((throttle >= 45 && throttle <= 55)) 
  {
    digitalWrite(in1, LOW);  //turns off the motor inputs if within the resting joystick position
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enA, 0);  //motors ends
    analogWrite(enB, 0);
  }
}

/*
Function reads the Ultrasonic sensor and determines the distance between an object and the sensor.
*/
void ultrasonicSensor() 
{
  digitalWrite(trigPin, LOW);  //off sound waves
  delayMicroseconds(2);        //delay

  digitalWrite(trigPin, HIGH);  //on sound waves
  delayMicroseconds(10);        //delay
  digitalWrite(trigPin, LOW);   //off sound waves

  duration = pulseIn(echoPin, HIGH);  //Measures the time it takes for sound wave to hit object
  distance = (duration * 0.034) / 2;  // Measure distance with the ultrasonic sensor
}
boolean debounceCalibrate(boolean last) {
  boolean current = digitalRead(buttonPin);  //Read the button state
  if (last != current) {                     //if it's different...
    delay(5);                                //wait 5ms
    current = digitalRead(buttonPin);        //read it again
  }
  return current;  //return the current value
}

// Read "sampleSize" samples and report the average
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(10); // I found without this delay, the first set of readings would be inaccurate, low
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}
void ReadAllAxis (int* xNewptr, int* yNewptr, int* zNewptr)
{
  *xNewptr = ReadAxis(xInput);
  *yNewptr = ReadAxis(yInput);
  *zNewptr = ReadAxis(zInput);
}



// Function to calibrate the accelerometer
void Calibrate() {
    // initialize all mins and maxes with one read
    ReadAllAxis(&xRawMin, &yRawMin, &zRawMin);
    xRawMax = xRawMin;
    yRawMax = yRawMin;
    zRawMax = zRawMin;

    // Start main calibration
    Serial.println("Calibrate");

    // Calibration for x-axis
    Serial.println("Align x-axis up.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    // Calibration for x-axis down
    Serial.println("Align x-axis down.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    // Calibration for y-axis
    Serial.println("Align y-axis up.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    // Calibration for y-axis down
    Serial.println("Align y-axis down.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    // Calibration for z-axis
    Serial.println("Align z-axis up.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    // Calibration for z-axis down
    Serial.println("Align z-axis down.  When ready press button.");
    while (digitalRead(buttonPin) != LOW) {}
    ReadAllAxis(&xRaw, &yRaw, &zRaw);
    AutoCalibrate(xRaw, yRaw, zRaw);
    delay(1000);
    Serial.println("Calibration completed. \n");
}

// Find the extreme raw readings from each axis
void AutoCalibrate(int xNew, int yNew, int zNew)
{
    if (xNew < xRawMin) // update x Min and Max
    {
      xRawMin = xNew;
    }
    if (xNew > xRawMax)
    {
      xRawMax = xNew;
    }
    if (yNew < yRawMin) // update y Min and Max
    {
      yRawMin = yNew;
    }
    if (yNew > yRawMax)
    {
      yRawMax = yNew;
    }
    if (zNew < zRawMin) // update z Min and Max
    {
      zRawMin = zNew;
    }
    if (zNew > zRawMax)
    {
      zRawMax = zNew;
    }
}

// Function to calculate and record maximum accelerations
void recordYMaxAccelerations(float yAccel) {
     // Check and update the maximum acceleration in the y-axis
     if (yAccel > maxAccelY) 
     {
        maxAccelY = yAccel;  
     }
}
void recordXMaxAccel(float xAccel) {
  // Check and update the maximum acceleration in the x-axis
    if (xAccel > maxAccelX) 
    {
      maxAccelX = xAccel;         
    }
}

//function for auto mode using line sensors
//collaboration with Kenneth Wang who helped me use the line sensor
void autoMode() {
  IRvalueA = analogRead(leftLine);  //left
  IRvalueB = analogRead(rightLine);  //right
  digitalWrite(in1, HIGH);        //sets direction
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  if (IRvalueA <= 150 && IRvalueB <= 150) {
    spd = 50;
    analogWrite(enA, spd);
    analogWrite(enB, spd);
  } else if (IRvalueA > 150) {
    spd = 50;
    analogWrite(enA, spd / 2);
    analogWrite(enB, spd * 2);
  } else if (IRvalueB > 150) {
    spd = 50;
    analogWrite(enA, spd * 2);
    analogWrite(enB, spd / 2);
  } else {
    digitalWrite(in1, LOW);  //turns off the motor inputs if within the resting joystick position
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enA, 0);  //motors ends
    analogWrite(enB, 0);
  }
}