//----------------------------LIBRERIAS----------------------------//
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
  
//ultrA 1
int pinEcho1 = 5;
int pinTrig1 = 4;

//ultrA 2
int pinEcho2 = 37;
int pinTrig2 = 35;
int distance2= 0;
boolean bucle=true;

//Variables del color verde
int Verde = 0;
int cont = 0;
int signature, x, y, width, height;
float cx, cy, area;
// Constantes del control PID
const float kp = 0.1;//0.1
const float ki = 0.1;//0.1
const float kd = 0.05;//0.001
int setpoint = 300;
int setpoint1 = 400;
// Variables del control PID
int p, i, d;
int p1, i1, d1;
int sumap, suma, pos, error, error_A;
int sumap1, suma1, pos1, error1, error_A1 ;

int x0, y0, x1, y1, iy, ix;
int Anve = 0;

/*
 * Calibra el sensor para la lectura de la linea 
 * Calibra 20 veses en blanco y negro y saca el promedio
 */
void calibrate()
{
  Serial.println("Negro");
  // Calibrar valor negro
  while(digitalRead(BOTON) == HIGH);
  for (int i = 0; i < 20; i++)
  {
    negro();
    Serial.println("\t");
    delay(2);
    delay(2);
  }

  Serial.println("Blanco");
  // Calibrar valor blanco
  while(digitalRead(BOTON) == HIGH);
  for (int i = 0; i < 20; i++)
  {
    blanco();
    Serial.println("\t");
    delay(2);
    delay(2);
  }

promedio();
Serial.println("");
while(digitalRead(BOTON) == HIGH);
}


/*
 * -------------------------------------SETUP-------------------------------------
 */
void setup()
{
  Serial.begin(9600);
  mid.attach(pmid);
  mit.attach(pmit);
  mdd.attach(pmdd);
  mdt.attach(pmdt);
  pinMode(pinTrig1, OUTPUT);
  pinMode(pinEcho1, INPUT);
  pinMode(pinTrig2, OUTPUT);
  pinMode(pinEcho2, INPUT);
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A8, A9, A10, A11, A12, A13, A14, A15}, SensorCount);
  qtr.setEmitterPin(40);
  pinMode(BOTON, INPUT_PULLUP);
  //pixy.init();
  eeprom();  
}

/*
 * Guarda valores en la memoria eeprom del arduino 
 * Primero pregunta sobre si hace falta una nueva calibracion 
 * Si es asi calibra el blanco y negro despues saca la nueva lectura y por ultimo la guarda en la memoria
 * De no ser asi carga los valores anteriores de la memoria 
*/
void eeprom(){
  Serial.println("");
  if (digitalRead(BOTON) == LOW) {
    Serial.println("Presione 'e' para escribir un nuevo valor:");
    while (!Serial.available()); 
    Serial.println("");
    char opcion = Serial.read();
    Serial.println("");
    if (opcion == 'e') {
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
      }
    } else {
     Serial.println("Valores anteriores:");
     int PL[8];
     for(int i=0;i<8;i++){
     EEPROM.get(i * sizeof(int), umbral[i]);
     Serial.print(umbral[i]);
     Serial.print("\t");
     
     }
     Serial.println("");
      delay(2000);
  }
}

/*
 * Con los vectores blanco y negro saca el promedio de cada sensor 
 * Saca las lecturas de el sensor y los guarda en los vectores negro y blanco
 */

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
/*
 * -------------------------------------LOOP-------------------------------------
*/
void loop(){
//Moverse (VDMax,VDMax);
lectura();
//verde();
direccion();
//Ultra1();
//ultra();
}

// Acciones para el control y velocidad de los servomotores

void Moverse(int VI, int VD){
  // Izquierdos 
  int i1 = VI;  
  int i2 = VI;
  // Derechos
  int d1 = VD;
  //Funcion especial para el servommotor

  int d2 = VD;
  mid.write(i1);
  mit.write(i2);
  mdd.write(d1);
  mdt.write(d2); 
}

/*
 * Saca las lecturas de los 8 senssores
 * Y los convierte a valores a la variable digital 
 * 0 blanco , 1 negro
 */
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
/*
 * Lee si hay algun parche verde sobre la linea y guarda cuantos parches son
 * Se encarga de verificar si primenramente detecta una interceccion en cruz o en formma de T
 * Si hay una interceccion y detecto anteriormente un parche de color verde
 * Regresa y revisa donde estan ubicados los parches
 * Y si simmplemente no encuentra parches verdes se pasa por alto la interceccion
 */
