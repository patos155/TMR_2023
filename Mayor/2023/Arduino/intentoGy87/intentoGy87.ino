#include "I2Cdev.h"
#include "MPU6050.h"
#include <Adafruit_BMP085.h>
#include <HMC5883L_Simple.h>

#define spamtimer(t) for (static uint32_t SpamTimer; (uint32_t)(millis() - SpamTimer) >= (t); SpamTimer = millis())

MPU6050 accelgyro;
Adafruit_BMP085 bmp; // barometro
HMC5883L_Simple Compass; // magnometro

int16_t ax, ay, az;
int16_t gx, gy, gz;

//#define LED_PIN 13
//bool blinkState = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // initialize devices
  Serial.println("Initializing I2C devices...");

  // initialize bmp085
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  // initialize mpu6050
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setI2CBypassEnabled(true); // set bypass mode for gateway to hmc5883L
  
  
  // initialize hmc5883l
  Compass.SetDeclination(49, 23, 'E');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);


  // configure Arduino LED for checking activity
  //pinMode(LED_PIN, OUTPUT);
}

void loop() {
  /*Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");*/
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  /*Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");
  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(101500));
  Serial.println(" meters");*/

  
  // read raw accel/gyro measurements from device
  /*accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  // display tab-separated accel/gyro x/y/z values
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);*/

  spamtimer(100) {
    float heading = Compass.GetHeadingDegrees();
    Serial.print("Heading: \t");
    Serial.println( heading );
  }


  /*// blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);*/

  //delay(500);
}