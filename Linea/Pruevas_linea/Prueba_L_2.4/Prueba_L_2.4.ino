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

// Variables y pines para el sonsor infrarojo
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
unsigned int Negro[8];
unsigned int Blanco[8];
int umbral[8];
unsigned int digital[8];
unsigned int diferencia[8];
int LP[8];
// Definir el contraste de la linea
int linea = 1;
// Definicion de los errores de la linea
String errorActual = "CE";
String errorAnterior = "";

// Servomotores 
Servo mid;
Servo mit;
Servo mdd;
Servo mdt;

// Pines de servomotores
const int pmdd = 22;
const int pmdt = 24;
const int pmit = 26;
const int pmid = 28;

// Leds Indicadores
const int ledBlue = 8;
const int ledRed = 9;

// Imprecion en la SD o en el puerto serial
int guardado = 1;
int serial = 1;

// Velocidades del servomotor 
int velConA3 = 150;//180
int velConA2 = 120;//150
int velConA1 = 110;//120
int velNeutra = 90;//90
int velConB1 = 70;//60
int velConB2 = 60;//30
int velConB3 = 30;//0

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
    Serial.begin(9600);
    // Guarda los datos
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

    pinMode(BOTON, INPUT_PULLUP);
    // pixy.init();
    pinMode(ledBlue, OUTPUT);
    pinMode(ledRed, OUTPUT);
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

void loop()
{
  /*
    lectura(); 
    direccion();
    if (guardado == 1)
    {
        guardar();
    }
    */
    Moverse(3, 3);
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
    else
    {
        Serial.println("Error al abrir el archivo.");
    }
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
    int VI;
    int VD;
    // Izquierdos
    if (auxVI == 3){VI = velConB3;}else if (auxVI == 2){VI = velConB2;}else if (auxVI == 1){VI = velConB1;}
    else if (auxVI == 0){VI = velNeutra;}
    else if (auxVI == -1){VI = velConA1;} else if (auxVI == -2){VI = velConA2;}else if (auxVI == -3){VI = velConA3;}
    // Derechos
    if (auxVD == 3){VD = velConA3;}else if (auxVD == 2){VD = velConA2;}else if (auxVD == 1){VD = velConA1;}
    else if (auxVD == 0){VD = velNeutra;}
    else if (auxVD == -1){VD = velConB1;}else if (auxVD == -2){VD = velConB2;}else if (auxVD == -3){VD = velConB3;}

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
    Serial.print("  ");
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print("  ");
        if (sensorValues[i] <= umbral[i])
        {
            digital[i] = 0;
        }
        else
        {
            digital[i] = 1;
        }
        Serial.print(digital[i]);
        Serial.print("");
    }
    Serial.println("");
}

void acciones()
{
    /*
      Acciones
      CE : Centrado
      PDD : Poco desviado hacia la derecha
      PDI : Poco desviado hacia la izquierda
      DI : Desviado hacia la izquierda
      DD : Desvoado hacia la derecha
      V9I : Vuelta de 90 grados hacia la izquierda
      V9D : Vuelta de 90 grados hacia la derecha
      NA : Nada detectado
    */

    if (errorActual == "CE")
    {
        Moverse(3, 3);
    }
    else if (errorActual == "MPDI")
    {
        Moverse(2, 3);
    }
    else if (errorActual == "MPDD")
    {
        Moverse(3, 2);
    }
    else if (errorActual == "PDI")
    {
        Moverse(1, 3);
    }
    else if (errorActual == "PDD")
    {
        Moverse(3, 1);
    }
    else if (errorActual == "DI")
    {
        Moverse(-2, 3);
    }
    else if (errorActual == "DD")
    {
        Moverse(3, -2);
    }
    else if (errorActual == "V9I")
    {
        Moverse(3, 3);
        delay(700);
        Moverse(1, 3);
        delay(400);
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
        /*
        Moverse(-2, -2);
        delay(150);
        Moverse(0, 0);
        delay(150);
        */

    }
    else if (errorActual == "V9D")
    {
        Moverse(3, 3);
        delay(700);
        Moverse(3, 1);
        delay(400);
        Moverse(1, -2);
        int bucleD = 1;
        while (bucleD == 1)
        {
            Moverse(1, -2);
            lectura();
            if (digital[3] == 1 && digital[4] == 1)
            {
                bucleD = 0;
            }
        }
        /*
        Moverse(-2, -2);
        delay(150);
        Moverse(0, 0);
        delay(150);
        */
    }
    else if (errorActual == "NA")
    {
        if (errorAnterior == "DI" or errorAnterior == "V9I" or errorAnterior == "PDI" or errorAnterior == "MPDI")
        {
            Moverse(-3, 2);
        }
        else if (errorAnterior == "DD" or errorAnterior == "V9D" or errorAnterior == "PDD" or errorAnterior == "MPDD")
        {
            Moverse(2, -3);
        }
        /*else if (errorAnterior == "CE"  )
        {
          Moverse(3, 3);
          delay(300);
        }*/
    }
    // delay(200);
}

