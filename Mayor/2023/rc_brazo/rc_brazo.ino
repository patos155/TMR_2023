#include <Servo.h>

Servo servo_1;
String nom = "Arduino";
String msg;
String continuar = "0";

// definicion de los puertos de entrada 
int rcPins[6] = {25,27,29,31,33,35};
int ReDig[8] = {0,0,0,0,0,0,0,0};
// definicion de salidas para control de los relevadores
int servo_pin = 13; 
int izq1=53;
int izq2=42;
int der1=48;
int der2=46;
int cen1=43;
int cen2=49;
int volt=44;
int motor = 3;
float poteD = 0;
int potencia = 0;
int potenciaServo = 90;

int min = 250;
int max = 500;

int voltaje=1;
int pos_servo=0;
int pos_brazo=0;

float chValue[6];


//Control de velocidad de arranque 
int PWM_PIN = 6;

 
const int pulseInDelay = 30000;   //20000;
 
void setup() 
{ 
   Serial.begin(9600);
   //conexion a relevadores 
   pinMode(izq1, OUTPUT);
   pinMode(izq2, OUTPUT);
   pinMode(der1, OUTPUT);
   pinMode(der2, OUTPUT);
   pinMode(cen1, OUTPUT);
   pinMode(cen2, OUTPUT);
   pinMode(volt, OUTPUT);

   pinMode(PWM_PIN,OUTPUT);

   servo_1.attach(servo_pin);

}


float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
void readChannel(int channel)
{
  int rawValue = pulseIn(rcPins[channel], HIGH, pulseInDelay);
  chValue[channel] = fmap( (float)rawValue, 1000.0, 2000.0, 0.0, 1.0);
  chValue[channel] = chValue[channel] < 0.0 ? 0.0 : chValue[channel]; 
  chValue[channel] = chValue[channel] > 1.0 ? 1.0 : chValue[channel];  
}

void difDelay(){
  if (chValue[5]<=0.5){
    delay(max);
  }else{
    delay(min);
  }
}

