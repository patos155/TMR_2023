#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#define Type DHT11

String nom = "Arduino";
String msg;
String continuar = "0";
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

struct packageE
{
  float temperatura;
  int gas1;
  int gas2;
  int gas3;
  //int gas4;
};

struct packageR
{
  int valX;
  int valY;
  int valbtnI;
  int valbtnD;
};

typedef struct packageR PackageR;
typedef struct packageE PackageE;
PackageR dataR;
PackageE dataE;

//int pos = 0;
int sensorg1 = 5;
int sensorg1a = A0;
int sensorg2 = 4;
int sensorg2a = A1;
int sensorg3 = 3;
int sensorg3a = A2;
int stemp = 2;

DHT HT(stemp, Type);
int humedad;
float tempC;
float tempF;
int gas1a, gas2a, gas3a;



void setup() {
  Serial.begin(9600);
  HT.begin();
  pinMode(sensorg1, INPUT);
  pinMode(sensorg2, INPUT);
  pinMode(sensorg3, INPUT);
}

void loop() {
  readSerialPort();
  if (msg=="1"){
    continuar ="1";
  }
  if(continuar !="1"){
    dataE.gas1 = digitalRead(sensorg1);
    dataE.gas2 = digitalRead(sensorg2);
    dataE.gas3 = digitalRead(sensorg3);
    gas1a = analogRead(sensorg1a);
    gas2a = analogRead(sensorg2a);
    gas3a = analogRead(sensorg3a);

    humedad = HT.readHumidity();
    tempC = HT.readTemperature();
    tempF = HT.readTemperature(true);

    EnviarDatos();
  }
}

void EnviarDatos()
{
  Serial.print(dataE.gas1);
  Serial.print("x");
  Serial.print(gas1a);
  Serial.print("x");
  Serial.print(dataE.gas2);
  Serial.print("x");
  Serial.print(gas2a);
  Serial.print("x");
  Serial.print(dataE.gas3);
  Serial.print("x");
  Serial.print(gas3a);
  Serial.print("x");
  Serial.print(humedad);
  Serial.print("x");
  Serial.println(tempC);
  delay(500);
}

void readSerialPort() {
 	msg = "";
 	if (Serial.available()) {
 			delay(10);
 			while (Serial.available() > 0) {
 					msg += (char)Serial.read();
 			}
 			Serial.flush();
 	}
}