void verde(){
  bool Negro = false; 

  if (//(digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0  && digital[6] == 0 && digital[7] == 0 )||
      //(digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0  && digital[6] == 0 && digital[7] == 0 )||
     // (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1  && digital[6] == 1 && digital[7] == 1 )||
      //(digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 1  && digital[6] == 1 && digital[7] == 1 )||
      (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1  && digital[6] == 1 && digital[7] == 1 )){
        Negro = true;
      }else{
         Negro = false;
      }
      
  int num_green_blocks = 0;
  int8_t i;
  char buf[128];  
  
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks){
      for (int i = 0; i < pixy.ccc.numBlocks; i++) {
       if (pixy.ccc.blocks[i].m_signature == 1) {  // 1 = verde
       
       int  x = pixy.ccc.blocks[i].m_x;
       int  y = pixy.ccc.blocks[i].m_y;
       if (y < 150 ){
       Anve = num_green_blocks;
        num_green_blocks++;

       } else {
         Serial.print("No Hay ");
       }
      }
     }
    }

    Serial.print(", Parches verdes: ");
    Serial.println(num_green_blocks);
    Serial.println(Anve);
  
    Serial.print(", Parches verdes: ");
    Serial.println(num_green_blocks);
    
  if (Negro == true and Anve  >=1) {
    
    // Si el vector es todo 1s, entonces se detectó negro y reinicia los valores
    Serial.println("Se detectó negro!");
    int Ade=0;
      mid.write(Ade);
      mit.write(Ade);
      mdd.write(Ade);
      mdt.write(Ade); 
      i= 0;
      i1=0;
      pos = 300;
      pos1= 450;
      delay(1500);
      Moverse (VStop, VStop);
      delay(1000);

      int num_green_blocks = 0;
      int8_t i;
      char buf[128];  
      boolean nada= true;
        int blocks = pixy.ccc.getBlocks();
       if (blocks) {
          if (pixy.ccc.numBlocks){
            Serial.print("detecta");
              for (int i = 0; i < pixy.ccc.numBlocks; i++) {
               if (pixy.ccc.blocks[i].m_signature == 1) {  // 1 = verde
              num_green_blocks++;
              Anve = num_green_blocks;
            }
            }
            for (int i = 0; i < pixy.ccc.numBlocks; i++){
              
             if (i == 0){
              x0 = pixy.ccc.blocks[i].m_x;
              y0 = pixy.ccc.blocks[i].m_y;
              }
             if (i == 1){
               x1 = pixy.ccc.blocks[i].m_x;
               y1 = pixy.ccc.blocks[i].m_y;
              } 
            
               ix = pixy.ccc.blocks[0].m_x;
               iy = pixy.ccc.blocks[0].m_y;
            }
            }
              
            } else{
              Serial.print("Nada");
              num_green_blocks = 0;
              nada = false;
              Anve = 0;
       }
       Negro = false;
    Serial.print(x0);   
    if (nada == false) { 
      Serial.print("A");
      // Adelante
     int Ade=180;
      mid.write(Ade);
      mit.write(Ade);
      mdd.write(Ade);
      mdt.write(Ade); 
      delay(1900);
      Anve = 0;
    }else
    if (Anve > 1 and x0 < 160 && x1 > 160 ){
       Serial.print("U");
       delay(1000);
      Moverse (VBMax,VDMax);
       delay(3600);
       Anve = 0;
    } else if (x0 < 160 ){
      Serial.print("I");
      // Izquierda
      Moverse (VDMax, VDMax);
      delay(600);
     Moverse (VBMax, VDMax);
     delay(400);
     Moverse (VDMax, VDMax);
     delay(300);
      Moverse (VDMin, VDMax);
     delay(1800);
           Moverse (VBMin, VDMax);
     delay(1800);
     Anve = 0;
    } else if (x0 > 160 ){
      Serial.print("D");
      // Derecha
     Moverse (VDMax, VDMax);
      delay(600);
     Moverse (VDMax, VBMax);
     delay(400);
     Moverse (VDMax, VDMax);
     delay(300);
     
     Moverse (VDMax, VDMin);
     delay(1800);
     
     Moverse (VDMax, VBMin);
     delay(300);
    }    
  }
}
/*
 * Calcula la posicion de los sensores con relacion a la linea dependeinte entre sensores derechos e izquierdos
 * Saca el control PID respecto a la linea de los sensores derechos e izquierdos
 * Despues se mappea la salida del control PID a los servommotores
 */
