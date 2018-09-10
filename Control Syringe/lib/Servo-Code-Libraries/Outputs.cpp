#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Outputs.h"
#include <ArduinoSTL.h>


void Outputs::assign_open(int connection) {

  Open.pins[counter_open] = connection;
  counter_open = counter_open++;
  };

void Outputs::assign_close(int connection){
  Close.pins[counter_close] = connection;
  counter_close = counter_close++;
};

void Outputs::assign_coordinates(){
  // generating input vector for LOCATIONS - (can add a confirmation section)

       String input;
       char input_list[100];

       Serial.println("Enter numbers corresponding to well plate locations seperated by spaces, press enter once done: ");
       while(Serial.available() == 0){};

       input = Serial.readString();                        // read the input locations as a string
       input.toCharArray(input_list, 100);                 // take input string and store in a character array

  // parse through character array and
       char * token = strtok (input_list," ");
       while (token != NULL) {
         coordinates.push_back(atoi(token));
         Serial.print ("this is token: ");
         Serial.println (token);
         token = strtok (NULL, " ");
         Serial.print ("this is token after strtok: ");
         Serial.println (token);
       }

       std::sort(coordinates.begin(), coordinates.end(), comp);
       write_vector(coordinates); // for each output
};


void Outputs::write_vector(const std::vector<int> & V){
  Serial.println ("The well plate locations are: ");
 for(int i=0; i < V.size(); i++)
   Serial.print (V[i]);
   Serial.println (" ");
   Serial.println (V.size());
}

bool Outputs::comp(const int& num1, const int& num2) {
   return num1 < num2;
}

void Outputs::origin(){
  Open.neutral();
  Close.neutral();
}

void Outputs::open(){
    Open.on();
    //delay(2000);
    Close.off();
};

void Outputs::close(){
    Close.on();
    //delay(2000);
    Open.off();
};
