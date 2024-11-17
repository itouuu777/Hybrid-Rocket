#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);

  // BNO055の初期化
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);

  if (!bme.begin(0x76, &Wire)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  } 

  delay(1000);
}

void loop() {
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  delay(500);
}