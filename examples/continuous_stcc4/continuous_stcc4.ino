// Continuous measurement example for Adafruit STCC4 CO2 sensor

#include <Adafruit_STCC4.h>

Adafruit_STCC4 sttc4;

void printStatus(uint16_t status) {
  Serial.print(F("Status: 0x"));
  Serial.print(status, HEX);
  Serial.print(F(" ("));

  bool first = true;
  if (status & STCC4_STATUS_VOLTAGE_ERROR) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("VOLTAGE_ERROR"));
    first = false;
  }
  if (status & STCC4_STATUS_DEBUG_MASK) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("DEBUG"));
    first = false;
  }
  if (status & STCC4_STATUS_SHT_NOT_CONNECTED) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("SHT_NOT_CONNECTED"));
    first = false;
  }
  if (status & STCC4_STATUS_MEMORY_ERROR_MASK) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("MEMORY_ERROR"));
    first = false;
  }
  if (status & STCC4_STATUS_TESTING_MODE) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("TESTING_MODE"));
    first = false;
  }
  if (first) {
    Serial.print(F("OK"));
  }
  Serial.println(F(")"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit STCC4 test"));

  if (!sttc4.begin()) {
    Serial.println(F("Failed to find STCC4 chip"));
    while (1) delay(10);
  }

  Serial.println(F("STCC4 found!"));

  if (!sttc4.reset()) {
    Serial.println(F("Failed to reset STCC4"));
    while (1) delay(10);
  }
  Serial.println(F("Reset successful"));

  // Test getProductID function after reset
  uint32_t productID = sttc4.getProductID();
  Serial.print(F("Product ID: 0x"));
  Serial.println(productID, HEX);

  // Uncomment to perform factory reset (clears calibration history)
  // Serial.println(F("Performing factory reset..."));
  // if (sttc4.factoryReset()) {
  //   Serial.println(F("Factory reset complete"));
  // } else {
  //   Serial.println(F("Factory reset failed"));
  // }

  if (!sttc4.enableContinuousMeasurement(true)) {
    Serial.println(F("Failed to start continuous measurement"));
    while (1) delay(10);
  }
  Serial.println(F("Continuous measurement started"));

  // Uncomment to perform conditioning (takes 22 seconds)
  // Serial.println(F("Performing conditioning..."));
  // if (sttc4.performConditioning()) {
  //   Serial.println(F("Conditioning complete"));
  // } else {
  //   Serial.println(F("Conditioning failed"));
  // }
}

void loop() {
  uint16_t co2;
  float temperature, humidity;
  uint16_t status;

  if (sttc4.readMeasurement(&co2, &temperature, &humidity, &status)) {
    Serial.print(F("CO2: "));
    Serial.print(co2);
    Serial.print(F(" ppm, Temp: "));
    Serial.print(temperature);
    Serial.print(F(" C, Humidity: "));
    Serial.print(humidity);
    Serial.print(F(" %, "));
    printStatus(status);
  } else {
    Serial.println(F("Failed to read measurement"));
  }

  delay(1000);
}
