#include "I2Cdev.h" // coneccion i2c
#include "MPU6050.h" // acelerometro y giroscopio 6 ejes
//#include <HMC5883L_Simple.h> // magnometro
#include <math.h> // matematicas broooooo

MPU6050 accelgyro;
//HMC5883L_Simple Compass;

#define FREQ  30.0 // sample freq in Hz

// global angle, gyro derived
double gSensitivity = 65.5; // for 500 deg/s, check data sheet
double gx = 0, gy = 0, gz = 0;
double gyrX = 0, gyrY = 0, gyrZ = 0;
int16_t accX = 0, accY = 0, accZ = 0;

double gyrXoffs = -281.00, gyrYoffs = 18.00, gyrZoffs = -83.00;

void setup() {
  Serial.begin(38400);
  Wire.begin();

  // initialize devices
  Serial.println("Initializing I2C devices...");

  // initialize mpu6050
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setI2CBypassEnabled(true); // set bypass mode for gateway to hmc5883L
  
  /*// initialize hmc5883l
  Compass.SetDeclination(23, 35, 'E');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);*/

}

void loop() {    
  // read raw accel/gyro measurements from device
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  // display tab-separated accel/gyro x/y/z values
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);

  /*float heading = Compass.GetHeadingDegrees();
  Serial.print("Heading: \t");
  Serial.println( heading );*/

  delay(500);

}
