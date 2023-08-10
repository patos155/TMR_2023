#include <Servo.h>
Servo mi1;
Servo mi2;

String nom = "Arduino";
String msg;
String continuar = "0";

// definicion de los puertos de entrada 
int rcPins[6] = {25,27,29,31,33,35};
int ReDig[8] = {0,0,0,0,0,0,0,0};
// definicion de salidas para control de los relevadores 
int izq1=53;
int izq2=42;
int der1=48;
int der2=46;
int volt=44;
int voltaje=1;
int pos_servo=0;
int pos_brazo=0;



float chValue[6];


//Control de velocidad de arranque 
int PWM_PINI = 6;
int pwmvalI = 0;
int PWM_PIND = 7;
int pwmvalD = 0;
int mi=0;
int md=0;

 
const int pulseInDelay = 30000;   //20000;
 
void setup() 
{ 
   Serial.begin(9600);
   //conexion a relevadores 
   pinMode(izq1, OUTPUT);
   pinMode(izq2, OUTPUT);
   pinMode(der1, OUTPUT);
   pinMode(der2, OUTPUT);
   pinMode(volt, OUTPUT);
   mi1.attach(10);
   mi2.attach(9);

   //control de velocidad de arranque de los motores 
   pinMode(PWM_PINI,OUTPUT);
   pinMode(PWM_PIND,OUTPUT);
}


void arr_motor()
{
   //Si cambio la direccion del motor o se detuvo entonces 
   // el pulso se reinicia 
   Serial.print("--------------- Motor izquierdo ");
   Serial.println(mi);
   Serial.print("Izquierdo!!!!!!!!!!11!");
   Serial.println(pwmvalI);
   if (mi==0){
       pwmvalI=1;
   }
   //else{
   //    delay(1000);
   //}
   // Incrementa la frecuencia para acelerar los motores 
   analogWrite(PWM_PINI, pwmvalI);
   if (pwmvalI<255){
       pwmvalI=pwmvalI+10;
   }else{
       pwmvalI=255;
   }
   //Si cambio la direccion del motor o se detuvo entonces 
   // el pulso se reinicia 
   Serial.print("--------------- Motor derecho ");
   Serial.println(md);
   Serial.print("Derecho!!!!!!!!!!!");
   Serial.println(pwmvalD); 
   // Incrementa la frecuencia para acelerar los motores 
   if (md==0){
       pwmvalD=1;
   }
   //else{
   //    delay(1000);
   //}
   // Incrementa la frecuencia para acelerar los motores 
   analogWrite(PWM_PIND, pwmvalD);
   if (pwmvalD<255){
       pwmvalD=pwmvalD+10;
   }else{
       pwmvalD=255;
   }
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
 
void control_motores()
{
   if ( chValue[5]<=.5 || continuar=="1"){
     Serial.println("Robot se detiene");
     // los valores de los relevadores izquierdos se ponen en 1,0 
     // para ir hacia atras
     //izq1=1 izq2=0;
     Serial.println("Izquierdo UNO BAJO");
     Serial.println("Izquierdo DOS BAJO");
     Serial.println("Derecho UNO BAJO");
     Serial.println("Derecho DOS BAJO");
     digitalWrite(izq1,LOW);
     digitalWrite(izq2,LOW);
     digitalWrite(der1,LOW);
     digitalWrite(der2,LOW);
     
  }else {
      Serial.println("robot se mueve");
      //delay(1000);
      // Motores Iaquierdos
      if (chValue[2]>.30 and chValue[2]<.70){
         Serial.println("Izquierdo PARADO");
         // los valores de los relevadores izquierdos se ponen en 0,0 para detener los motores
         //izq1=0  izq2=0;
         digitalWrite(izq1,LOW);
         digitalWrite(izq2,LOW);
         mi=0;
      }else{
         if (chValue[2]>.70 and chValue[4]>.3){
            Serial.println("Izquierdo ADELANTE");
            // los valores de los relevadores izquierdos se ponen en 0,1 para ir hacia adelante
            //izq1=0 izq2=1;
            digitalWrite(izq1,LOW);
            digitalWrite(izq2,HIGH);
            mi=1;
         } else {
             if (chValue[2]<.30 and chValue[4]>.3){
                Serial.println("Izquierdo atras");
                // los valores de los relevadores izquierdos se ponen en 1,0 para ir hacia atras
                //izq1=1 izq2=0;
                digitalWrite(izq1,HIGH);
                digitalWrite(izq2,LOW);
                mi=-1;
             }
         }
      }
      // Motores derechos
      if (chValue[1]>.30 and chValue[1]<.70){
         Serial.println("derecho PARADO");
         // los valores de los relevadores izquierdos se ponen en 0,0 para detener los motores
         //izq1=0  izq2=0;
         digitalWrite(der1,LOW);
         digitalWrite(der2,LOW);
         md=0;
      }else{
         if (chValue[1]>.70 and chValue[4]>.3){
            Serial.println("derecho  ADELANTE");
            // los valores de los relevadores derechos se ponen en 0,1 para ir hacia adelante
            //izq1=0 izq2=1;
            digitalWrite(der1,LOW);
            digitalWrite(der2,HIGH);
            md=1;
         } else {
             if (chValue[1]<.30 and chValue[4]>.3){
                Serial.println("derecho atras");
                // los valores de los relevadores derechos se ponen en 1,0 para ir hacia atras
                //izq1=1 izq2=0;
                digitalWrite(der1,HIGH);
                digitalWrite(der2,LOW);
                md=-1;
             }
         }
      }

      arr_motor();
      
      // Servo brazo camara
      if (chValue[3]>.40 and chValue[3]<.60){
         Serial.println("brazo");
         Serial.println(pos_brazo);
         mi1.write(pos_brazo);
      }else{
         if (chValue[3]>.60){
            Serial.println("brazo ");
            pos_brazo=pos_brazo+10;
            if (pos_brazo>160){
              pos_brazo=160;
            }
            mi1.write(pos_brazo);
            Serial.println(pos_brazo);
         } else {
             if (chValue[3]<.40 ){
                Serial.println("brazo ");
                pos_brazo=pos_brazo-10;
                if (pos_brazo<0){
                    pos_brazo=0;
                }
                mi1.write(pos_brazo);
                Serial.println(pos_brazo);
             }
         }
         
      }  
      // Servo camara
      if (chValue[0]>.40 and chValue[0]<.60){
         Serial.println("Servo camara");
         Serial.println(pos_servo);
         mi2.write(pos_servo);
      }else{
         if (chValue[0]>.60){
            Serial.println("Servo camara");
            pos_servo=pos_servo+10;
            if (pos_servo>160){
              pos_servo=160;
            }
            mi2.write(pos_servo);
            Serial.println(pos_servo);
         } else {
             if (chValue[0]<.40){
                Serial.println("servo camara");
                pos_servo=pos_servo-10;
                if (pos_servo<0){
                    pos_servo=0;
                }
                mi2.write(pos_servo);
                Serial.println(pos_servo);
             }
         }
      }
  }   
  //delay(2000);
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
  Serial.print("-----------------------------------continuar: ");
  Serial.println(continuar);
  if (msg=="1"){
    continuar="1";
  }
  if (continuar != "1"){
    for(int iChannel = 0; iChannel < 6; iChannel++){
      Serial.println(iChannel);
      readChannel(iChannel);
    }
    printChannel();
    
    control_motores();  
  }else{
    digitalWrite(izq1,LOW);
    digitalWrite(izq2,LOW);
    digitalWrite(der1,LOW);
    digitalWrite(der2,LOW);
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
