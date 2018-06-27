#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>



Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
int group_num;

//Use microseconds to set PWM pulses
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  //Serial.print(pulselength); Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  //Serial.print(pulselength); Serial.println(" us per bit");
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  //Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
};

//Class to characterize the syringe groups by identifying what channels are within a group and functions to set their states
class SyringeGroups{
  public:
    int i;
    int pins[16];
    void off(){
      for (i = 0; i < 16; i++){
        setServoPulse(pins[i],0.0008);
      };
      i = 0;
    };
    void oneeighty(){
      for(i = 0; i < 16; i++){
        setServoPulse(pins[i],0.0022);
      };
      };
    void on(){
      for(i = 0; i < 16; i++){
        setServoPulse(pins[i],0.0014);
      };
      };
};

void setup() {
  Serial.begin(9600); //set Baud Rate
  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  delay(10);
}
void loop() {

  // Ask user for number of control syringes they are using
  Serial.print("Enter how many syringe groups you will be using for TERRA: \n **You can have groups with 1 or more syringes** \n");
  while(Serial.available() == 0){};
  group_num = Serial.read() - 48;
  Serial.println(group_num);

  int k;
  int j;
  char pin_list[40];
  String channel_list;
  SyringeGroups groups[group_num];

  //Give each syringe group object the channel list that it will control
  if(k == 0){
    for (k = 0; k < group_num; k++){
      Serial.print("Enter which channels on the Adafruit Motor Shield are going to be used for Group");
      Serial.println(k + 1);
      Serial.println("** Seperate channels using a space **");
      while(Serial.available() == 0) {};
      channel_list = Serial.readString();
      channel_list.toCharArray(pin_list,40);

      char *token = strtok(pin_list," ");
      while(token != NULL){
        groups[k].pins[j] = atoi(token);
        Serial.println(groups[k].pins[j]);
        token = strtok (NULL, " ");
        j++;
        };
      j = 0;
    };
  };

  Serial.println(groups[1].pins[1]);

  //Control the actuation of control syringe groups

  while(1){

    groups[1].on();
    delay(5000);
    groups[1].off();
    delay(5000);




}
}
