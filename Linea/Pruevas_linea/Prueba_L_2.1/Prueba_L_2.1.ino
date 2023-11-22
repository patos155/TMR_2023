//----------------------------LIBRERIAS----------------------------//
#include <QTRSensors.h>
#include <Servo.h>
#define BOTON    3
#include <EEPROM.h>
QTRSensors qtr;
#include <Wire.h>
#include <SD.h>
#include <Pixy2.h>
Pixy2 pixy;

// Variables y pines para el sonsor infrarojo
// 600-700
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
unsigned int Negro[8];
unsigned int Blanco[8];
int umbral[8];
unsigned int digital[8];
unsigned int diferencia[8];
int LP[8];
int linea = 1;
boolean ok = true;

//Servos
Servo mid;
Servo mit;
Servo mdd;
Servo mdt;

//Pines de servomotores
const int pmdd=22;//43
const int pmdt=24;//45
const int pmit=26;
const int pmid=28;

// Velocidad servo
//Adelante
  int VDMax = 180;
  int VDMed = 145;
  int VDMin = 110;
//Detenerse
  int VStop = 90;
//Atras
  int VBMin = 70;
  int VBMed = 35;
  int VBMax = 0;

//Leds Indicadores
const int ledBlue= 8;
const int ledRed= 9;
 int anteriorError = 0;
/*
 * Calibra el sensor para la lectura de la linea 
 * Calibra 20 veses en blanco y negro y saca el promedio
 */
void calibrate()
{
  digitalWrite(ledBlue,LOW);
  digitalWrite(ledRed,HIGH);
  delay(1000);
  Serial.println("Negro");
  // Calibrar valor negro
  while(digitalRead(BOTON) == HIGH);
  for (int i = 0; i < 20; i++)
  {
    negro();
    Serial.println("\t");
    delay(2);
    digitalWrite(ledBlue,LOW);
    digitalWrite(ledRed,LOW);
    delay(2);
  }
  digitalWrite(ledBlue,HIGH);
  digitalWrite(ledRed,LOW);
  delay(1000);
  Serial.println("Blanco");
  // Calibrar valor blanco
  while(digitalRead(BOTON) == HIGH);
  for (int i = 0; i < 20; i++)
  {
    blanco();
    Serial.println("\t");
    delay(2);
    digitalWrite(ledBlue,LOW);
    digitalWrite(ledRed,LOW);
    delay(2);
  }
digitalWrite(ledBlue,LOW);
digitalWrite(ledRed,LOW);
delay(1000);
promedio();
Serial.println("");

}

void setup()
{
  Serial.begin(9600);
  mid.attach(pmid);
  mit.attach(pmit);
  mdd.attach(pmdd);
  mdt.attach(pmdt);
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A8, A9, A10, A11, A12, A13, A14, A15}, SensorCount);
  qtr.setEmitterPin(40);
  pinMode(BOTON, INPUT_PULLUP);
  //pixy.init();
   pinMode(ledBlue, OUTPUT);
   pinMode(ledRed, OUTPUT);
  eeprom();  
}

void promedio(){
  for(int i=0;i<8;i++){
  diferencia[i]=(Negro[i]-Blanco[i])/2;
  umbral[i]=Negro[i]-diferencia[i];  
}
  for(int i=0;i<8;i++){
  Serial.print(diferencia[i]);
  Serial.print("\t");
  }
}
void blanco(){
  for(int i=0;i<8;i++){
     qtr.read(sensorValues);
    Serial.print(sensorValues[i]);
    Serial.print('\t');
    Blanco[i] =  (sensorValues[i]);
  }
}
void negro(){
    for(int i=0;i<8;i++){
     qtr.read(sensorValues);
     Serial.print(sensorValues[i]);
     Serial.print('\t');
     Negro[i] =    (sensorValues[i]);
    }
}

void eeprom(){
  Serial.println("");
  digitalWrite(ledBlue,LOW);
  digitalWrite(ledRed,LOW);
  delay(2000);
    if (digitalRead(BOTON) == LOW) {
      digitalWrite(ledBlue,LOW);
      digitalWrite(ledRed,HIGH);
      delay(1000);
      calibrate();
      for(int i=0;i<8;i++){
      Serial.print(umbral[i]);
      Serial.print("\t");
      }
    Serial.println("");      
      for(int i=0;i<8;i++){
      LP[i]= umbral[i];
      Serial.print(LP[i]);
      Serial.print("\t");
      }      

    Serial.println("");      
    int LO[8];
    for(int i=0;i<8;i++){
      EEPROM.put(i * sizeof(int), umbral[i]);
      EEPROM.get(i * sizeof(int), LO[i]);
      Serial.print(LO[i]); 
      Serial.print("\t");
    }

      delay(2000);
    Serial.println("");
      } else {
     digitalWrite(ledBlue,HIGH);
     digitalWrite(ledRed,LOW);
     delay(1000);
     Serial.println("Valores anteriores:");
     int PL[8];
     for(int i=0;i<8;i++){
     EEPROM.get(i * sizeof(int), umbral[i]);
     Serial.print(umbral[i]);
     Serial.print("\t");
     
     }
     Serial.println("");
      delay(1000);
  }
}

