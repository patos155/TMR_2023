#include <SPI.h>
#include <LoRa.h>

#define SS_PIN 10
#define RST_PIN 9
#define DI0_PIN 2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);

  if (!LoRa.begin(915E6)) {
    Serial.println("Error al iniciar el módulo LoRa");
    while (1);
  }
}

void loop() {
  String mensaje;
  Serial.print("Escribe un mensaje: ");
  while (Serial.available()) {
    mensaje = Serial.readString();
  }

  if (mensaje != "") {
    LoRa.beginPacket();
    LoRa.print(mensaje);
    LoRa.endPacket();
    Serial.println("Mensaje enviado: " + mensaje);
  }

  delay(100);
}
