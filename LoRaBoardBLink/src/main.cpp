#include <Arduino.h>
#define RF_mosi 17
#define RF_miso 16
#define RF_SCK 18

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(115200);
  pinMode(RF_mosi, OUTPUT);
  pinMode(RF_miso, OUTPUT);
  pinMode(RF_SCK, OUTPUT);
}



void loop() {
  Serial.println("Testest");
  
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}