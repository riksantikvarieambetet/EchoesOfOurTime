#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Mouse.h>
#include <SensorToButton.h> // Button debouncing library

//Uses an Arduino Micro with an MPU-6050 6-DoF (Accel/Gyro) as A HID device to control 3D Vista Virtual Tour Pro via mouse movement and click. Uses LED's to indicate direction (1pc/each) and button push (4pcs). 

//#define DEBUG 1 // comment out to disable Serial.print() in loop


const int ledPinLeft = 4; // left direction LED
const int ledPinRight = 5; // right direction LED
const int ledPinUp = 6; // up direction LED
const int ledPinDown = 7;// down direction LED
const int ledPinMouse1 = 8; // mouse press LED 1
const int ledPinMouse2 = 9; // mouse press LED 2
const int ledPinMouse3 = 10; // mouse press LED 3
const int ledPinMouse4 = 11; // mouse press LED 4
const int buttonPin = 12; // left mouse button, connect button to this and ground

//for averaging the values from the MPU6050, so the directional LED's won't blink  
const int numReadings = 10;
int readingsX[numReadings];      // the readings from the analog input
int readIndexX = 0;              // the index of the current reading
int totalX = 0;                  // the running total
int averageX = 0;                // the average
int readingsY[numReadings];      // the readings from the analog input
int readIndexY = 0;              // the index of the current reading
int totalY = 0;                  // the running total
int averageY = 0;                // the average
int led_L = 0;
int led_R = 0;
int led_U = 0;
int led_D = 0;
int led_M = 0;

SensorToButton mouseButton(buttonPin, 50);

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
int vx, vy;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(ledPinLeft, OUTPUT);  // sets the pin as output
  pinMode(ledPinRight, OUTPUT);  // sets the pin as output
  pinMode(ledPinUp, OUTPUT);  // sets the pin as output
  pinMode(ledPinDown, OUTPUT);  // sets the pin as output
  pinMode(ledPinMouse1, OUTPUT);  // sets the pin as output
  pinMode(ledPinMouse2, OUTPUT);  // sets the pin as output
  pinMode(ledPinMouse3, OUTPUT);  // sets the pin as output
  pinMode(ledPinMouse4, OUTPUT);  // sets the pin as output
  mpu.initialize();
  if (!mpu.testConnection()) { while (1); }

  for (int thisReadingX = 0; thisReadingX < numReadings; thisReadingX++) {
  readingsX[thisReadingX] = 0;
  }
  for (int thisReadingY = 0; thisReadingY < numReadings; thisReadingY++) {
  readingsY[thisReadingY] = 0;
  }
  //delay(40000);
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  //Serial.print(ax);
  //vx = (ay)/150;  
  //vy = (ax)/150;


  // The following values might need to be changed depending on hardware used.
  if (ay<-550) {
    vx = ay/102; 
  }
  else if (ay>-551) {
    vx = ay/128; 
  }
  if (ax>1020) {
    vy = ax/170; 
  }
  else if (ax<1021) {
    vy = ax/100; 
  }
  vx = vx + 6;
  vy = vy - 7;

  if (vx<5 && vx>-5) {
    vx = 0;  
  }
  else;
  if (vy<5 && vy>-5) {
    vy = 0;  
  }
  else;

  
  mouseButton.read();
  if (mouseButton.isPressed()) {
    Mouse.press();
    digitalWrite(ledPinMouse1, HIGH);
    digitalWrite(ledPinMouse2, HIGH);
    digitalWrite(ledPinMouse3, HIGH);
    digitalWrite(ledPinMouse4, HIGH);
    led_M = 1;        
  }
  else {
    Mouse.release();
    digitalWrite(ledPinMouse1, LOW);
    digitalWrite(ledPinMouse2, LOW);
    digitalWrite(ledPinMouse3, LOW);
    digitalWrite(ledPinMouse4, LOW);
    led_M = 0;         
  }
  if (mouseButton.wasPressed()) {
    #ifdef DEBUG
    Serial.print("MOUSE_PRESSED, ");
    #endif
  }
  if (mouseButton.wasReleased()) {
    #ifdef DEBUG
    Serial.print("MOUSE_RELEASED, ");
    #endif
  }

  // The following averages the readings to produce a stable direction LED output. 
  totalX = totalX - readingsX[readIndexX];
  readingsX[readIndexX] = vx;
  totalX = totalX + readingsX[readIndexX];
  readIndexX = readIndexX + 1;

  if (readIndexX >= numReadings) {
    readIndexX = 0;
  }
  averageX = totalX / numReadings;

  totalY = totalY - readingsY[readIndexY];
  readingsY[readIndexY] = vy;
  totalY = totalY + readingsY[readIndexY];
  readIndexY = readIndexY + 1;

  if (readIndexY >= numReadings) {
    readIndexY = 0;
  }
  averageY = totalY / numReadings;
 
  if (averageX<-5) {
    digitalWrite(ledPinLeft, HIGH);
    digitalWrite(ledPinRight, LOW);
    led_L = 1;
    led_R = 0;  
  }
  else if (averageX>5) {
    digitalWrite(ledPinLeft, LOW);
    digitalWrite(ledPinRight, HIGH);
    led_L = 0; 
    led_R = 1; 
  }
  else {
    digitalWrite(ledPinLeft, LOW);
    digitalWrite(ledPinRight, LOW);
    led_L = 0; 
    led_R = 0; 
  }

  if (averageY<-5) {
    digitalWrite(ledPinUp, LOW);
    digitalWrite(ledPinDown, HIGH);
    led_U = 0; 
    led_D = 1; 
  }
  else if (averageY>5) {
    digitalWrite(ledPinUp, HIGH);
    digitalWrite(ledPinDown, LOW);
    led_U = 1; 
    led_D = 0; 
  }
  else {
    digitalWrite(ledPinUp, LOW);
    digitalWrite(ledPinDown, LOW);
    led_U = 0; 
    led_D = 0; 
  }

  Mouse.move(vx, vy);
  
  #ifdef DEBUG
  Serial.print("ax = ");
  Serial.print(ax);
  Serial.print(" | ay = ");
  Serial.print(ay);
  Serial.print("    | vx = ");
  Serial.print(vx);
  Serial.print(" | vy = ");
  Serial.print(vy);
  Serial.print("    | avX = ");
  Serial.print(averageX);
  Serial.print(" | avY = ");
  Serial.print(averageY);
//  Serial.print(" | Left = ");
//  Serial.print(led_L);
//  Serial.print(" | Right = ");
//  Serial.print(led_R);
//  Serial.print(" | Up = ");
//  Serial.print(led_U);
//  Serial.print(" | Down = ");
//  Serial.print(led_D);
//  Serial.print(" | Mouse = ");
//  Serial.println(led_M);
  Serial.println();
  #endif
  
  delay(30);
}