void control_motores()
{
   if ( chValue[4]<=.5 || continuar == "1"){
     Serial.println("Robot se detiene");
     // los valores de los relevadores izquierdos se ponen en 1,0 
     // para ir hacia atras
     //izq1=1 izq2=0;
     /*Serial.println("Izquierdo UNO BAJO");
     Serial.println("Izquierdo DOS BAJO");
     Serial.println("Derecho UNO BAJO");
     Serial.println("Derecho DOS BAJO");*/
     digitalWrite(izq1,LOW);
     digitalWrite(izq2,LOW);
     digitalWrite(der1,LOW);
     digitalWrite(der2,LOW);
     digitalWrite(cen1,LOW);
     digitalWrite(cen2,LOW);
     servo_1.write(90);
     
  }else {
      //Serial.println("robot se mueve");
      //delay(1000);
      // Motor de brazo (grande)
      bloqMotor();
      if (chValue[5] >= 0.5){
        Serial.println("Configuracion normal");

        if (chValue[2]>.20 and chValue[2]<.80){
          //Serial.println("Elevacion bloqueada");
          // los valores de los relevadores izquierdos se ponen en 0,0 para detener los motores
          //izq1=0  izq2=0;
          digitalWrite(izq1,LOW);
          digitalWrite(izq2,LOW);
        }else{
          if (chValue[2]>.80 and motor == 2){
              Serial.println("Aumentando Elevacion"); 
              // los valores de los relevadores izquierdos se ponen en 0,1 para ir hacia adelante
              //izq1=0 izq2=1;
                  motor = 2;
                  analogWrite(PWM_PIN, 120);
                  digitalWrite(izq1,LOW);
                  digitalWrite(izq2,HIGH);
                  delay(75);
                  analogWrite(PWM_PIN, 0); 
                  delay(120); 
          } else {
              if (chValue[2]<.20 and motor == 2){
                  Serial.println("Reduciendo Elevacion");
                  // los valores de los relevadores izquierdos se ponen en 1,0 para ir hacia atras
                  motor = 2;
                  analogWrite(PWM_PIN, 120);
                  digitalWrite(izq1,HIGH);
                  digitalWrite(izq2,LOW);
                  delay(75);
                  analogWrite(PWM_PIN, 0); 
                  delay(120);     
              }
          }
        }
        // Motores de elevacion (chiquito)
        if (chValue[1]>.20 and chValue[1]<.80){
          //Serial.println("Brazo bloqueado");
          // los valores de los relevadores izquierdos se ponen en 0,0 para detener los motores
          //izq1=0  izq2=0;
          digitalWrite(der1,LOW);
          digitalWrite(der2,LOW);
        }else{
          if (chValue[1]>.80 and motor == 1){
              Serial.println("Bajando Brazo");
              // los valores de los relevadores derechos se ponen en 0,1 para ir hacia adelante
              //izq1=0 izq2=1;
              motor = 1;
              analogWrite(PWM_PIN, 75);
              digitalWrite(der1,LOW);
              digitalWrite(der2,HIGH);
              delay(100);
              analogWrite(PWM_PIN, 0); 
              delay(150);     
          } else {
              if (chValue[1]<.20 and motor == 1){
                  Serial.println("Subiendo Brazo");
                  // los valores de los relevadores derechos se ponen en 1,0 para ir hacia atras
                  //izq1=1 izq2=0;
                  motor = 1;
                  analogWrite(PWM_PIN, 75);
                  digitalWrite(der1,HIGH);
                  digitalWrite(der2,LOW);
                  delay(100);
                  analogWrite(PWM_PIN, 0); 
                  delay(150);       
              }
          }
        }
        //Motores centro poyo
        if (chValue[0]>.20 and chValue[0]<.80){
          //Serial.println("CENTRO PARADO");
          digitalWrite(cen1,LOW);
          digitalWrite(cen2,LOW);
        }else{
          if (chValue[0]>.80 and motor == 0){
              Serial.println("Centro Derecha");
              motor = 0;
              analogWrite(PWM_PIN, 150);
              digitalWrite(cen1,LOW);
              digitalWrite(cen2,HIGH);
              delay(100);
              analogWrite(PWM_PIN, 0); 
              delay(100);           
          } else {
              if (chValue[0]<.20 and motor == 0){
                  Serial.println("Centro Izquierda");
                  motor = 0;
                  analogWrite(PWM_PIN, 150);
                  digitalWrite(cen1,HIGH);
                  digitalWrite(cen2,LOW);
                  delay(100);
                  analogWrite(PWM_PIN, 0); 
                  delay(100);       
                  
              }
          }
        }
      // Servo

        if (chValue[3]>.20 and chValue[3]<.80){
          //Serial.println("Servo PARADO");
          servo_1.write(90); // posiciones de 0-180 donde 90 es parado
        }else{
          if (chValue[3]>.80){
              Serial.println("Servo sube");
              servo_1.write(105);
          } else {
              if (chValue[3]<.20){
                  Serial.println("Servo baja");
                  servo_1.write(75);
              }
          }
        }
      }else{
        Serial.println("-----------------------Configuracion de potencia-----------------------------");
        
        if (chValue[2]>.40 and chValue[2]<.60){ 
          digitalWrite(izq1,LOW);
          digitalWrite(izq2,LOW);
        }else{
          if (chValue[2]>.60 and motor == 2){
              Serial.println("Aumentando Elevacion"); 
                  motor = 2;
                  potencia = round((chValue[2]-0.6)*637.5);//850
                  Serial.print("Potencia: ");
                  Serial.println(potencia);
                  analogWrite(PWM_PIN, potencia);
                  digitalWrite(izq1,LOW);
                  digitalWrite(izq2,HIGH);
          } else {
              if (chValue[2]<.40 and motor == 2){
                  Serial.println("Reduciendo Elevacion");
                  motor = 2;
                  potencia = round((chValue[2]*-1)*637.5)+255;//850
                  Serial.print("Potencia: ");
                  Serial.println(potencia);
                  analogWrite(PWM_PIN, potencia); 
                  digitalWrite(izq1,HIGH);
                  digitalWrite(izq2,LOW);
              }
          }
        }
        // Motores de elevacion (chiquito)
        if (chValue[1]>.40 and chValue[1]<.60){
          digitalWrite(der1,LOW);
          digitalWrite(der2,LOW);
        }else{
          if (chValue[1]>.60 and motor == 1){
              Serial.println("Bajando Brazo");
              motor = 1;
              potencia = round((chValue[1]-0.6)*637.5);
              Serial.print("Potencia: ");
              Serial.println(potencia);
              analogWrite(PWM_PIN, potencia);
              digitalWrite(der1,LOW);
              digitalWrite(der2,HIGH);
          } else {
              if (chValue[1]<.40 and motor == 1){
                  Serial.println("Subiendo Brazo");
                  motor = 1;
                  potencia = round((chValue[1]*-1)*637.5)+255;
                  Serial.print("Potencia: ");
                  Serial.println(potencia);
                  analogWrite(PWM_PIN, potencia); 
                  digitalWrite(der1,HIGH);
                  digitalWrite(der2,LOW);
              }
          }
        }
        //Motores centro poyo
        if (chValue[0]>.40 and chValue[0]<.60){
          //Serial.println("CENTRO PARADO");
          digitalWrite(cen1,LOW);
          digitalWrite(cen2,LOW);
        }else{
          if (chValue[0]>.70 and motor == 0){
              Serial.println("Centro Derecha");
              motor = 0;
              potencia = round((chValue[0]-0.6)*637.5);
              Serial.print("Potencia: ");
              Serial.println(potencia);
              analogWrite(PWM_PIN, potencia); 
              digitalWrite(cen1,LOW);
              digitalWrite(cen2,HIGH);
          } else {
              if (chValue[0]<.30 and motor == 0){
                  Serial.println("Centro Izquierda");
                  motor = 0;
                  potencia = round((chValue[0]*-1)*637.5)+255;
                  Serial.print("Potencia: ");
                  Serial.println(potencia);
                  analogWrite(PWM_PIN, potencia); 
                  digitalWrite(cen1,HIGH);
                  digitalWrite(cen2,LOW);
                  
              }
          }
        }
      // Servo

        if (chValue[3]>.40 and chValue[3]<.60){ //3
          //Serial.println("Servo PARADO");
          servo_1.write(90); // posiciones de 0-180 donde 90 es parado
        }else{
          if (chValue[3]>.60){
              potenciaServo = round(((chValue[3]*100)*2.25)-45);
              Serial.print("PotenciaServo: ");
              Serial.println(potenciaServo);
              Serial.println("Servo sube");
              servo_1.write(potenciaServo);
          } else {
              if (chValue[3]<.40){
                  potenciaServo = round((chValue[3]*100)*2.25);
                  Serial.print("PotenciaServo: ");
                  Serial.println(potenciaServo);                  
                  Serial.println("Servo baja");
                  servo_1.write(potenciaServo);
              }
          }
        }
      }
    
  }   
  //delay(2000);
  //Serial.print("-----------Motor: ");
  //Serial.println(motor);
}

