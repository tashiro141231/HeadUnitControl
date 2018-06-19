#include "head_unit_control.h"

#define SLAVE_ID 50
#define CTRL_PIN 8
#define BAUDRATE 1000000

HUCtrl slave(SLAVE_ID, CTRL_PIN);

void setup() {
  // put your setup code here, to run once:
  slave.begin(BAUDRATE);
  Serial.begin(BAUDRATE);
}

void loop() {
  // put your main code here, to run repeatedly:
  int num = slave.poll();
  if(num != 0){
    Serial.print("unit ID: ");
    Serial.println(num);
  }
  else {
    //Serial.println("Not received");
  }
}
