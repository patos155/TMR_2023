/*-----------------------------------
 *            LIBRERIAS            *
-----------------------------------*/
#include <VL53L0X.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MLX90614.h>
#include <MPU6050.h>
#include <SPI.h>
#include <I2Cdev.h>
#include <SD.h>
extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
#include "SparkFunISL29125.h"
}

//Procediemento que selecciona el puerto en el multiplexor
#define TCAADDR 0x70
void tcaselect(uint8_t i) {
    if (i > 7) return;
        Wire.beginTransmission(TCAADDR);
        Wire.write(1 << i);
        Wire.endTransmission();
}

String gris[]={"65D","369","368"};


/*---------------------------------*
 *            MOTORES              *
-----------------------------------*/
  Servo mid;
  Servo mit;
  Servo mdd;
  Servo mdt;


VL53L0X sensor;

SFE_ISL29125 RGB_1;
SFE_ISL29125 RGB_2;
/*-----------------------------------
 *            VARIABLES            *
-----------------------------------*/

/////////// pines de conexion ////////////////////
/* Motores  */
  const int pmit=47;
  const int pmid=49;
  const int pmdd=43;//43
  const int pmdt=45;//45


/* Ultrasónicos  */
  const int trig_it =9;           // conexiones de triger y echo para los sensores ultrsonicos 
  const int echo_it = 8;
  const int trig_if = 3;
  const int echo_if = 2;
  const int trig_ft = 22;
  const int echo_ft = 24;
  const int trig_df = 31;
  const int echo_df = 33;
  const int trig_dt = 35;
  const int echo_dt = 37;

// Temperatura infrarroja  
  Adafruit_MLX90614 tempI = Adafruit_MLX90614();
  Adafruit_MLX90614 tempD = Adafruit_MLX90614();

//Giroscopio  
  const int mpu_dir = 0x68;
  MPU6050 giro(mpu_dir);

//SD  
  File archivo;
  const uint8_t BUFFER_SIZE = 20;
  char fileName[] = "datos.txt"; // SD library only supports up to 8.3 names
  char buff[BUFFER_SIZE+2] = "";  // Added two to allow a 2 char peek for EOF state
  uint8_t index = 0;
  const uint8_t chipSelect = 53;//8;p
  const uint8_t cardDetect = 52;//6;
  enum states: uint8_t { NORMAL, E, EO };
  uint8_t state = NORMAL;
  bool alreadyBegan = false;  // SD.begin() misbehaves if not first call



  

/* Variables de distancia minima a paredes */
  int LL=200;                       // Distancia lateral
  int LD=70;                        // Distancia delantera 
  String inercia="FT";             // Inercia 
  int margen = 2;                  // Variable de rango entre sensores izquierdos para centrar
  int dif=1;                       // diferencia entre sensores 
  String mensaje="";               // para ver ajuste 
  

/*Variables de duracion de los movimientos*/

  int imp = 40;                     //Tiempo impulso luego de giro izquierda
  int giros_I = 15; //16
  int giros_ISF = 15; //16
  int giros_D = 23; //20               //Tiempo de giros izquierda y derecha
  int gir_u =54;                    //Tiempo de giro en u
  int avance_ISF = 0;
  
// activar/desactivar impresion a puero serial 
  int ab_serial=0;
  int ab_grabar=1;
  int tiempo=0;

/*  Variables de temperatura  */
  float ti_amb = 0;
  float ti_obj = 0;
  float ti_dif = 0;
  float td_amb = 0;
  float td_obj = 0;
  float td_dif = 0;
  int   con_vi = 0;
  int   led_vi = 40;
  bool vI = false;

/*  Variables de giroscopio  */
  int ax = 0;
  int ay = 0;
  int az = 0;
  int gx = 0;
  int gy = 0;
  int gz = 0;
  float ang_x = 0;
  float ang_y = 0;
  float ang_z = 0;

/* variables de sensores de piso */
  unsigned int red_1 = 0;
  unsigned int green_1 = 0;
  unsigned int blue_1 = 0;
  unsigned int red_2 = 0;
  unsigned int green_2 = 0;
  unsigned int blue_2 = 0;
  String color="";  
  int lr1 = 65;
  int lr2 = 55;
  int lg1 = 60;
  int lg2 = 50;
  int lb1 = 60;
  int lb2 = 50;

  
/* Contador de ciclo */
  int ciclo=0;
 