void loop(){
//Moverse (VBMax,VDMax);
lectura();
direccion();

}

void Moverse(int VI, int VD){
  // Izquierdos 
  int VID = 90;
  if (VI == 180){
    VID = 0;
  }else if (VI == 145){
    VID = 35;
  }else if (VI == 110){
    VID = 70;
  }else if (VI == 70){
   VID = 110;
  }else if( VI == 35){
    VID = 145;
  }else if (VI == 0){
    VID = 180;
  }
  
  int i1 = VID;  
  int i2 = VID;
  // Derechos
  int d1 = VD;
  int d2 = VD;
  mid.write(i1);
  mit.write(i2);
  mdd.write(d1);
  mdt.write(d2); 
}
void lectura(){
  qtr.read(sensorValues);
Serial.print("  ");
  for (uint8_t i = 0; i < SensorCount; i++)
  {
  Serial.print("  ");
  if(sensorValues[i]<=umbral[i]){digital[i]=0;}else{digital[i]=1;}
  Serial.print(digital[i]);
  Serial.print("");

  }
Serial.println("");
}


void direccion (){  
/*
// digital[0] digital[1] digital[2] digital[3] digital[4] digital[5] digital[6] digital[7]
*/
//Centrado
if (
    //-BBNNBB-
    (digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0)||
    //-BNNNBB-
    (digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0)||
    //-BBNNNB-
    (digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0)){
    Moverse(VDMax,VDMax);
    anteriorError = 3;
    Serial.println("centro");
}else 
  /*Derecha
  Un poco desviado
  */
  if(
    //-BBBNNB-
    (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0)||
    //-BBBNNN-
    (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1)){
    Moverse(VDMax,VDMin);
    anteriorError = 5;
}else
  /*Izquierda
  Un poco desviado
  */
  if(
    //-BNNBBB-
    (digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0)||
    //-NNNBBB-
    (digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0)){
    Moverse(VDMin,VDMax);
    anteriorError = 2;
}else
  /*Derecha
  Desviado
  */
  if(
    //-BBBBNN-
    (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 1 && digital[6] == 1)||
    //--BBBNNN
    (digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1)||
    //--BBBBNN
    (digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0 && digital[6] == 1 && digital[7] == 1)){
    Moverse(VDMax,VBMed);
    anteriorError = 6;
}else
  /*Izquierda
  Desviado
  */
  if(
    //-NNBBBB-
    (digital[1] == 1 && digital[2] == 1 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0)||
    //NNNBBB--
    (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0)||
    //NNBBBB--
    (digital[0] == 1 && digital[1] == 1 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0)){
    Moverse(VBMed,VDMax);
    anteriorError = 1;
}else
  /*Derecha
  Vueltas 90 grados
  */
  if(
    //--BBNNNN
    //(digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1)||
    //--BNNNNN
    (digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1)){
    Moverse(VDMax,VDMax);
    delay(600);
    Moverse(VDMax,VDMed);
    delay(500); 
    
    anteriorError = 7;
    Serial.print("derech");
}else
  /*Izquierda
  Vueltas a 90 grados
  */
  if(
    //NNNNBB--
    //(digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0)||
    //NNNNNB--
    (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0)){
    Moverse(VDMax,VDMax);
    delay(600);
    Moverse(VDMed,VDMax);
    delay(500);      
    anteriorError = 0;
    Serial.print("izq");
    }else
    // anteriorError == 0,1,2 izquierda
    // anteriorError == 5,6,7 derecha
    if (anteriorError == 5 or anteriorError == 6 or anteriorError == 7){
        //Moverse(VDMax,VDMax);
        //delay(200);
        Moverse(VDMed,VBMed);
    }else
    if(anteriorError == 2 or anteriorError == 1 or anteriorError == 0){
      //Moverse(VDMax,VDMax);
      //delay(200);
      Moverse(VBMed,VDMed);
    }/*else
    // anteriorError = 0 centro
    if(anteriorError = 0){
      Moverse(VDMax,VDMax);

}*/
}
