#include <Servo.h>

Servo servoX; // servos de la camara
Servo servoY;

int StateX = 90; // variables que ayudan a los angulos de los servos
int StateY = 90;

int cambio = 0; // variable para registrar x o y cuando se encuentre | en el mensaje
char apollo; // apollo para comparar letras
String stateX; // variable donde se guarda el valor numerico de x a forma de string
String stateY; // variable donde se guarda el valor numerico de y a forma de string

String datosSr[8]; // maximos y minimos para la zona muerta de los ejes X y Y (0-3), maximos y minimos para el movimiento (4-7) en string

int datosNo[8]; // maximos y minimos para la zona muerta de los ejes X y Y (0-3), maximos y minimos para el movimiento (4-7) en numeros
                // maxmX minmX maxmY minmY maxX minX maxY minY
                //   0     1     2     3     4    5    6    7
void setup() 
{ 
  Serial.begin(9600);

  servoX.attach(8); // pines donde estan conectados los servos
  servoY.attach(9);

  servoX.write(90); // se inician los servos en posicion recta
  servoY.write(90);

  DatosInicio(); // obtencion de las variables de inicio (maximos y minimos)
}

void loop()
{  
  readSerialPort(); //le los mensajes del serial
  control_motores(); //establece los valores para StateX y StateY
  servoX.write(StateX); // se mandan los angulos a los servos
  servoY.write(StateY);
  ImprimirDatos(); // se imprime lo que se tenga que imprimir
}

void ImprimirDatos(){
  Serial.print("State X = ");
  Serial.print(StateX);
  Serial.print(" State Y = ");
  Serial.print(StateY);
  Serial.print(" Estado X = ");
  Serial.print(stateX.toInt());
  Serial.print(" Estado Y = ");
  Serial.println(stateY.toInt());
}

void DatosInicio(){
  while (cambio != 8){                             
    if (Serial.available()){
      while (Serial.available() > 0){
        apollo = (char)Serial.read();
        if (apollo == '|'){
          cambio ++;
        }else{
          Serial.println(cambio);
          datosSr[cambio] += apollo;
        }
      }
    }
  }
  for (int i=0;i<8;i++){
    datosNo[i] = datosSr[i].toInt();
    Serial.println(datosNo[i]);
  }
  cambio = 1;
  apollo = "";
  Serial.println("Datos Registrados correctamente");
}

void readSerialPort() {  
  apollo = "";
  cambio = 1;
  if (Serial.available()){
    stateX = "";
    stateY = "";
    while (Serial.available() > 0){
      apollo = (char)Serial.read();
      if (apollo == '|'){
        cambio = 0;
      }else{
        if (cambio){
          stateX += apollo;
        }else{
          stateY += apollo;
        }
      }
    }
    Serial.flush();
  }
}

void control_motores()
{
  StateX = map(stateX.toInt(),datosNo[5],datosNo[4],0,180); // devuelve solo un numero entero entre los valores de las ultimas posiciones segun los valores de las primeras posicions
  StateY = map(stateY.toInt(),datosNo[7],datosNo[6],0,180);
  
  if (stateX.toInt() > datosNo[1] and stateX.toInt() < datosNo[0]){ // si esta en la zona muerta el valor es 90
    StateX = 90;
  }
  if (stateY.toInt() > datosNo[3] and stateY.toInt() < datosNo[2]){
    StateY = 90;
  }
}