// variables para sensores ultrasonicos 
  long MIT,MIF,MFT,MDF,MDT;        // Mediciones de sensores ultrasonicos en centimetros 
  long MMIT,MMIF,MMFT,MMDF,MMDT;   // Mediciones de sensores ultrasonicos en milimetros
  int  LIT,LIF,LFT,LDF,LDT;        // Valores logicos de lectura de sensores ultrasonicos

void setup()
{
  Serial.begin(9600);
  /*  Motores  */
    mid.attach(pmid);
    mit.attach(pmit);
    mdd.attach(pmdd);
    mdt.attach(pmdt);
// sensores de distancia ultrasonicos 
  pinMode(trig_it, OUTPUT);    //Izquierdo Trasero
  pinMode(echo_it, INPUT);
  pinMode(trig_if, OUTPUT);    //Izquierdo frontal
  pinMode(echo_if, INPUT);
  pinMode(trig_ft, OUTPUT);    //Frontal
  pinMode(echo_ft, INPUT);
  pinMode(trig_df, OUTPUT);    //Derecho frontal
  pinMode(echo_df, INPUT);
  pinMode(trig_dt, OUTPUT);    //Derecho trasero
  pinMode(echo_dt, INPUT);
  pinMode(led_vi, OUTPUT);
 
  digitalWrite(trig_it,LOW);   // pone a bajo los trigers de los sensores ultarsonicos 
  digitalWrite(trig_if,LOW);
  digitalWrite(trig_ft,LOW);
  digitalWrite(trig_df,LOW);
  digitalWrite(trig_dt,LOW);
  digitalWrite(led_vi,LOW);
 

  //inicia la comunicación I2C
  while (!Serial);
  //delay(1000);

  Wire.begin();
    
  //Serial.println("\nTCAScanner ready!");

  //busca dispositivos I2C
  for (uint8_t t=0; t<8; t++) {
   // Serial.println(t);
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
      }
    }
  }

    //  Infrarrojos de temperatura
    tcaselect(2);
    tempI.begin();
    tcaselect(3);
    tempD.begin();
 
  
  //Giroscopio  
    tcaselect(1);
    giro.initialize();
  
  // SD  
  //  while (!Serial);  // Wait for serial port to connect (ATmega32U4 type PCBAs)
    pinMode(cardDetect, INPUT);
    if (!SD.begin(chipSelect)){
    //  Serial.println("error al iniciar SD");
    }else{
      //Serial.println("SD conectada");
    }

    if (SD.exists("Maze.txt")){
      SD.remove("Maze.txt");
    }
    archivo = SD.open("Maze.txt",FILE_WRITE);
    archivo.println("1");
    archivo.close();
    // sensores de piso 
    tcaselect(5);
    if (RGB_1.init()){
      Serial.println("Sensor piso 1 inicializado");
    }else{
      Serial.println("Sensor piso 1 NO inicializado");
    }
    tcaselect(6);
    if (RGB_2.init()){
      Serial.println("Sensor piso 2 inicializado");
    }else{
      Serial.println("Sensor piso 2 NO inicializado");
    }
}

void sal_trampa(){
   if (ciclo<=giros_D){
    moverse(-1,-1);
    inercia="BACK";
    ciclo++;
    }else{
      ciclo=0;
      giro_u();
      
    }
}

void loop(){
  ab_serial=0;
  ab_grabar=1;
  giros_I=20;   //16     10
  giros_ISF=23; //16
  giros_D=20;   //20
  imp=30;
  LL=200;                       // Distancia lateral
  LD=60;                        // Distancia delantera 80
  Leer();
  direccion();
  //ciclo++;
  //grabar();
  //moverse(1,1); 
}


//*--------------------------------------------------------------------------------*
//*                            FUNCIONES Y PROCESOS                                *
//*--------------------------------------------------------------------------------*

//***************************************************
//*     Lectura de Sensores                         *
//***************************************************

