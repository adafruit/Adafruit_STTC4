// Self test example for Adafruit STCC4 CO2 sensor
// Performs factory reset, conditioning, and self test

#include <Adafruit_STCC4.h>

Adafruit_STCC4 stcc4;

void printSelfTestResult(uint16_t result) {
  Serial.print(F("Self-test result: 0x"));
  Serial.print(result, HEX);
  Serial.print(F(" ("));

  if (result == 0x0000 || result == 0x0010) {
    Serial.println(F("PASS)"));
    return;
  }

  Serial.print(F("FAIL - "));
  bool first = true;
  
  if (result & STCC4_STATUS_VOLTAGE_ERROR) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("VOLTAGE_ERROR"));
    first = false;
  }
  if (result & STCC4_STATUS_DEBUG_MASK) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("DEBUG"));
    first = false;
  }
  if (result & STCC4_STATUS_SHT_NOT_CONNECTED) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("SHT_NOT_CONNECTED"));
    first = false;
  }
  if (result & STCC4_STATUS_MEMORY_ERROR_MASK) {
    if (!first) Serial.print(F(", "));
    Serial.print(F("MEMORY_ERROR"));
    first = false;
  }
  Serial.println(F(")"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit STCC4 Self Test"));

  if (!stcc4.begin()) {
    Serial.println(F("Failed to find STCC4 chip"));
    while (1) delay(10);
  }

  Serial.println(F("STCC4 found!"));

  // Get product ID
  uint32_t productID = stcc4.getProductID();
  Serial.print(F("Product ID: 0x"));
  Serial.println(productID, HEX);

  // Perform factory reset
  Serial.println(F("Performing factory reset..."));
  if (!stcc4.factoryReset()) {
    Serial.println(F("Factory reset failed"));
    while (1) delay(10);
  }
  Serial.println(F("Factory reset complete"));

  // Perform conditioning
  Serial.println(F("Performing conditioning (22 seconds)..."));
  if (!stcc4.performConditioning()) {
    Serial.println(F("Conditioning failed"));
    while (1) delay(10);
  }
  Serial.println(F("Conditioning complete"));

  // Perform self test
  Serial.println(F("Performing self test..."));
  uint16_t selfTestResult;
  if (!stcc4.performSelfTest(&selfTestResult)) {
    Serial.println(F("Self test failed to execute"));
    while (1) delay(10);
  }
  
  printSelfTestResult(selfTestResult);
  
  if (selfTestResult != 0x0000 && selfTestResult != 0x0010) {
    Serial.println(F("Self test FAILED - halting"));
    while (1) delay(10);
  }

  Serial.println(F("Self test procedure complete - PASSED"));
}

void loop() {
  delay(1000);
}