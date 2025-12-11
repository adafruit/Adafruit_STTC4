/*!
 * @file Adafruit_STCC4.cpp
 *
 * @mainpage Adafruit STCC4 CO2 sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's STCC4 driver for the
 * Arduino platform. It is designed specifically to work with the
 * Adafruit STCC4 breakout: https://www.adafruit.com/product/xxxx
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * for Adafruit Industries.
 *
 * @section license License
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_STCC4.h"

/*!
 * @brief Instantiates a new STCC4 class
 */
Adafruit_STCC4::Adafruit_STCC4() {
  i2c_dev = nullptr;
}

/*!
 * @brief Destructor for STCC4 class
 */
Adafruit_STCC4::~Adafruit_STCC4() {
  if (i2c_dev) {
    delete i2c_dev;
  }
}

/*!
 * @brief Initializes the STCC4 sensor
 * @param i2c_addr The I2C address of the sensor (default 0x64)
 * @param wire The Wire object to use for I2C communication (default &Wire)
 * @return true if initialization was successful, false otherwise
 */
bool Adafruit_STCC4::begin(uint8_t i2c_addr, TwoWire* wire) {
  if (i2c_dev) {
    delete i2c_dev;
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_addr, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Perform soft reset before detection
  reset();

  // Verify product ID using helper function
  uint32_t product_id = getProductID();
  if (product_id != STCC4_PRODUCT_ID) {
    return false;
  }

  return true;
}

/*!
 * @brief Calculate CRC-8 checksum for STCC4 data
 * @param data Pointer to data bytes
 * @param len Number of bytes to calculate CRC for
 * @return CRC-8 checksum
 */
uint8_t Adafruit_STCC4::crc8(const uint8_t* data, uint8_t len) {
  uint8_t crc = 0xFF;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}

/*!
 * @brief Perform soft reset of the sensor
 * @return true if reset was successful, false otherwise
 */
bool Adafruit_STCC4::reset() {
  uint8_t reset_addr = 0x00;
  uint8_t reset_cmd = STCC4_CMD_PERFORM_SOFT_RESET;

  if (!i2c_dev->write(&reset_cmd, 1, true, &reset_addr)) {
    return false;
  }

  delay(10); // Wait for reset to complete (10ms per datasheet)
  return true;
}

/*!
 * @brief Enter or exit sleep mode
 * @param enable true to enter sleep mode, false to exit sleep mode
 * @return true if operation was successful, false otherwise
 */
bool Adafruit_STCC4::sleepMode(bool enable) {
  if (enable) {
    // Enter sleep mode
    if (!writeCommand(STCC4_CMD_ENTER_SLEEP_MODE)) {
      return false;
    }
    delay(1); // Wait for sleep command execution (1ms per datasheet)
  } else {
    // Exit sleep mode
    uint8_t wake_byte = STCC4_CMD_EXIT_SLEEP_MODE;
    if (!i2c_dev->write(&wake_byte, 1)) {
      return false;
    }
    delay(5); // Wait for wake up (5ms per datasheet)
  }
  return true;
}

/*!
 * @brief Enable or disable continuous measurement with 1s sampling interval
 * @param enable true to start continuous measurement, false to stop
 * @return true if command was sent successfully, false otherwise
 */
bool Adafruit_STCC4::enableContinuousMeasurement(bool enable) {
  if (enable) {
    return writeCommand(STCC4_CMD_START_CONTINUOUS_MEASUREMENT);
  } else {
    return writeCommand(STCC4_CMD_STOP_CONTINUOUS_MEASUREMENT);
  }
}

/*!
 * @brief Perform single shot measurement
 * @return true if command was sent successfully, false otherwise
 */
bool Adafruit_STCC4::measureSingleShot() {
  return writeCommand(STCC4_CMD_MEASURE_SINGLE_SHOT);
}

/*!
 * @brief Perform sensor conditioning to improve initial CO2 sensing performance
 * @return true if command was sent successfully, false otherwise
 */
bool Adafruit_STCC4::performConditioning() {
  if (!writeCommand(STCC4_CMD_PERFORM_CONDITIONING)) {
    return false;
  }
  delay(22000); // Wait 22 seconds for conditioning to complete
  return true;
}

/*!
 * @brief Perform factory reset to clear FRC and ASC algorithm history
 * @return true if command was sent successfully, false otherwise
 */
bool Adafruit_STCC4::factoryReset() {
  if (!writeCommand(STCC4_CMD_PERFORM_FACTORY_RESET)) {
    return false;
  }
  delay(90); // Wait 90ms for factory reset to complete
  return true;
}

/*!
 * @brief Perform sensor self-test
 * @param result Pointer to store self-test result
 * @return true if self-test was performed successfully, false otherwise
 */
bool Adafruit_STCC4::performSelfTest(uint16_t* result) {
  uint8_t cmd[2] = {(uint8_t)(STCC4_CMD_PERFORM_SELF_TEST >> 8),
                    (uint8_t)(STCC4_CMD_PERFORM_SELF_TEST & 0xFF)};
  uint8_t data[3]; // 2 bytes + 1 CRC

  if (!i2c_dev->write(cmd, 2)) {
    return false;
  }

  delay(360); // Wait for self test to complete (360ms per datasheet)

  if (!i2c_dev->read(data, 3)) {
    return false;
  }

  // Verify CRC
  if (crc8(&data[0], 2) != data[2]) {
    return false;
  }

  *result = (data[0] << 8) | data[1];
  return true;
}

/*!
 * @brief Write command to sensor
 * @param command 16-bit command code
 * @return true if command was sent successfully, false otherwise
 */
bool Adafruit_STCC4::writeCommand(uint16_t command) {
  uint8_t cmd[2] = {(uint8_t)(command >> 8), (uint8_t)(command & 0xFF)};
  return i2c_dev->write(cmd, 2);
}

/*!
 * @brief Read command response with CRC validation
 * @param command 16-bit command code
 * @param data Buffer to store response data
 * @param len Expected response length in bytes
 * @return true if read successful and CRC valid, false otherwise
 */
bool Adafruit_STCC4::readCommand(uint16_t command, uint8_t* data, uint8_t len) {
  uint8_t cmd[2] = {(uint8_t)(command >> 8), (uint8_t)(command & 0xFF)};

  if (!i2c_dev->write_then_read(cmd, 2, data, len, true)) {
    return false;
  }

  // Validate CRC for all data words (every 3 bytes: 2 data + 1 CRC)
  for (uint8_t i = 0; i < len; i += 3) {
    if (crc8(&data[i], 2) != data[i + 2]) {
      return false; // CRC mismatch
    }
  }

  return true;
}

/*!
 * @brief Read measurement data from sensor
 * @param co2 Pointer to store CO2 concentration in ppm
 * @param temperature Pointer to store temperature in degrees C
 * @param humidity Pointer to store relative humidity in %
 * @param status Pointer to store sensor status word
 * @return true if measurement was read successfully, false otherwise
 */
bool Adafruit_STCC4::readMeasurement(uint16_t* co2, float* temperature,
                                     float* humidity, uint16_t* status) {
  uint8_t data[12]; // 4 x (2 bytes + 1 CRC)

  if (!readCommand(STCC4_CMD_READ_MEASUREMENT, data, 12)) {
    return false;
  }

  // Extract data (MSB first)
  uint16_t co2_raw = (data[0] << 8) | data[1];
  uint16_t temp_raw = (data[3] << 8) | data[4];
  uint16_t hum_raw = (data[6] << 8) | data[7];
  uint16_t status_raw = (data[9] << 8) | data[10];

  // Convert raw values using datasheet formulas
  *co2 = co2_raw;                                     // CO2 is direct ppm value
  *temperature = (temp_raw * 175.0 / 65536.0) - 45.0; // Temperature conversion
  *humidity = (hum_raw * 125.0 / 65536.0) - 6.0;      // Humidity conversion
  *status = status_raw;

  return true;
}

/*!
 * @brief Get the product ID from the sensor
 * @return 32-bit product ID, or 0 if read failed
 */
uint32_t Adafruit_STCC4::getProductID() {
  uint8_t data[6]; // First 2 words only (2 bytes + CRC each)

  if (!readCommand(STCC4_CMD_GET_PRODUCT_ID, data, 6)) {
    return 0; // Return 0 on failure
  }

  // Extract product ID from first 4 bytes (MSB first)
  uint32_t product_id = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                        ((uint32_t)data[3] << 8) | data[4];

  return product_id;
}