void direccion(){
  mensaje="";
  if(color != "NEGRO" && con_vi==0){
    if (inercia=="FT"){
      //Con pared al frente
      if (LFT==0){
        // NO HAY PARED A LA IZQUIERDA Y HAY PAREDA AL FRENTE
        if (LIT ==1 && LIF==1){
          izquierda();
        }else{
          // NO HAY PARED A LA DERECHA Y HAY PAREDA AL FRENTE
          if ((LDF==1 && LDT==1) && (LIT ==0 && LIF==0)){
            derecha();
          }else{
            //HAY PARED EN TODAS LAS DIRECCIONES
             if ((LDF==0 && LDT==0) && (LIF==0 && LIT==0) ){
               giro_u();
             }else{
              //SI HAY UNA INDETERMINACION
                adelante();
             }
          
          }
        }
      }else{
         //No hay pared a la izquierda y no hay pared al frente
         if (inercia=="IM"){
            impulso();
         }else{
            if (LIT ==1 && LIF==1){
              izquierda_sin_fte();
           }else{
            adelante(); 
           }
         }
      }
    }else{
      if (inercia=="IZ"){
        izquierda();
      }else{
        if (inercia=="IZF"){
          izquierda_sin_fte();
        }else{
          if(inercia=="DER"){
            derecha();
          }else{
            if (inercia=="GU"){
              giro_u();
            }else{
              if (inercia=="IM"){
                impulso();
              }
            }
          }
        }
      }
    }
  }else{
    if(color == "NEGRO"){
      giro_u();
    }else{
      if (con_vi>0 && con_vi<5){
          Serial.print("victima ");
          Serial.println(con_vi);
          moverse(0,0);
          digitalWrite(led_vi,HIGH);
          con_vi=con_vi+1;
      }else{
          if (con_vi>=5){
              Serial.print("sali de victima");
              con_vi=0;
              digitalWrite(led_vi,LOW);
              adelante();
           }
      }
    }
  }
}

//***************************************************
//* Función para controlar Servo motores            *
//***************************************************

void adelante(){
  if(LDF==1){
      // ESTA PEGADO A LA PARED IZQUIERDA AJUSTA A LA DERECHA 
      if (MMIF<=60){
        moverse(2,3);
      }else{
        // ESTA ALEJADO DE LA PARED IZQUIERDA AJUSTA A LA IZQUIERDA 
        if (MMIF>=70){ //80
          moverse(3,2);
        }else{
          // SIGUE DE FRENTE 
          moverse(1,1);
        }
      }
  }else{
    if (MMIF==MMDF){
      moverse(1,1);
    }else{
      if(MMIF>MMDF){
        moverse(3,2);
      }else{
        if(MMDF>MMIF){
        moverse(2,3);
          }
        }
  }

  }
  /*
  // ESTA PEGADO A LA PARED IZQUIERDA AJUSTA A LA DERECHA 
  if (MMIF<=60){
    moverse(2,3);
  }else{
    // ESTA ALEJADO DE LA PARED IZQUIERDA AJUSTA A LA IZQUIERDA 
    if (MMIF>=70){ //80
      moverse(3,2);
    }else{
      // SIGUE DE FRENTE 
      moverse(1,1);
    }
  }*/
  
}


void detenerse(){
  moverse(0,0);
}



void izquierda(){
  if (ciclo<=giros_I){
    moverse(-1,1);
    inercia="IZ";
    ciclo++;
  }else{
    if (MDT>MDF){  
        moverse(-1,1);
        inercia="IZ";
        }
        else{
        if (MDT==MDF){
          moverse(1,1);
          inercia="IZ";
        }else{
          ciclo=0;
          moverse(1,1);
          inercia="IM";
        }
    /*}else{
        moverse(-1,1);
        inercia="IZ";
        ciclo+1;
    }*/
    }
  }
}

void izquierda_sin_fte(){
  if(avance_ISF < 4){
    if(LFT == 0){
      inercia="FT";
      avance_ISF=0;
      ciclo=0;
      }
      else{
        inercia="IZF";
        moverse(1,1);
        avance_ISF++;
        }
    }
    else{
      if (ciclo<=giros_ISF){
        moverse(-1,1);
        inercia="IZF";
        ciclo++;
      }else{
            ciclo=0;
            avance_ISF=0;
            moverse(1,1);
            inercia="IM";
        }
    }
}

void derecha(){ 
  if (ciclo<=giros_D){
    moverse(1,-1);
    inercia="DER";
    ciclo++;
  }else{
     if (MIT>MIF){
        moverse(1,-1);
        inercia="DER";
      }
      else{
        if (MIT==MIF){
          moverse(1,1);
          inercia="DER";
        }
        else{
          ciclo=0;
          moverse(1,1);
          inercia="FT";
        }
      }
    /*
    if (MIT<=MIF){   
        ciclo=0;
        moverse(1,1);
        inercia="IM";
        //inercia="FT"; 
    }else{
        moverse(-1,1);
        inercia="IZ";
        ciclo++;
    }
    */
  }
}

