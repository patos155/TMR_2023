//----------------------------LIBRERIAS----------------------------//
#include <QTRSensors.h>
#include <Servo.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SD.h>
#include <Pixy2.h>
#include <SD.h>
QTRSensors qtr;
Pixy2 pixy;
#define BOTON 3
#define CS 53

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

// Leds Indicadores
const int ledBlue = 8;
const int ledRed = 9;

// Variables y pines para el sonsor infrarojo
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
unsigned int Negro[8];
unsigned int Blanco[8];
int umbral[8];
unsigned int digital[8];
unsigned int diferencia[8];
int LP[8];

// Definicion de los errores de la linea de las condicionales y las intersecciones
String errorActual = "CE";
String errorAnterior = "";
boolean parches = false;

// Constantes del control PID
const float kp = 0.5;//0.5
const float kd = 1.5;//1.5
const float ki = 0.1;//0.1

// Variables del control PID
int P_D, I_D, D_D;
int P_I, I_I, D_I;
int sumaPosI, sumaI, posI, errorI, errorAntI;
int sumaPosD, sumaD, posD, errorD, errorAntD;

int setpointI = 300;
int setpointD = 400;

// Imprecion en la SD 
int guardado = 1;

// Definir la imprecion de la linea
int linea = 1;


/*
 * Calibra el sensor para la lectura de la linea
 * Calibra 20 veses en blanco y negro y saca el la diferencia
 */
void calibrate()
{
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, HIGH);
    delay(1000);
    Serial.println("Negro");
    // Calibrar valor negro
    while (digitalRead(BOTON) == HIGH)
        ;
    for (int i = 0; i < 20; i++)
    {
        negro();
        Serial.println("\t");
        delay(2);
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledRed, LOW);
        delay(2);
    }
    digitalWrite(ledBlue, HIGH);
    digitalWrite(ledRed, LOW);
    delay(1000);
    Serial.println("Blanco");
    // Calibrar valor blanco
    while (digitalRead(BOTON) == HIGH)
        ;
    for (int i = 0; i < 20; i++)
    {
        blanco();
        Serial.println("\t");
        delay(2);
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledRed, LOW);
        delay(2);
    }
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, LOW);
    delay(1000);
    promedio();
    Serial.println("");
}

void setup()
{
  //Inicia el puerto para la imprecion
  Serial.begin(9600);
  // Guarda los datos en la targeta SD
  if (guardado == 1)
  {
    if (SD.begin(CS))
    {
      Serial.println("Tarjeta SD inicializada correctamente.");
      // Eliminar el archivo si ya existe
      if (SD.exists("datos.txt"))
      {
          SD.remove("datos.txt");
          Serial.println("Archivo 'datos.txt' eliminado.");
      }
      // Crear un archivo llamado "datos.txt" en modo de escritura
      File dataFile = SD.open("datos.txt", FILE_WRITE);

      if (dataFile)
      {
          Serial.println("Archivo 'datos.txt' creado correctamente.");
          dataFile.close();
      }
      else
      {
          Serial.println("Error al crear el archivo.");
      }
    }
    else
    {
        Serial.println("Error al inicializar la tarjeta SD.");
    }
  }
  // Inicializa los servomotores
  mid.attach(pmid);
  mit.attach(pmit);
  mdd.attach(pmdd);
  mdt.attach(pmdt);
  // Define los pines de los sensores infrarojos
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A8, A9, A10, A11, A12, A13, A14, A15}, SensorCount);
  qtr.setEmitterPin(40);
  //Inicia el boton para la calibracion y los leds indicadores
  pinMode(BOTON, INPUT_PULLUP);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  //Inicia el sensor pixy
  pixy.init();
  //Crea o carga lecturas anteriores
  eeprom();
}

