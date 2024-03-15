/*!
 * @file DFRobot_SD3031.h
 * @brief Define the basic structure of class DFRobot_SD3031
 * @details Define SD3031 functions
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-07-25
 * @url https://github.com/DFRobot/DFRobot_SD3031
 */
#ifndef _DFROBOT_SD3031_H
#define _DFROBOT_SD3031_H

#include "Arduino.h"
#include "Wire.h"


class DFRobot_SD3031
{
public:

  /* The address is offset 0x30 because it shares a register with the GNSS module */
#define SD3031_REG_SEC            (0x00 + 0x30)   ///< RTC Seconds Register
#define SD3031_REG_MIN            (0x01 + 0x30)   ///< RTC Minutes Register
#define SD3031_REG_HOUR           (0x02 + 0x30)   ///< RTC Hours Register
#define SD3031_REG_WEEK           (0x03 + 0x30)   ///< RTC Week Register
#define SD3031_REG_DAY            (0x04 + 0x30)   ///< RTC Day Register
#define SD3031_REG_MONTE          (0x05 + 0x30)   ///< RTC Month Register
#define SD3031_REG_YEAR           (0x06 + 0x30)   ///< RTC Year Register
#define SD3031_REG_ALARM_SEC      (0x07 + 0x30)   ///< RTC Seconds Alarm Register
#define SD3031_REG_ALARM_MIN      (0x08 + 0x30)   ///< RTC Minutes Alarm Register
#define SD3031_REG_ALARM_HOUR     (0x09 + 0x30)   ///< RTC Hours Alarm Register
#define SD3031_REG_ALARM_WEEK     (0x0A + 0x30)   ///< RTC Week Alarm Register
#define SD3031_REG_ALARM_DAY      (0x0B + 0x30)   ///< RTC Day Alarm Register
#define SD3031_REG_ALARM_MONNTE   (0x0C + 0x30)   ///< RTC Month Alarm Register
#define SD3031_REG_ALARM_YEAR     (0x0D + 0x30)   ///< RTC Year Alarm Register
#define SD3031_REG_ALARM_CON      (0x0E + 0x30)   ///< RTC Alarm Control Register
#define SD3031_REG_CTR1           (0x0F + 0x30)   ///< Control Register 1
#define SD3031_REG_CTR2           (0x10 + 0x30)   ///< Control Register 2
#define SD3031_REG_CTR3           (0x11 + 0x30)   ///< Control Register 3
#define SD3031_REG_COUNTDOWM      (0X13 + 0x30)   ///< Countdown Register
#define SD3031_REG_TEMP           (0x16 + 0x30)   ///< Internal Temperature Register
#define SD3031_REG_I2C_CON        (0x17 + 0x30)   ///< I2C Control
#define SD3031_REG_BAT_VAL        (0x1A + 0x30)   ///< Battery Level

public:
  /**
   * @struct sTimeData_t
   * @brief Structure for storing time data
   */
  typedef struct {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    String   week;
    uint8_t  hour;
    uint8_t minute;
    uint8_t second;
  }sTimeData_t;

  /**
   * @enum  eHours_t
   * @brief  e24hours, e12hours
   */
  typedef enum {
    e12hours = 0,
    e24hours = 1 << 7,
  }eHours_t;

  /**
   * @enum eWeek_t
   * @brief Enumerate week definition
   */
  typedef enum {
    eSunday = 0x01,
    eMonday = 0x02,
    eTuesday = 0x04,
    eWednesday = 0x08,
    eThursday = 0x10,
    eFriday = 0x20,
    eSaturday = 0x40,
    eEveryDay = 0x7f,
    eWorkday = 0x3e,
  }eWeek_t;


  /**
   * @enum  eTrigger_t
   * @brief  Enumerate interrupt definition
   */
  typedef enum {
    eYearEnable = 0x40,
    eMondayEnable = 0x20,
    eDayEnable = 0x10,
    eHoursEnable = 0x04,
    eMinuteEnable = 0x02,
    eSecondEnable = 0x01,
  }eTrigger_t;

public:

  /**
   * @fn DFRobot_SD3031
   * @brief Constructor
   * @return None
   */
  DFRobot_SD3031();

  /**
   * @fn getRTCTime
   * @brief Get information of year in RTC module
   * @return Return the obtained year
   */
  sTimeData_t getRTCTime(void);

  /**
   * @brief Set clock as 24-hour or 12-hour format
   * @param mode Clock calculation method
   */
  void setHourSystem(eHours_t mode);

  /**
   * @fn setTime
   * @brief Set time into rtc and take effect immediately
   * @param year 2000~2099
   * @param month 1~12
   * @param day 1~31
   * @param hour 0~23
   * @param minute 0~59
   * @param second 0~59
   * @return None
   */
  void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

  /**
   * @fn setAlarm
   * @brief Set the data for triggering alarm
   * @param year 2000~2099
   * @param month 1~12
   * @param day 1~31
   * @return None
   */
  void setAlarm(uint16_t year, uint8_t month, uint8_t day);
  /**
   * @fn setAlarm
   * @brief Set the Alarmnumber object
   * @param week eWeek_t
   * @param hour 0~23
   * @param minute 0~59
   * @param second 0~59
   */
  void setAlarm(eWeek_t week, uint8_t hour, uint8_t minute, uint8_t second);

  /**
   * @brief Get internal temperature of the clock
   * @return Return the obtained temperature, unit: ℃
   */
  int8_t getTemperatureC(void);

  /**
   * @brief Get voltage of onboard battery
   * @return float Return the obtained voltage
   */
  float getVoltage(void);

  /**
   * @brief Clear alarm flag bit
   */
  void clearAlarm(void);

  /**
   * @fn getAMorPM
   * @brief output AM or PM of time
   * @return AM or PM, return empty string for 24 hours mode
   */
  String getAMorPM();

  /**
   * @fn enable32k
   * @brief enable the 32k output
   * @return None
   */
  void enable32k();

  /**
   * @fn disable32k
   * @brief disable the 32k output
   * @return None
   */
  void disable32k();

  /**
   * @fn writeSRAM
   * @brief write the SRAM
   * @param addr 0x2c~0x0x71
   * @param data uint8_t HEX
   * @return None
   */
  void writeSRAM(uint8_t addr, uint8_t data);

  /**
   * @fn readSRAM
   * @brief readthe SRAM
   * @param addr 0x2c~0x0x71
   * @return data stored in the SRAM
   */
  uint8_t readSRAM(uint8_t addr);

  /**
   * @fn clearSRAM
   * @brief clear the SRAM
   * @param addr 0x2c~0x0x71
   * @return None
   */
  void clearSRAM(uint8_t addr);

  /**
   * @fn countDown
   * @brief Countdown
   * @param second  countdown time 0~0xffffff
   */
  void countDown(uint32_t second);

private:

  /**
   * @fn bcd2bin(uint8_t val)
   * @brief BCD code to BIN code
   * @param val Input BCD code
   * @return Return BIN code
   */
  uint8_t bcd2bin(uint8_t val);

  /**
   * @fn bin2bcd(uint8_t val)
   * @brief BIN code to BCD code
   * @param val Input BIN code
   * @return Return BCD code
   */
  uint8_t bin2bcd(uint8_t val);

  /**
   * @fn writeReg
   * @brief Write register function, designed as virtual function, implemented by derived class
   * @param reg  Register address 8bits
   * @param pBuf Storage and buffer for data to be written
   * @param size Length of data to be written
   * @return None
   */
  virtual void writeReg(uint8_t reg, void* pBuf, size_t size) = 0;

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
  virtual uint8_t readReg(uint8_t reg, void* pBuf, size_t size) = 0;

private:
  eHours_t _mode = e24hours;

};
#endif