void direccion (){  
  //Calculo PID Izquierda
    qtr.read(sensorValues);
    sumap=(400*digital[3]+500*digital[2]+600*digital[1]+700*digital[0]);
    suma=(digital[0]+digital[1]+digital[2]+digital[3]);
    pos=(sumap/suma);
    if(error<=500 && pos==-1){
      pos=400;
    }else if(error>=600 && pos==-1){
      pos=700;
    }
    error=pos;
   // Calculo PID Derecha
    sumap1=(700*digital[7]+600*digital[6]+500*digital[5]+400*digital[4]);
    suma1=(digital[4]+digital[5]+digital[6]+digital[7]);
    pos1=(sumap1/suma1);
    // Serial.print(poslast);
    if(error1<=500 && pos1==-1){
      pos1=400;
    }else if(error1>=600 && pos1==-1){
      pos1=700;
    }
    error1=pos1;
   
   // Pid
   // Izquierdos
   p = pos - setpoint;
   d = p - error_A;
   i += p;
   int PWMi = 0;
    PWMi = (kp*error) + (ki*i) + (kd*d);
   error_A = p;
   // Pid
   // Derechos
   p1 = pos1 - setpoint1;
   d1 = p1 - error_A1;
   i1 += p1;
   int PWMd = 0;
    PWMd = (kp*error1) + (ki*i1) + (kd*d1);
   error_A1 = p1;
   
  // Izquierdos
   if (PWMi >=255){//255
    PWMi =-255;
   }else if (PWMi <= -255){
    PWMi = 255;
   }
   int PWM1 = map(PWMi, -255, 255, 0, 180); 
   if (pos == 400){
    PWM1= 180;
    i = 0;
   }
   // Derechos
   if (PWMd >=255){
    PWMd =-255;
   }else if (PWMd <= -255){
    PWMd = 255;
   }
   int PWM2 = map(PWMd, -255, 255, 0, 180); 
   if (pos1 == 400){
    PWM2 = 180;
    i1 = 0;
   }
   Serial.print("izquierdos :");
   Serial.print(PWM1);
   Serial.print("derechos :");
   Serial.print(PWM2);
  // Izquierdos
  mid.write(PWM1);
  mit.write(PWM1);
  // Derechos
  mdd.write(PWM2);
  mdt.write(PWM2);
 
}

/*
 * Zona experimental-en proceso
 * Lecturas y acciones de sensores ultrasonicos para evadir un obsatculo
 */
void Ultra1(){
   //Ultra 2
  digitalWrite(pinTrig2, LOW);
  delayMicroseconds(100);
  digitalWrite(pinTrig2, HIGH);
  delayMicroseconds(100);
  long pulseDuration2 = pulseIn(pinEcho2, HIGH, 10000);
  distance2 = (int)(0.034 * pulseDuration2 / 2);
    Serial.print("ULT 2 cm " );
    Serial.print(distance2);
    Serial.print("  ");

}
void ultra(){
 //Ultra 1
  digitalWrite(pinTrig1, LOW);
  delayMicroseconds(100);
  digitalWrite(pinTrig1, HIGH);
  delayMicroseconds(100);
  long pulseDuration1 = pulseIn(pinEcho1, HIGH, 10000);
  int distance1 = (int)(0.034 * pulseDuration1 / 2);
    Serial.print("ULT 1 cm ");
    Serial.print(distance1);
    Serial.print("   ");
  // Acciones para esquivar el obstaculo y reencontrar la linea
  if (distance1<=5 && distance1 >=1){
    bucle = true;
    Moverse (VBMax, VBMax);
    delay(300);
    
    Moverse (VDMax, VBMax);
    delay(1300);
    
    Moverse (VDMin, VDMax);
    delay(2000);

    Moverse (VStop,VStop);
    delay(1200);
    
    Moverse (VBMin, VDMax);
    delay(500);
    
    Moverse (VDMin, VDMax);
    delay(2000);
    
    Moverse (VBMin, VDMax);
    delay(1200);

   while(bucle == true){
    delay(100);
    Ultra1();
    lectura();
    if(digital[3] == 1 or digital[4] == 1){
      bucle = false;
      Serial.print("det");
      Moverse (VStop, VStop);
      delay(400);
      Moverse (VDMax, VDMax);
      delay(250);
      Serial.print("det");
      Moverse (VStop, VStop);
      delay(400);
      direccion();
      break;
    }else {
      Moverse (VDMin, VDMax);
    }
   }
  }
}
 