void promedio()
{
    for (int i = 0; i < 8; i++)
    {
        diferencia[i] = (Negro[i] - Blanco[i]) / 2;
        umbral[i] = Negro[i] - diferencia[i];
    }
    for (int i = 0; i < 8; i++)
    {
        Serial.print(diferencia[i]);
        Serial.print("\t");
    }
}
void blanco()
{
    for (int i = 0; i < 8; i++)
    {
        qtr.read(sensorValues);
        Serial.print(sensorValues[i]);
        Serial.print('\t');
        Blanco[i] = (sensorValues[i]);
    }
}
void negro()
{
    for (int i = 0; i < 8; i++)
    {
        qtr.read(sensorValues);
        Serial.print(sensorValues[i]);
        Serial.print('\t');
        Negro[i] = (sensorValues[i]);
    }
}

void eeprom()
{
    Serial.println("");
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, LOW);
    delay(2000);
    if (digitalRead(BOTON) == LOW)
    {
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledRed, HIGH);
        delay(1000);
        calibrate();
        for (int i = 0; i < 8; i++)
        {
            Serial.print(umbral[i]);
            Serial.print("\t");
        }
        Serial.println("");
        for (int i = 0; i < 8; i++)
        {
            LP[i] = umbral[i];
            Serial.print(LP[i]);
            Serial.print("\t");
        }

        Serial.println("");
        int LO[8];
        for (int i = 0; i < 8; i++)
        {
            EEPROM.put(i * sizeof(int), umbral[i]);
            EEPROM.get(i * sizeof(int), LO[i]);
            Serial.print(LO[i]);
            Serial.print("\t");
        }

        delay(2000);
        Serial.println("");
    }
    else
    {
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledRed, LOW);
        delay(1000);
        Serial.println("Valores anteriores:");
        int PL[8];
        for (int i = 0; i < 8; i++)
        {
            EEPROM.get(i * sizeof(int), umbral[i]);
            Serial.print(umbral[i]);
            Serial.print("\t");
        }
        Serial.println("");
        delay(1000);
    }
}
//-------------------------------------------------------LOOP-------------//
void loop()
{
  lectura();
  direccion();
  if (linea == 1){
    dibLinea();
  }
  if (guardado == 1)
  {
      guardar();
  }
  // Moverse(3, 3);
}

void dibLinea()
{
    Serial.print(" ");
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(digital[i]);
        Serial.print("");
    }
    Serial.print(" ");
    Serial.print("AccionR: ");
    Serial.print(errorActual);
    Serial.print(" ErrorA: ");
    Serial.print(errorAnterior);


    Serial.println("");
}
void guardar()
{
    File dataFile = SD.open("datos.txt", FILE_WRITE); // Abre el archivo "datos.txt" en modo escritura.

    if (dataFile)
    {

        dataFile.print("Lectura : ");
        for (uint8_t i = 0; i < SensorCount; i++)
        {
            dataFile.print(digital[i]);
            dataFile.print(" ");
        }
        dataFile.print("Acción realizada: ");
        dataFile.print(errorActual);
        dataFile.print(" Error anterior: ");
        dataFile.print(errorAnterior);
        dataFile.println("");
        // Cierra el archivo.
        dataFile.close();
    }
    /*
    else
    {
        Serial.println("Error al abrir el archivo.");
    }
    */
}

void Moverse(int auxVI, int auxVD)
{
    /*
      Servomotores
      3 : 180 Velocidad hacia adelante alta
      2 : 145 Velocidad hacia adelante media
      1 : 110 Velocidad hacia adelante baja
      0 : 90 Velocidad neutra
      -1 : 70 Velocidad hacia atras baja
      -2 : 35 Velocidad hacia atras media
      -3 : 0 Velocidad hacia atras alta
    */
    // Izquierdos
    int VI;
    int VD;
    if (auxVI == 3){VI = 0;}else if (auxVI == 2){VI = 35;}else if (auxVI == 1){VI = 70;}
    else if (auxVI == 0){VI = 90;}
    else if (auxVI == -1){VI = 110;}else if (auxVI == -2){VI = 145;}else if (auxVI == -3){VI = 180;}
    // Derechos
    if (auxVD == 3){VD = 180;}else if (auxVD == 2){VD = 145;}else if (auxVD == 1){VD = 110;}
    else if (auxVD == 0){VD = 90;}
    else if (auxVD == -1){VD = 70;}else if (auxVD == -2){VD = 35;}    else if (auxVD == -3){VD = 0;}

    int i1 = VI;
    int i2 = VI;
    // Derechos
    int d1 = VD;
    int d2 = VD;
    mid.write(i1);
    mit.write(i2);
    mdd.write(d1);
    mdt.write(d2);
}

