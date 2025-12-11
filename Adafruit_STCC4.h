/*!
 * @file Adafruit_STCC4.h
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
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * for Adafruit Industries.
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_STCC4_H
#define _ADAFRUIT_STCC4_H

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Arduino.h>
#include <Wire.h>

/** Default I2C address for the STCC4 */
#define STCC4_DEFAULT_ADDR 0x64

/** Command: Start continuous measurement */
#define STCC4_CMD_START_CONTINUOUS_MEASUREMENT 0x218B
/** Command: Stop continuous measurement */
#define STCC4_CMD_STOP_CONTINUOUS_MEASUREMENT 0x3F86
/** Command: Read measurement */
#define STCC4_CMD_READ_MEASUREMENT 0xEC05
/** Command: Set RHT compensation */
#define STCC4_CMD_SET_RHT_COMPENSATION 0xE000
/** Command: Set pressure compensation */
#define STCC4_CMD_SET_PRESSURE_COMPENSATION 0xE016
/** Command: Measure single shot */
#define STCC4_CMD_MEASURE_SINGLE_SHOT 0x219D
/** Command: Enter sleep mode */
#define STCC4_CMD_ENTER_SLEEP_MODE 0x3650
/** Command: Exit sleep mode payload byte */
#define STCC4_CMD_EXIT_SLEEP_MODE 0x00
/** Command: Perform conditioning */
#define STCC4_CMD_PERFORM_CONDITIONING 0x29BC
/** Command: Perform soft reset */
#define STCC4_CMD_PERFORM_SOFT_RESET 0x06
/** Command: Perform factory reset */
#define STCC4_CMD_PERFORM_FACTORY_RESET 0x3632
/** Command: Perform self test */
#define STCC4_CMD_PERFORM_SELF_TEST 0x278C
/** Command: Enable testing mode */
#define STCC4_CMD_ENABLE_TESTING_MODE 0x3FBC
/** Command: Disable testing mode */
#define STCC4_CMD_DISABLE_TESTING_MODE 0x3F3D
/** Command: Perform forced recalibration */
#define STCC4_CMD_PERFORM_FORCED_RECALIBRATION 0x362F
/** Command: Get product ID */
#define STCC4_CMD_GET_PRODUCT_ID 0x365B

/** Expected product ID */
#define STCC4_PRODUCT_ID 0x0901018A

/** Status bit flags */
#define STCC4_STATUS_VOLTAGE_ERROR 0x0001     ///< Supply voltage error
#define STCC4_STATUS_DEBUG_MASK 0x000E        ///< Debug flags mask
#define STCC4_STATUS_SHT_NOT_CONNECTED 0x0010 ///< SHT sensor not connected
#define STCC4_STATUS_MEMORY_ERROR_MASK 0x0060 ///< Memory error flags mask
#define STCC4_STATUS_TESTING_MODE 0x4000      ///< Testing mode active

/*!
 * @brief Class that stores state and functions for interacting with
 * the STCC4 CO2 sensor
 */
class Adafruit_STCC4 {
 public:
  Adafruit_STCC4();
  ~Adafruit_STCC4();

  bool begin(uint8_t i2c_addr = STCC4_DEFAULT_ADDR, TwoWire* wire = &Wire);
  bool reset();
  bool sleepMode(bool enable);
  bool enableContinuousMeasurement(bool enable);
  bool measureSingleShot();
  bool readMeasurement(uint16_t* co2, float* temperature, float* humidity,
                       uint16_t* status);
  bool performConditioning();
  bool factoryReset();
  bool performSelfTest(uint16_t* result);
  uint32_t getProductID();

 private:
  Adafruit_I2CDevice* i2c_dev;
  uint8_t crc8(const uint8_t* data, uint8_t len);
  bool writeCommand(uint16_t command);
  bool readCommand(uint16_t command, uint8_t* data, uint8_t len);
};

#endif // _ADAFRUIT_STCC4_H