void giro_u(){
  if (ciclo<=54){
    moverse(-1,1);
    inercia="GU";
    ciclo++;
  }else{
    //if (MDT>=MDF+dif && MDT<=MDF+dif){
        ciclo=0;
        moverse(1,1);
        inercia="FT";
    /*}else{
        moverse(-1,1);
        inercia="IZ";
        ciclo+1;
    }*/
  }
}

void impulso(){
  if (ciclo<=imp){
    if(LFT==0){
      ciclo=0;
      inercia="FT";
    }else{
      if(LDT ==0 && LDF==0){
          // ESTA PEGADO A LA PARED IZQUIERDA AJUSTA A LA DERECHA 
          if (MMDF<=60){
            moverse(3,2);
          }else{
            // ESTA ALEJADO DE LA PARED IZQUIERDA AJUSTA A LA IZQUIERDA 
            if (MMDF>=70){ //80
              moverse(2,3);
            }else{
              // SIGUE DE FRENTE 
              moverse(1,1);
            }
         }
      }else{
         moverse(1,1);
        }
      inercia="IM";
   }
      ciclo++;
  }else{
    //moverse(0,0);
    //delay(5000);
    inercia="FT";
    ciclo=0;
    }
 }

void moverse(int vi, int vd){
  Serial.println(ciclo);
  int i1 = 90; //90 se detienen los motores 
  int i2 = 90;
  int d1 = 90;
  int d2 = 90;
  //vi = velocidad iquierda
  if (vi==0){
      int i1 = 90; //90 se detienen los motores 
      int i2 = 90;
  }else{
    if (vi==1){
      i1=48;         // para que giren en sentidos contrarios  a las manecillas del reloj
      i2=48;         // los valores se  ajustan para que giren parejos los motores 
    }else{
      if (vi==-1){
         i1=132;          // menor a 90 para motores derechos y mayor a 90 en izquierdos 
         i2=132;  
      }else{
         if (vi==2){
            mensaje="ajuste a la derecha ";
            i1=30;         // para que giren en sentidos contrarios  a las manecillas del reloj
            i2=30;         // los valores se  ajustan para que giren parejos los motores           
         }else{
          if (vi==3){
             i1=70;          // menor a 90 para motores derechos y mayor a 90 en izquierdos
             i2=70;         //  e impulsen el robot hacia adelant          
          }    
        }
      }
    }
  }
  
  //vd = velocidad derecha
  if (vd==0){
      int d1 = 90; //90 se detienen los motores 
      int d2 = 90;
  }else{
    if (vd==1){
      d1=132;          // menor a 90 para motores derechos y mayor a 90 en izquierdos
      d2=132;          //  e impulsen el robot hacia adelante
    }else{
      if (vd==-1){
        d1=48;         // para que giren en sentidos contrarios  a las manecillas del reloj
        d2=48;         // los valores se  ajustan para que giren parejos los motores        
      }else{
        if (vd==2){
          mensaje="ajuste a la izquierda ";
          d1=140;          // menor a 90 para motores derechos y mayor a 90 en izquierdo
          d2=140;          //  e impulsen el robot hacia adelant          
        }else{
          if (vd==3){
             d1=110;          // menor a 90 para motores derechos y mayor a 90 en izquierdos
             d2=110;          //  e impulsen el robot hacia adelant          
          }    
        }
      }
    }
  }

  mid.write(i1);
  mit.write(i2);
  mdd.write(d1);
  mdt.write(d2);
  
}

