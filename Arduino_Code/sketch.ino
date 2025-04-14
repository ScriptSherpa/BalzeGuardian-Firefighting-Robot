//FIREROVER //
//*------ Arduino Fire Fighting Robot Code----- *//
//By ~ RHYTHM SHAH //
#include <NewPing.h>
#include <Servo.h>
#include <SoftwareSerial.h>
//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(14, 15); //SIM800L Tx & Rx is connected to Arduino #14 & #15
Servo myservo;
Servo servo;
int pos = 0;    
boolean fire = false;
/*-------defining Inputs------*/
#define Left_S 2      // left sensor
#define Right_S 3     // right sensor
#define Forward_S 4  //forward sensor
#define TRIGGER_PIN A2
#define ECHO_PIN A3
/*-------defining Outputs------*/
#define IN1 9       // left motor
#define IN2 10       // left motor
#define IN3 11      // right motor
#define IN4 12       // right motor
#define pump 5
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
#define max_distance 50
NewPing sonar(TRIGGER_PIN, ECHO_PIN, max_distance);
int distance = 0;
int leftDistance;
int rightDistance;
boolean object;
void setup()
{
  randomSeed(analogRead(0));
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
 //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  Serial.println("Initializing...");
  pinMode(Left_S, INPUT);
  pinMode(Right_S, INPUT);
  pinMode(Forward_S, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(pump, OUTPUT);
  myservo.attach(6);
  myservo.write(90);
  servo.attach(7);
  servo.write(0);
}
void put_off_fire()
{
    delay (500);
    stop() ; delay(10);
   digitalWrite(pump, HIGH);  delay (500);
   for (pos = 50; pos <= 130; pos += 1)
   {
    myservo.write(pos);
    delay(10);  
  }
  for (pos = 130; pos >= 50; pos -= 1) {
    myservo.write(pos);
    delay(10);
  }
 digitalWrite(pump, LOW);
  myservo.write(90);
  fire=false;
}
void loop()
{
   myservo.write(90); //Sweep_Servo();
    // read the sensor on analog A0:
 int sensorReading = analogRead(A1); int range = map(sensorReading, sensorMin, sensorMax, 0, 3);  
    if (digitalRead(Left_S) ==1 && digitalRead(Right_S)==1 && digitalRead(Forward_S) ==1) //If Fire not detected all sensors are zero
    {
    //Do not move the robot
    stop() ;
    }
    else if (digitalRead(Forward_S) ==0) //If Fire is straight ahead
    {
      switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    Serial.println("** Close Fire **");
    fire = true;
    break;
  case 1:    // A fire between 1-3 feet away.
    Serial.println("** Distant Fire **");
    objectAvoid();
    moveForward(); //Move the robot forward
    break;
   sendSMS();
   Serial.println("message sent.");
   delay(1000);
    }
    else if (digitalRead(Left_S) ==0) //If Fire is to the left
    {
    //Move the robot left
    objectAvoid();
    moveLeft();
    sendSMS();
   Serial.println("message sent.");
   delay(1000);
    }
    else if (digitalRead(Right_S) ==0) //If Fire is to the right
    {
    //Move the robot right
    objectAvoid();
    moveRight();
    sendSMS();
   Serial.println("message sent.");
   delay(1000);
    }
delay(300); //Slow down the speed of robot
     while (fire == true)
     {
      put_off_fire();
     }
}
void objectAvoid() {
  distance = getDistance();
  if (distance <= 15) {
    //stop
    stop();
    lookLeft();
    lookRight();
    delay(100);
    if (rightDistance <= leftDistance) {
      //left
      object = true;
      turn();
    } else {
      //right
      object = false;
      turn();
    }
    delay(100);
  }
  else {
    //forword
    moveForward();
  }
}
int getDistance() {
  delay(50);
  int cm = sonar.ping_cm();
  if (cm == 0) {
    cm = 100;
  }
  return cm;
}
int lookLeft () {
  //look left
  servo.write(150);
  delay(500);
  leftDistance = getDistance();
  delay(100);
  servo.write(90);
  return leftDistance;
  delay(100);
}
int lookRight() {
  //look right
  servo.write(30);
  delay(500);
  rightDistance = getDistance();
  delay(100);
  servo.write(90);
  return rightDistance;
  delay(100);
}
void turn() {
  if (object == false) {
    moveLeft();
    delay(700);
    moveForward();
    delay(800);
    moveRight();
    delay(900);
    if (digitalRead(Right_S) ==0) {
      loop();
    } else {
      moveForward();
    }
  }
  else {
    moveRight();
    delay(700);
    moveForward();
    delay(800);
    moveLeft();
    delay(900);
    if (digitalRead(Left_S) ==0) {
      loop();
    } else {
      moveForward();
    }
  }
}
void moveRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void moveLeft() {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
}
void stop() {
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, HIGH);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, HIGH);
}
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void sendSMS()
{
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("FIRE DETECTED !!"); //text content
  updateSerial();
  mySerial.write(26);
}
void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available())
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}