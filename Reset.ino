#include <ArduinoSTL.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "SyringeGroups.h"
#include "Outputs.h"


// Important Definitions
#define pinEnable    4  // PWM or Enable pins
#define pinStep      3  // Step 3
#define pinDir       2  // Direction 2
#define pinEnable_2  13 // PWM or enable pins
#define pinStep_2    9  // Step 92
#define pinDir_2     8  // Direction  8
#define limSwitch_1  7  // first Limit Switch

//~~~~~~~~~~ Microstepping Features ~~~~~~~~~~//
#define microStep1  5   // mode select for microstepping
#define microStep2  10  // mode select for microstepping


//Syringe Variables
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
char incomingData = 0;
char incomingData_r = 0;
int outputNum = 0;
int reset = 0;
int dispense_time = 0;
int flush = 0;
int disp_flush[2] = {0,0};
int disp_flush_counter = 0;


void setup(){
  Serial.begin(1152000); // set baud rate for communication
  //Ezira code
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pinMode(LED_BUILTIN, OUTPUT);
  //Ezira code

  delay(10);

  // OUTPUT declarations for Stepper 1
  pinMode( pinEnable,   OUTPUT );
  pinMode( pinDir   ,   OUTPUT );
  pinMode( pinStep  ,   OUTPUT );

  // OUTPUT declarations for Stepper 2
  pinMode( pinEnable_2, OUTPUT );
  pinMode( pinDir_2   , OUTPUT );
  pinMode( pinStep_2  , OUTPUT );

  //Microstepping Output

  pinMode (microStep1, OUTPUT);
  pinMode (microStep2, OUTPUT);
  digitalWrite (microStep1, HIGH);
  digitalWrite (microStep2, HIGH);

  // Limit Switch initialization

  digitalWrite (limSwitch_1, HIGH);


}

