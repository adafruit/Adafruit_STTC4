// Single shot measurement example for Adafruit STCC4 CO2 sensor
// Uses sleep/wake cycle for power saving

#include <Adafruit_STCC4.h>

Adafruit_STCC4 stcc4;

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

  Serial.println(F("Adafruit STCC4 Single Shot Test"));

  if (!stcc4.begin()) {
    Serial.println(F("Failed to find STCC4 chip"));
    while (1) delay(10);
  }

  Serial.println(F("STCC4 found!"));

  // Test getProductID function
  uint32_t productID = stcc4.getProductID();
  Serial.print(F("Product ID: 0x"));
  Serial.println(productID, HEX);
}

void loop() {
  // Wake up from sleep mode
  Serial.println(F("Waking up sensor..."));
  if (!stcc4.sleepMode(false)) {
    Serial.println(F("Failed to wake sensor"));
    delay(100);
    return;
  }

  // Perform single shot measurement
  Serial.println(F("Starting single shot measurement..."));
  if (!stcc4.measureSingleShot()) {
    Serial.println(F("Failed to start single shot measurement"));
    delay(100);
    return;
  }

  // Wait for measurement to complete (500ms per datasheet)
  delay(500);

  // Read measurement
  uint16_t co2;
  float temperature, humidity;
  uint16_t status;

  if (stcc4.readMeasurement(&co2, &temperature, &humidity, &status)) {
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

  // Put sensor to sleep to save power
  Serial.println(F("Putting sensor to sleep..."));
  if (!stcc4.sleepMode(true)) {
    Serial.println(F("Failed to put sensor to sleep"));
  }

  Serial.println(F("Waiting 10 seconds..."));
  Serial.println();
  
  // Wait 10 seconds before next measurement
  delay(10000);
}