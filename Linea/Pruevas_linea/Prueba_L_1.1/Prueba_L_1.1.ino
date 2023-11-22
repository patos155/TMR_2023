// Librerias usadas
#include <QTRSensors.h>
#include <Servo.h>
#define BOTON    3
#define AI  A4
#include <EEPROM.h>
QTRSensors qtr;
#include <Wire.h>
#include <SD.h>
#include <Pixy2.h>
Pixy2 pixy;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

unsigned int Negro[8][10];
unsigned int NegroMay[8];
unsigned int NegroMen[8];
unsigned int digital[8];

void negro() {
  for (int b = 0; b < 10; b++) {
    qtr.read(sensorValues);
    for (int i = 0; i < SensorCount; i++) {
      Serial.print(sensorValues[i]);
      Serial.print('\t');
      Negro[i][b] = sensorValues[i];
    }
    Serial.println();
  }
  
  for (int i = 0; i < SensorCount; i++) {
    NegroMay[i] = Negro[i][0];
    NegroMen[i] = Negro[i][0];
    for (int b = 1; b < 10; b++) {
      if (Negro[i][b] > NegroMay[i]) {
        NegroMay[i] = Negro[i][b];
      }
      if (Negro[i][b] < NegroMen[i]) {
        NegroMen[i] = Negro[i][b];
      }
    }
  }

  Serial.println("Mayores");
  for (int i = 0; i < SensorCount; i++) {
    Serial.print(NegroMay[i]);
    Serial.print('\t');
  }
  Serial.println();

  Serial.println("Menores");
  for (int i = 0; i < SensorCount; i++) {
    Serial.print(NegroMen[i]);
    Serial.print('\t');
  }
  Serial.println();
}

void calibrate() {
  Serial.println("Negro");
  // Calibrar valor negro
  while (digitalRead(BOTON) == HIGH);
  negro();
  Serial.println("");
  while (digitalRead(BOTON) == HIGH);
}

void setup() {
  Serial.begin(9600);

  //Configura los pines de infrarojo
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A8, A9, A10, A11, A12, A13, A14, A15}, SensorCount);
  qtr.setEmitterPin(40);
  //Define pines para el boton de calibracion, leds , y ultrasonicos
  pinMode(BOTON, INPUT_PULLUP);
  calibrate();
}

// Leer la lectura de los sensores
void lectura(){
  // Lee valores de sensores 
  qtr.read(sensorValues);
Serial.print("  ");
  for (uint8_t i = 0; i < SensorCount; i++)
  {
  Serial.print("  ");
  if(sensorValues[i]>=NegroMen[i] && sensorValues[i]<=NegroMay[i]){digital[i]=1;}else{digital[i]=0;}
  Serial.print(digital[i]);
  Serial.print("");

  }
 for (uint8_t i = 0; i < SensorCount; i++)
  {
  Serial.print("  ");
  Serial.print(sensorValues[i]);
  Serial.print("");

  }
}

void loop() {
    Serial.println("");
  lectura();


}
