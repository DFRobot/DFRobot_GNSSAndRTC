/*!
 * @file DFRobot_GNSSAndRTC.h
 * @brief Define the basic structure of class DFRobot_GNSSAndRTC
 * @details Define SD3031 and L76K functions
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [qsjhyy](yihuan.huang@dfrobot.com)
 * @version V1.0
 * @date 2022-08-30
 * @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
 */
#ifndef _DFRobot_GNSS_AND_RTC_H_
#define _DFRobot_GNSS_AND_RTC_H_

#include "Arduino.h"
#include "Wire.h"

#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
#include "SoftwareSerial.h"
#else
#include "HardwareSerial.h"
#endif

#include "SD3031/DFRobot_SD3031.h"
#include "L76K/DFRobot_GNSS.h"

// #define ENABLE_DBG ///< Enable this macro to see the detailed running process of the program
#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif


class DFRobot_GNSSAndRTC:public DFRobot_GNSS, public DFRobot_SD3031
{
public:
#define MODULE_I2C_ADDRESS     0x66     ///< Sensor device address

// Custom function registers 0x24 ~ 0x2F
#define REG_CALIB_STATUS_REG   0x2AU
#define REG_CALIB_RTC_REG      0x2BU

#define REG_RTC_READ_REG       0x2EU
#define REG_RTC_READ_LEN       0x2FU

// Custom function registers 0xAA ~ (0xB0 - 1)
#define REG_CS32_PID           0xAAU
#define REG_CS32_VID           0xACU
#define REG_CS32_VERSION       0xAEU

#define MODULE_DFR1103_PID     0x444F   ///< Sensor PID
#define MODULE_DFR1103_VID     0x3343   ///< Sensor VID
#define MODULE_DFR1103_VERSION 0x0100   ///< Sensor VERSION

/**
 * @enum eCalibRTCStatus_t
 * @brief RTC 模块校准状态
 */
  typedef enum {
    eCalibNone = 0x00,
    eCalibComplete = 0x01,
    eUnderCalib = 0x02,
  }eCalibRTCStatus_t;

public:
  /**
   * @fn DFRobot_GNSSAndRTC
   * @brief Constructor
   * @return None
   */
  DFRobot_GNSSAndRTC();

  /**
   * @fn begin
   * @brief subclass initialization function
   * @return bool type, means returning initialization status
   * @retval true NO_ERROR
   */
  virtual bool begin(void);

  /**
   * @fn calibRTC(void)
   * @brief Calibrate RTC immediately with GNSS
   * @note This is a single calibration;
   * @n If the GNSS module signal is weak, time calibration may encounter issues.
   * @return None
   */
  void calibRTC(void);

  /**
   * @fn calibRTC(uint8_t hour)
   * @brief The loop automatically performs GNSS timing based on the set interval
   * @param hour Automatic calibration of the time interval. range: 0~255, unit: hour.
   * @note When set to zero, automatic time calibration is disabled.
   * @n Enabling it will trigger an immediate calibration.
   * @n If the GNSS module signal is weak, time calibration may encounter issues.
   * @return None
   */
  void calibRTC(uint8_t hour);

  /**
   * @fn calibStatus
   * @brief Current clock calibration status
   * @param mode By default, it is set to true, indicating access to the calibration status only.
   * @n  If continuous calibration for one minute does not return a successful calibration,
   * @n  you can pass in false to manually terminate this calibration session.
   * @return uint8_t type, indicates current clock calibration status
   * @retval 0 Not calibrated
   * @retval 1 Calibration complete
   * @retval 2 Under calibration
   * @note Note: To avoid affecting subsequent calibration status,
   * @n    "Calibration completed Status (1)" is automatically zeroed after a successful read
   */
  uint8_t calibStatus(bool mode = true);

private:

  /**
   * @fn writeReg
   * @brief Write register function, designed as virtual function, implemented by derived class
   * @param reg  Register address 8bits
   * @param pBuf Storage and buffer for data to be written
   * @param size Length of data to be written
   * @return None
   */
  void writeReg(uint8_t reg, void* pBuf, size_t size) = 0;

  /**
   * @fn readReg
   * @brief Read register function, designed as virtual function, implemented by derived class
   * @param reg  Register address 8bits
   * @param pBuf Storage and buffer for data to be read
   * @param size Length of data to be read
   * @return uint8_t type, indicates returning read register status
   * @retval 0 Reading succeeded
   * @retval 1 Reading failed
   */
  uint8_t readReg(uint8_t reg, void* pBuf, size_t size) = 0;

};

class DFRobot_GNSSAndRTC_I2C:public DFRobot_GNSSAndRTC
{
public:
  DFRobot_GNSSAndRTC_I2C(TwoWire* pWire = &Wire, uint8_t addr = MODULE_I2C_ADDRESS);
  bool begin(void);
protected:
  virtual void writeReg(uint8_t reg, void* pBuf, size_t size);
  virtual uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
private:
  TwoWire* _pWire;
  uint8_t _deviceAddr;
};

class DFRobot_GNSSAndRTC_UART:public DFRobot_GNSSAndRTC
{
public:
#define UART_BAUDRATE        ((uint32_t)57600)

#define UART0_READ_REGBUF    0xBB
#define UART0_WRITE_REGBUF   0xCC

public:
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
  DFRobot_GNSSAndRTC_UART(SoftwareSerial* sSerial, uint32_t Baud = UART_BAUDRATE);
#else
  DFRobot_GNSSAndRTC_UART(HardwareSerial* hSerial, uint32_t Baud = UART_BAUDRATE, uint8_t rxpin = 0, uint8_t txpin = 0);
#endif

  bool begin(void);
protected:
  virtual void writeReg(uint8_t reg, void* pBuf, size_t size);
  virtual uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
private:

#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
  SoftwareSerial* _serial;
#else
  HardwareSerial* _serial;
#endif
  uint32_t _baud;
  uint8_t _rxpin;
  uint8_t _txpin;
};

#endif/* _DFRobot_GNSS_AND_RTC_H_ */