void direccion()
{
    /*
      digital[0] digital[1] digital[2] digital[3] digital[4] digital[5] digital[6] digital[7]
    */
    // Centrado
    if (
        //-BBNNBB-
        (digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0))
    {
        errorActual = "CE";
        errorAnterior = errorActual;
        Serial.println("Centrado");
    }
    else
        /*Derecha
        Muy poco desviado
        */
        if (
            //-BBNNNB-
            (digital[1] == 0 && digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0))
        {
            errorActual = "MPDD";
            errorAnterior = errorActual;
            Serial.println("Muy poco desviado derecha");
        }
        else
            /*Izquierda
            Muy poco desviado
            */
            if (
                //-BNNNBB-
                (digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0 && digital[6] == 0))
            {
                errorActual = "MPDI";
                errorAnterior = errorActual;
                Serial.println("Muy poco desviado izquierda");
            }
            else
                /*Derecha
                Poco desviado desviado
                */
                if (
                    //-BBBNNB-
                    (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 0) ||
                    //-BBBNNN-
                    (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1))
                {
                    errorActual = "PDD";
                    errorAnterior = errorActual;
                    Serial.println("Poco desviado derecha");
                }
                else
                    /*Izquierda
                    Un poco desviado
                    */
                    if (
                        //-BNNBBB-
                        (digital[1] == 0 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0) ||
                        //-NNNBBB-
                        (digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0))
                    {
                        errorActual = "PDI";
                        errorAnterior = errorActual;
                        Serial.println("Poco desviado izquierda");
                    }
                    else
                        /*Derecha
                        Desviado
                        */
                        if (
                            //-BBBBNN-
                            (digital[1] == 0 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 1 && digital[6] == 1) ||
                            //--BBBNNN
                            (digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) ||
                            //--BBBBNN
                            (digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0 && digital[6] == 1 && digital[7] == 1))
                        {
                            errorActual = "DD";
                            errorAnterior = errorActual;
                            Serial.println("Desviado derecha");
                        }
                        else
                            /*Izquierda
                            Desviado
                            */
                            if (
                                //-NNBBBB-
                                (digital[1] == 1 && digital[2] == 1 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0 && digital[6] == 0) ||
                                // NNNBBB--
                                (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0) ||
                                // NNBBBB--
                                (digital[0] == 1 && digital[1] == 1 && digital[2] == 0 && digital[3] == 0 && digital[4] == 0 && digital[5] == 0))
                            {
                                errorActual = "DI";
                                errorAnterior = errorActual;
                                Serial.println("Desviado izquierda");
                            }
                            else
                                /*Derecha
                                Vueltas 90 grados
                                */
                                if (
                                    //--BBNNNN
                                    (digital[2] == 0 && digital[3] == 0 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1) ||
                                    //--BNNNNN
                                    (digital[2] == 0 && digital[3] == 1 && digital[4] == 1 && digital[5] == 1 && digital[6] == 1 && digital[7] == 1))
                                {
                                    errorActual = "V9D";
                                    errorAnterior = errorActual;
                                    Serial.print("Vuelta 90 derecha");
                                }
                                else
                                    /*Izquierda
                                    Vueltas a 90 grados
                                    */
                                    if (
                                        // NNNNBB--
                                        (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 0 && digital[5] == 0) ||
                                        // NNNNNB--
                                        (digital[0] == 1 && digital[1] == 1 && digital[2] == 1 && digital[3] == 1 && digital[4] == 1 && digital[5] == 0))
                                    {
                                        errorActual = "V9I";
                                        errorAnterior = errorActual;
                                        Serial.print("Vuelta 90 izquierda");
                                    }
                                    else
                                    {
                                        // Nada detectado
                                        errorActual = "NA";
                                        Serial.print("NA");
                                        Serial.print(errorAnterior);
                                    }

    acciones();
}