void lectura()
{
    qtr.read(sensorValues);
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        if (sensorValues[i] <= umbral[i])
        {
            digital[i] = 0;
        }
        else
        {
            digital[i] = 1;
        }
    }
    
}
void parcheVerde()
{
  parches = false;
  int cuantos = 0;
  int x0,x1,y0,y1;
  int num_green_blocks = 0;
  int8_t i;
  char buf[128];  
  int bloques = pixy.ccc.getBlocks();
  if (bloques){
    if (pixy.ccc.numBlocks){
      for (int i = 0; i < pixy.ccc.numBlocks; i++) {
         if (pixy.ccc.blocks[i].m_signature == 1) {  // 1 = verde
           cuantos++;
         } 
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
    }
  }else{
    cuantos = 0;
  }
  if (cuantos == 0){
    Serial.print("Avaza");
    Moverse(3, 3);
    delay(1000);
    I_I = 0;
    I_D = 0;
  } else if (cuantos >=1 and x0 < 160 && x1 > 160){
    Serial.print("Vuelta U");
    Moverse(-3, 2);
    delay(500);
    int bucleU = 1;
    while (bucleU == 1)
    {
        Moverse(-3, 2);
        lectura();
        if (digital[3] == 1 && digital[4] == 1)
        {
            bucleU = 0;
        }
    }
  } else if (x0 < 160 ){
      Serial.print("I");
      Moverse(3, 3);
      delay(600);
      Moverse(-3, 2);
      delay(200);
      int bucleI = 1;
      while (bucleI == 1)
      {
          Moverse(-3, 2);
          lectura();
          if (digital[3] == 1 && digital[4] == 1)
          {
              bucleI = 0;
          }
      }
    } else if (x0 > 160 ){
      Serial.print("D");
      Moverse(3, 3);
      delay(600);
      Moverse(-2, -2);
      delay(200);
      int bucleI = 1;
      while (bucleI == 1)
      {
          Moverse(-2, -2);
          lectura();
          if (digital[3] == 1 && digital[4] == 1)
          {
              bucleI = 0;
          }
      }
    }

  
  
}
void acciones()
{
  /*
    Acciones
    CE : Centrado
    V9I : Vuelta de 90 grados hacia la izquierda
    V9D : Vuelta de 90 grados hacia la derecha
    PID : Recorrido en las lineas como curvas o lineas rectas
  */

  if (errorActual == "CE")
  {
      Moverse(3, 3);
      I_I = 0;
      I_D = 0;
  }
  //
  else if (errorActual == "INT" && parches == true)
  {
      Moverse(-3,-3);
      delay(600);
      Moverse(0,0);
      delay(2000);
      parcheVerde();

  }


  
  else if (errorActual == "V9I")
  {
      Moverse(3, 3);
      delay(1300);
      int bucleI = 1;
      while (bucleI == 1)
      {
          Moverse(-3, 2);
          lectura();
          if (digital[3] == 1 && digital[4] == 1)
          {
              bucleI = 0;
          }
      }
  }
  else if (errorActual == "V9D")
  {
      Moverse(3, 3);
      delay(1300);
      int bucleD = 1;
      while (bucleD == 1)
      {
          Moverse(2, -2);
          lectura();
          if (digital[3] == 1 && digital[4] == 1)
          {
              bucleD = 0;
          }
      }
  }
  else if (errorActual == "PID"){
    //Calculo PID Izquierda
    qtr.read(sensorValues);
    sumaPosI=(400*digital[3]+500*digital[2]+600*digital[1]+700*digital[0]);
    sumaI=(digital[0]+digital[1]+digital[2]+digital[3]);
    posI=(sumaPosI/sumaI);
    if(errorI<=500 && posI==-1){
      posI=400;
    }else if(errorI>=600 && posI==-1){
      posI=700;
    }
    errorI=posI;
    // Calculo PID Derecha
    sumaPosD=(700*digital[7]+600*digital[6]+500*digital[5]+400*digital[4]);
    sumaD=(digital[4]+digital[5]+digital[6]+digital[7]);
    posD=(sumaD/sumaPosD);
    // Serial.print(poslast);
    if(errorD<=500 && posD==-1){
      posD=400;
    }else if(errorD>=600 && posD==-1){
      posD=700;
    }
    errorD=posD;
    
    // Pid
    // Izquierdos
    P_I = posI - setpointI;
    D_I = P_I - errorAntI;
    I_I += P_I;
    int PWMi = 0;
    PWMi = (kp*errorI) + (ki*I_I) + (kd*D_I);
    errorAntI = P_I;
    // Pid
    // Derechos
    P_D = posD - setpointD;
    D_D = P_D - errorAntD;
    I_D += P_D;
    int PWMd = 0;
    PWMd = (kp*errorD) + (ki*I_D) + (kd*D_D);
    errorAntD = P_D;
    
    // Izquierdos
    if (PWMi >=255){//255
    PWMi =-255;
    }else if (PWMi <= -255){
    PWMi = 255;
    }
    int PWM1 = map(PWMi, -255, 255, 180, 0); 
    if (posI == 400){
    I_I = 0;
    }
    // Derechos
    if (PWMd >=255){
    PWMd =-255;
    }else if (PWMd <= -255){
    PWMd = 255;
    }
    int PWM2 = map(PWMd, -255, 255, 0, 180); 
    if (posD == 400){
    I_D = 0;
    }
    // Izquierdos
    mid.write(PWM1);
    mit.write(PWM1);
    // Derechos
    mdd.write(PWM2);
    mdt.write(PWM2);
   }

}


void direccion()
{
/*
  digital[0] digital[1] digital[2] digital[3] digital[4] digital[5] digital[6] digital[7]
*/
  // Centrado
  if (
    //BBBBBBBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0) ||
    //BBBNNBBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0) ||
    //BBBNBBBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0) ||
    //BBBBNBBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0) ||
    //BBBNNNBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0 && digital[7] == 0) ||
    //BBNNNBBB
    (digital[0] == 0 && digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0)
    )
    {
    errorActual = "CE";
    errorAnterior = errorActual;
    }else
    /*Derecha
    Vueltas 90 grados
    */
    if (
       //BBBBNNNN
       (digital[0] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) ||
       //BBBNNNNN
       (digital[1] == 0 && digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) ||
       //BBNNNNNN
       (digital[0] == 0 && digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) 
       //BBNNNNNB
       //(digital[0] == 0 && digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 0)
       )
       {
        errorActual = "V9D";
        errorAnterior = errorActual;
       }else
       /*Izquierda
       Vueltas a 90 grados
       */
       if (
          //NNNNBBBB
          (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0&& digital[6] == 0 && digital[7] == 0) ||
          //NNNNNBBB
          (digital[1] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0 && digital[7] == 0) ||
          //NNNNNNBB
          (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0 && digital[7] == 0) 
          //BNNNNNBB
          //(digital[0] == 0 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0 && digital[7] == 0)
          )
          {
          errorActual = "V9I";
          errorAnterior = errorActual;
          }else
          /* Interseccion
           * NNNNNNNN
           */
          if(
            (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) 
            )
            {
            errorActual = "INT";
            errorAnterior = errorActual;
            }else{
                  // Se encuentra en la linea
                  errorActual = "PID";
                  errorAnterior = errorActual;
                 }
   acciones();
}  