void loop(){
  //  Ezira code
  //  Ask user for number of outputs their microfluidic chip has
  while(Serial.available() == 0){};

  if (Serial.available() > 0) {
    incomingData = 0;
    while(1) {
      incomingData = Serial.read();

      if (incomingData == 'r'){
        reset = 1;
        while(Serial.available() == 0){};
        if (Serial.available() > 0) {
          incomingData_r = 0;
          while(1) {
            incomingData_r = Serial.read();
            if (incomingData_r == '\n') break;
            if (incomingData_r == -1) continue;
            outputNum *= 10;
            outputNum = ((incomingData_r - 48) + outputNum);
          };
        };
      } else {
          if (incomingData == '\n') break;
          if (incomingData == -1) continue;
          outputNum *= 10;
          outputNum = ((incomingData - 48) + outputNum);
      };
    };
  };

  Outputs outputs[outputNum];
  const int openPins[] = {0,2,4,6,8,10,12,14};
  const int closePins[] = {1,3,5,7,9,11,13,15};

//  Establish pins sequence of open and closed valves for each output
  for (int k = 0; k < outputNum; k++){
    outputs[k].assign_open(openPins[k]);
    outputs[k].assign_close(closePins[k]);
    if (reset == 1){
      outputs[k].origin();
      };
    outputs[k].assign_coordinates();
  };

  for (int k = 0; k < outputNum; k++){
    outputs[k].close();
  };

  while(Serial.available() == 0){};
  if (Serial.available() > 0) {
    String dispenseFR;
    char dispenseFR_list[100];
    while(Serial.available() == 0){};
    dispenseFR = Serial.readString();
    dispenseFR.toCharArray(dispenseFR_list, 100);

    char *token = strtok (dispenseFR_list," ");
    while (token != NULL) {
      disp_flush[disp_flush_counter] = atoi(token);
      disp_flush_counter++;
      token = strtok (NULL, " ");
    };
  };

  dispense_time = disp_flush[0];
  flush = disp_flush[1];








 // ~~~ VARIABLES FOR XY-PLANE ~~~ //

   // std::vector<int> V;
    int wellPlate [8][12];
    int n,m = 0;
    int order = 1;

// generating array for well-plate location

  Serial.println ("This is the array for a 96 well plate:");
  Serial.print ("[") ;

    for (n=0; n < 8; n++) {
      for (m=0; m < 12; m++){
        wellPlate[n][m] = order;
        order++;
        Serial.print (wellPlate[n][m]);
        Serial.print (" ");
      }
      if (n<7)
      Serial.println();
    }
    Serial.println("]");


// TRANSLATION loop starts here //


// for loop that goes through how many outputs you have
for (int outputIterator = 0; outputIterator < outputNum; outputIterator++){


  // ~~~ HOMING SEQUENCE ~~~ //
  int ivy = 0;


  digitalWrite( pinDir_2   ,HIGH); // Direction control
  digitalWrite( pinStep_2  ,LOW);  // initialize it to be not moving
  digitalWrite( pinDir   , LOW); // Direction control of motor 2
  digitalWrite( pinStep  , LOW);  // initialize motor 2 to be not moving

  for (ivy=0; ivy<(800*4); ivy++){
      if ( digitalRead(7) == LOW){
        break;
      }
          Serial.println(ivy);
          digitalWrite( pinStep, HIGH);
          digitalWrite( pinStep_2, HIGH);
          delay(1);
          digitalWrite(pinStep, LOW);
          digitalWrite(pinStep_2, LOW);
          delay(1);
  };

  delay(flush*1000);


int Xbefore = 0;
int Ybefore = 0;
int Xnow = 0;
int Ynow = 0;
int i = 0;  // movement iterator

    // assuming XY plane is at home, visit each location
for (int size = 1; size < (outputs[outputIterator].coordinates.size() + 1); size++){

      for (n = 0; n < 8; n++) { // n
        for (m = 0; m < 12; m++){ // m

           if  (outputs[outputIterator].coordinates[size - 1] == wellPlate[n][m] ){


            digitalWrite (microStep1, HIGH);
            digitalWrite (microStep2, HIGH);

            Serial.print ("The m and n values are ");
            Serial.print (m);
            Serial.print (" ");
            Serial.print (n);
            Serial.print (".");
            Serial.println ();


            Serial.print ("The Xbefore and Ybefore values are ");
            Serial.print (Xbefore);
            Serial.print (" ");
            Serial.print (Ybefore);
            Serial.print (".");
            Serial.println ();

             Ynow = n - Ybefore; // calculating how much needed to move in Y
             Xnow = m - Xbefore; // calculating how much needed to move in X

             if (Xnow < 0){
              digitalWrite(pinDir,LOW);
              Xnow = Xnow * (-1);
            }
            else {
              digitalWrite(pinDir,HIGH);
            }

            if (Ynow < 0){
             digitalWrite(pinDir_2,HIGH);
             Ynow = Ynow * (-1);
           }
           else {
             digitalWrite(pinDir_2,LOW);
           }

            for (i = 0; i < ((48.5*4)*Xnow); i++){
              digitalWrite (pinStep, HIGH);
              delay(10);
              digitalWrite (pinStep,LOW);
              delay(10);
            }

            for (i = 0; i < ((48.5*4)*Ynow); i++){
              digitalWrite (pinStep_2, HIGH);
              delay(10);
              digitalWrite (pinStep_2,LOW);
              delay(10);
            }

            Serial.print ("The plane has moved ");
            Serial.print (Xnow);
            Serial.print (" ");
            Serial.print (Ynow);
            Serial.print (".");
            Serial.println ();

            delay (2500);

            // open syringes - output is released
            outputs[outputIterator].open();

            delay(dispense_time*1000); // dispense time (can be dictated by flowrate)

            // close valves, all fluids go to waste
            outputs[outputIterator].close();
            delay (2500);

            // setting the locations to current location
            Ybefore = n;
            Xbefore = m;

           }
        }
      }
    }
  }


while (true);

}