void Leer(){
  //  Distancia Ultrasonicos en cm 
    MIT = ultra(trig_it,echo_it);
    delay(5);
    MFT = ultra(trig_ft,echo_ft);
    delay(5);
    MDT = ultra(trig_dt,echo_dt);
    delay(5);
    MIF = ultra(trig_if,echo_if);
    delay(5);
    MDF = ultra(trig_df,echo_df);
   /*  Distancia Ultrasonicos en mm */
    MMIF = ultra_mm(trig_if,echo_if);
    delay(5);
    MMDF = ultra_mm(trig_df,echo_df);
    delay(5);
    MMIT = ultra_mm(trig_it,echo_it);
    delay(5);
    MMDT = ultra_mm(trig_dt,echo_dt);
    delay(5);
    MMFT = ultra_mm(trig_ft,echo_ft);
    delay(5);    

  //  Valores logicos Ultrasonicos 
    if(MMIT>LL){
      LIT=1;
    }else{
      LIT=0;
    }
    if(MMIF>LL){
      LIF=1;
    }else{
      LIF=0;
    }
    if(MMFT>LD){
      LFT=1;
    }else{
      LFT=0;
    }
    if(MMDF>LL){
      LDF=1;
    }else{
      LDF=0;
    }
    if(MMDT>LL){
      LDT=1;
    }else{
      LDT=0;
    }

      //  Lectura de Sensores de temperatura Infrarojos 
  
    tcaselect(2);
    ti_amb=tempI.readAmbientTempC();
    ti_obj=tempI.readObjectTempC();
    ti_dif = ti_obj - ti_amb;
    
    tcaselect(3);
    td_amb=tempD.readAmbientTempC();
    td_obj=tempD.readObjectTempC();
    td_dif = td_obj - td_amb;

    if (ti_obj>ti_amb+10){
        con_vi=1;
    }
    if (td_obj>td_amb+10){
        con_vi=1;
    }
   
    //  Lectura de girsocopio 
    tcaselect(1);
    giro.getAcceleration(&ax, &ay, &az);
    ang_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2)))*(180.0 / 3.14);
    ang_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2)))*(180.0 / 3.14);
    ang_z = atan(az / sqrt(pow(ax, 2) + pow(ay, 2)))*(180.0 / 3.14);

    // lectura sensores de piso 
    tcaselect(5);
    red_1 = RGB_1.readRed();
    green_1 = RGB_1.readGreen();
    blue_1 = RGB_1.readBlue();
    tcaselect(6);
    red_2 = RGB_2.readRed();
    green_2 = RGB_2.readGreen();
    blue_2 = RGB_2.readBlue();
    aserie();
    grabar();

    //Obtencion del color del piso 
    if(((red_1>lr1-10 and red_1<lr1+10) and (green_1>lg1-10 and green_1<lg1+10) and (blue_1>lb1-10 and blue_1<lb1+10)) or 
       ((red_2>lr2-10 and red_2<lr2+10) and (green_2>lg2-10 and green_2<lg2+10) and (blue_2>lb2-10 and blue_2<lb2+10))){
       color = "NEGRO";
    }else{
       color = "BLANCO";
    }
}

  void aserie(){
        if (ab_serial==1){
        Serial.print("ciclo ");Serial.println(ciclo);
        Serial.print("Distancias MM IT: ");Serial.print(MMIT);Serial.print("mm---");
        Serial.print("IF: ");Serial.print(MMIF);Serial.print("mm---");
        Serial.print("FT: ");Serial.print(MMFT);Serial.print("mm---");
        Serial.print("DF: ");Serial.print(MMDF);Serial.print("mm---");
        Serial.print("DT: ");Serial.print(MMDT);Serial.println("mm---");
        /*Serial.println("------------------------------------------------------------------------------------------------");
        Serial.print("Distancias MM IT: ");Serial.print(MMIT);Serial.print("mm---");
        Serial.print("IF: ");Serial.print(MMIF);Serial.print("mm---");
        Serial.print("FT: ");Serial.print(MMFT);Serial.print("mm---");
        Serial.print("DF: ");Serial.print(MMDF);Serial.print("mm---");
        Serial.print("DT: ");Serial.print(MMDT);Serial.println("mm---");
        Serial.println("------------------------------------------------------------------------------------------------");
        */
        Serial.print("Val.Log.      IT: ");Serial.print(LIT);
        Serial.print("IF: ");Serial.print(LIF);Serial.print("-----");
        Serial.print("FT: ");Serial.print(LFT);Serial.print("-----");
        Serial.print("DF: ");Serial.print(LDF);Serial.print("-----");
        Serial.print("DT: ");Serial.println(LDT);Serial.println("-----");
        Serial.println("------------------------------------------------------------------------------------------------");

        Serial.print("ciclo ");Serial.println(ciclo);
        Serial.print("Temperatura derecho Ambiente: "); Serial.print(ti_amb);Serial.print(" Objeto: "); Serial.print(ti_obj);Serial.print(" Diferencia: "); Serial.println(ti_dif);
        Serial.print("Temperatura izquierda Ambiente  : "); Serial.print(td_amb);Serial.print(" Objeto: "); Serial.print(td_obj);Serial.print(" Diferencia: "); Serial.println(td_dif);
        //Serial.println("------------------------------------------------------------------------------------------------");
        Serial.print("Angulo X: "); Serial.print(ang_x);Serial.print(" --- Y: "); Serial.println(ang_y);Serial.print(" --- Z: "); Serial.println(ang_z);
        Serial.println("------------------------------------------------------------------------------------------------");
        // Print out readings, change HEX to DEC if you prefer decimal output
        Serial.print("Red1: "); Serial.println(red_1/*,HEX*/);
        Serial.print("Green1: "); Serial.println(green_1/*,HEX*/);
        Serial.print("Blue1: "); Serial.println(blue_1/*,HEX*/);
        Serial.println();
        Serial.print("Red2: "); Serial.println(red_2,HEX);
        Serial.print("Green2: "); Serial.println(green_2,HEX);
        Serial.print("Blue2: "); Serial.println(blue_2,HEX);
        Serial.print("Color: "); Serial.println(color);
        Serial.println();
        //delay(5000);
    }
  }