void bloqMotor(){
  if ((chValue[0] < 0.40 or chValue[0] > 0.60) and motor == 3){
    motor = 0;
  }

  if ((chValue[1] < 0.40 or chValue[1] > 0.60) and motor == 3){
    motor = 1;
  }

  if ((chValue[2] < 0.40 or chValue[2] > 0.60) and motor == 3){
    motor= 2;
  }
  
  if (chValue[0] > 0.40 and chValue[0] < 0.60 and chValue[1] > 0.40 and chValue[1] < 0.60 and chValue[2] > 0.40 and chValue[2] < 0.60){
    motor = 3;
  }

  if (chValue[0] > 0.40 and chValue[0] < 0.60 and motor == 0){
    motor = 3;
  }

  if (chValue[1] > 0.40 and chValue[1] < 0.60 and motor == 1){
    motor = 3;
  }

  if (chValue[2] > 0.40 and chValue[2] < 0.60 and motor == 2){
    motor = 3;
  }
  
}

void printChannel()
{
  for(int iChannel = 0; iChannel < 6; iChannel++)
  {
    Serial.print("Ch #");
    Serial.print(iChannel);
    Serial.print(": ");
    Serial.println(chValue[iChannel]);
  };
  Serial.println("------------");
  //delay(2000);
}
 
void loop()
{
  readSerialPort();
  if(msg=="1"){
    continuar="1";
  }

  if (continuar != "1"){
    for(int iChannel = 0; iChannel < 6; iChannel++){
      //Serial.println(iChannel);
      readChannel(iChannel);
    }
    printChannel();
    
    control_motores();  
  }else{
    digitalWrite(izq1,LOW);
    digitalWrite(izq2,LOW);
    digitalWrite(der1,LOW);
    digitalWrite(der2,LOW);
    digitalWrite(cen1,LOW);
    digitalWrite(cen2,LOW);
    servo_1.write(90);
  }
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
