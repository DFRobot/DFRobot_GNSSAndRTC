# DFRobot_GNSS
- [中文版](./README_CN.md)

This product is a module that integrates GNSS (Global Navigation Satellite System) and RTC (Real-Time Clock) chips. It is capable of receiving signals from various satellite systems such as BeiDou and GPS, allowing it to obtain accurate time information for RTC synchronization. This ensures high precision and stability of timekeeping. The module offers users a simple and convenient way to calibrate and maintain device time, making it suitable for a wide range of applications requiring precise time synchronization.

In many application domains, time synchronization is of paramount importance. This includes scenarios involving a large number of Internet of Things (IoT) devices, automated systems, and scientific research, all of which rely on accurate and consistent time as a foundational element.

![effect picture](./resources/images/DFR1103.png)

## Product Link(https://www.dfrobot.com/)

    SKU: DFR1103

## Table of Contents

* [Summary](#Summary)
* [Installation](#Installation)
* [Methods](#Methods)
* [Compatibility](#Compatibility)
* [History](#History)
* [Credits](#Credits)

## Summary

  * Get real time data. The initial time needs to be set by users. Users can set to get the compiling time. Minimum unit: s 
  * Alarm clock. Users can set alarm time and get falling-edge pulse at interrupt pin to trigger the alarm clock
  * Measure the chip temperature, error: ±0.5℃


## Installation
There are two methods for using this library:<br>
1. Open Arduino IDE, search for "DFRobot_GNSS" on the status bar in Tools ->Manager Libraries and install the library.<br>
2. Download the library file before use, paste it into \Arduino\libraries directory, then open the examples folder and run the demo in the folder.<br>

## Methods

```C++

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

/******************************************************************
 *                  RTC(SD3031) module API
 ******************************************************************/

  /**
   * @fn getRTCTime
   * @brief Get information of year in RTC module
   * @return Return the obtained year
   */
  sTimeData_t getRTCTime(void);

  /**
   * @brief Set clock as 24-hour or 12-hour format
   * @param mode Clock time format
   */
  void setHourSystem(eHours_t mode){ _mode = mode; };

  /**
   * @fn setTime
   * @brief Set time into RTC and take effect immediately
   * @param year 2000~2099
   * @param month 1~12
   * @param day 1~31
   * @param week 0~6
   * @param hour 0~23
   * @param minute 0~59
   * @param second 0~59
   * @return None
   */
  void setTime(uint16_t year, uint8_t month, uint8_t day,eWeek_t week,uint8_t hour, uint8_t minute, uint8_t second);

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
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |    bit7    |    bit6    |    bit5    |    bit4    |    bit3    |    bit2    |    bit1    |    bit0    |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |            |  Saturday  |  Friday    |  Thursday  | Wednesday  |  Tuesday   |  Monday    |  Sunday    |
   * @n ---------------------------------------------------------------------------------------------------------
   * @param hour 0~23
   * @param minute 0~59
   * @param second 0~59
   */
  void setAlarm(uint8_t week, uint8_t hour, uint8_t minute, uint8_t second);

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
   * @brief Output AM or PM of time
   * @return AM or PM, return empty string for 24 hours mode
   */
  String getAMorPM();

  /**
   * @fn enable32k
   * @brief Enable 32k frequency output
   * @return None
   */
  void enable32k();

  /**
   * @fn disable32k
   * @brief Disable 32k frequency output
   * @return None
   */
  void disable32k();
  /**
   * @fn writeSRAM
   * @brief Write SRAM
   * @param addr 0x14-0xFF
   * @param data Write data
   * @return true indicates writing SRAM succeeded, false indicates writing SRAM failed
   */
  uint8_t writeSRAM(uint8_t addr, uint8_t data);

  /**
   * @fn readSRAM
   * @brief Read SRAM
   * @param addr 0x14~0xFF
   * @return Data stored in SRAM
   */
  uint8_t readSRAM(uint8_t addr);

  /**
   * @fn clearSRAM
   * @brief Clear SRAM
   * @param addr 0x14~0xFF
   * @return true indicates clearing SRAM succeeded, false indicates clearing SRAM failed
   */
  uint8_t clearSRAM(uint8_t addr);
  
  /**
   * @fn countDown
   * @brief Countdown
   * @param second  countdown time 0-0xffffff
   */
  void countDown(uint32_t second);

/******************************************************************
 *                  GNSS(L76K) module API
 ******************************************************************/
/**
 * @fn getUTC
 * @brief Get utc standard time
 * @return sTim_t type, represents the returned hour, minute and second
 * @retval sTim_t.hour hour
 * @retval sTim_t.minute minute
 * @retval sTim_t.second second
 */
  sTim_t getUTC(void);

/**
 * @fn getDate
 * @brief Get the date like year, month and day
 * @return sTim_t type, represents the returned year, month and day
 * @retval sTim_t.year year
 * @retval sTim_t.month month
 * @retval sTim_t.day day
 */
  sTim_t getDate(void);

/**
 * @fn getLat
 * @brief Get latitude
 * @return sLonLat_t Type, represents the returned latitude 
 * @retval sLonLat_t.latDD   Latitude degree(0-90)
 * @retval sLonLat_t.latMM   Latitude  The first and second digits behind the decimal point 
 * @retval sLonLat_t.latMMMMM Latitude  The third and seventh digits behind the decimal point 
 * @retval sLonLat_t.latitude  Latitude value with 7 decimal digits 
 * @retval sLonLat_t.latDirection Direction of latitude 
 */
  sLonLat_t getLat(void);

/**
 * @fn getLon
 * @brief Get longitude 
 * @return sLonLat_t Type, represents the returned longitude 
 * @retval sLonLat_t.lonDDD  Longitude degree(0-90)
 * @retval sLonLat_t.lonMM   Longitude  The first and second digits behind the decimal point 
 * @retval sLonLat_t.lonMMMMM Longitude The third and seventh digits behind the decimal point 
 * @retval sLonLat_t.lonitude Longitude value with 7 decimal digits 
 * @retval sLonLat_t.lonDirection Direction of longitude 
 */
  sLonLat_t getLon(void);

/**
 * @fn getNumSatUsed
 * @brief Get the number of the used satellite used 
 * @return uint8_t type, represents the number of the used satellite
 */
  uint8_t getNumSatUsed(void);

/**
 * @fn getAlt
 * @brief Get altitude 
 * @return double type, represents altitude 
 */
  double getAlt(void);

/**
 * @fn getSog
 * @brief Get speed over ground 
 * @return speed Float data(unit: knot)
 */
  double getSog(void);

/**
 * @fn getCog
 * @brief Get course over ground 
 * @return Float data(unit: degree) 
 */
  double getCog(void);

/**
 * @fn setGnss
 * @brief Set GNSS to be used
 * @param mode
 * @n   eGPS              use gps
 * @n   eBeiDou           use beidou
 * @n   eGPS_BeiDou       use gps + beidou
 * @n   eGLONASS          use glonass
 * @n   eGPS_GLONASS      use gps + glonass
 * @n   eBeiDou_GLONASS   use beidou +glonass
 * @n   eGPS_BeiDou_GLONASS use gps + beidou + glonass
 * @return NULL
 */
  void setGnss(eGnssMode_t mode);

/**
 * @fn getGnssMode
 * @brief Get the used gnss mode
 * @return mode
 * @retval 1  gps
 * @retval 2  beidou
 * @retval 3  gps + beidou
 * @retval 4  glonass
 * @retval 5  gps + glonass
 * @retval 6  beidou +glonass
 * @retval 7  gps + beidou + glonass
 */
  uint8_t getGnssMode(void);

/**
 * @fn getAllGnss
 * @brief Get GNSS data, call back and receive 
 * @return null
 */
  void getAllGnss(void);

/**
 * @fn enablePower
 * @brief Enable gnss power
 * @return null
 */
void enablePower(void);

/**
 * @fn disablePower
 * @brief Disable gnss power
 * @return null
 */
void disablePower(void);

/**
 * @fn setCallback
 * @brief Set callback function type 
 * @param  * call function name 
 * @return null
 */
  void setCallback(void (*call)(char *, uint8_t));

```

## Compatibility

MCU                | Work Well    |   Work Wrong    | Untested    | Remarks
------------------ | :----------: | :-------------: | :---------: | :----:
Arduino Uno        |      √       |                 |             |
Arduino MEGA2560   |      √       |                 |             |
Arduino Leonardo   |      √       |                 |             |
FireBeetle-ESP8266 |      √       |                 |             |
FireBeetle-ESP32   |      √       |                 |             |
FireBeetle-M0      |      √       |                 |             |
Micro:bit          |      √       | nonsupport uart |             |


## History

- 2023/08/31 - Version 1.0.0 released.

## Credits

Written by qsjhyy(yihuan.huang@dfrobot.com), 2023. (Welcome to our website)