//Función para grabar SD 
 
 void grabar(){
  if (ab_grabar==1){
      tiempo=millis();
      archivo = SD.open("Maze.txt",FILE_WRITE);
      archivo.write("************************************************************************************************");archivo.println("");
      archivo.print("ciclo ");archivo.println(ciclo);
      archivo.print("->  tiempo ");archivo.println(tiempo);
      archivo.print("Inercia ");archivo.println(inercia);
      archivo.print("Mensaje ");archivo.println(mensaje);
      archivo.print("Distancias MM IT: ");archivo.print(MMIT);archivo.print("mm---");
      archivo.print("IF: ");archivo.print(MMIF);archivo.print("mm---");
      archivo.print("FT: ");archivo.print(MMFT);archivo.print("mm---");
      archivo.print("DF: ");archivo.print(MMDF);archivo.print("mm---");
      archivo.print("DT: ");archivo.print(MMDT);archivo.println("mm---");
      archivo.print("Val.Log.      IT: ");archivo.print(LIT);
      archivo.print("IF: ");archivo.print(LIF);archivo.print("-----");
      archivo.print("FT: ");archivo.print(LFT);archivo.print("-----");
      archivo.print("DF: ");archivo.print(LDF);archivo.print("-----");
      archivo.print("DT: ");archivo.println(LDT);archivo.println("-----");
      archivo.print("RED1: ");archivo.print(red_1);archivo.print("-----");
      archivo.print("Green1: ");archivo.print(green_1);archivo.print("-----");
      archivo.print("Blue1: ");archivo.println(blue_1);archivo.println("-----");
      archivo.print("RED2: ");archivo.print(red_2);archivo.print("-----");
      archivo.print("Green2: ");archivo.print(green_2);archivo.print("-----");
      archivo.print("Blue2: ");archivo.println(blue_2);archivo.println("-----");
      archivo.print("Color: ");archivo.println(color);archivo.println("-----");
      
      archivo.println("------------------------------------------------------------------------------------------------");
      //archivo.write("------------------------------------------------------------------------------------------------");archivo.println("");
      //archivo.write("Temperatura Izquierdo Ambiente: ");archivo.print(ti_amb);archivo.print(" Objeto: ");archivo.print(ti_obj);archivo.print(" Diferencia: ");archivo.println(ti_dif);
      //archivo.write("Temperatura Derecho Ambiente:   ");archivo.print(td_amb);archivo.print(" Objeto: ");archivo.print(td_obj);archivo.print(" Diferencia: ");archivo.println(td_dif);
      //archivo.write("------------------------------------------------------------------------------------------------");archivo.println("");
      //archivo.write("Angulo X: ");archivo.print(ang_x);archivo.print(" --- Y: ");archivo.println(ang_y);
      //archivo.write("------------------------------------------------------------------------------------------------");archivo.println("");
      archivo.close(); 
  }
 
  
} 
/*  Función para lectura de ultrasónicos  */

long ultra(int trigPin,int echoPin){
  long t=0;
  long d=0;

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  t=pulseIn(echoPin,HIGH);
  d = t/59;

  return d;
}

/*  Función para lectura de ultrasónicos en milimetros  */

long ultra_mm(int trigPin,int echoPin){
  float t=0;
  float d=0;

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  t=pulseIn(echoPin,HIGH);
  d= 340000*t/2000000;
  //d = t/59;

  return d;
}
