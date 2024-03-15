/*!
 * @file DFRobot_GNSS.h
 * @brief Define the basic structure of the DFRobot_GNSS class, the implementation of the basic methods
 * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [ZhixinLiu](zhixin.liu@dfrobot.com)
 * @version V1.0
 * @date 2022-10-26
 * @url https://github.com/DFRobot/DFRobot_GNSS
 */
#ifndef __DFRobot_GNSS_H__
#define __DFRobot_GNSS_H__

#include <Arduino.h>
#include <Wire.h>

class DFRobot_GNSS
{
public:
#define REG_YEAR_H       0
#define REG_YEAR_L       1
#define REG_MONTH        2
#define REG_DATE         3
#define REG_HOUR         4
#define REG_MINUTE       5
#define REG_SECOND       6

#define REG_LAT_1        7
#define REG_LAT_2        8
#define REG_LAT_X_24     9
#define REG_LAT_X_16     10
#define REG_LAT_X_8      11
#define REG_LAT_DIS      12

#define REG_LON_1        13
#define REG_LON_2        14
#define REG_LON_X_24     15
#define REG_LON_X_16     16
#define REG_LON_X_8      17
#define REG_LON_DIS      18

#define REG_USE_STAR     19

#define REG_ALT_H        20
#define REG_ALT_L        21
#define REG_ALT_X        22

#define REG_SOG_H        23
#define REG_SOG_L        24
#define REG_SOG_X        25

#define REG_COG_H        26
#define REG_COG_L        27
#define REG_COG_X        28

#define REG_START_GET    29
#define REG_I2C_ADDR     30
#define REG_DATA_LEN_H   31
#define REG_DATA_LEN_L   32
#define REG_ALL_DATA     33

#define REG_GNSS_MODE    34
#define REG_SLEEP_MODE   35
  // #define REG_RGB_MODE     36   // 0x24

#define GNSS_I2C_FLAG    1
#define GNSS_UART_FLAG   2

#define TIME_OUT         200   ///< 200 ms time out

  // #define RGB_ON 0x05
  // #define RGB_OFF 0x02

#define ENABLE_POWER     0
#define DISABLE_POWER    1

#define UART_MAX_READ_LEN   250
#define I2C_MAX_READ_LEN    32

public:
  /**
   * @struct sTim_t
   * @brief Store the time and date information obtained from GPS
   */
  typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
  }sTim_t;

  /**
   * @struct sLonLat_t
   * @brief Store latitude, longitude and direction information obtained from GPS
   */
  typedef struct {
    uint8_t lonDDD;
    uint8_t lonMM;
    uint32_t lonMMMMM;
    char lonDirection;
    uint8_t latDD;
    uint8_t latMM;
    uint32_t latMMMMM;
    char latDirection;
    double latitude;
    double latitudeDegree;
    double lonitude;
    double lonitudeDegree;
  }sLonLat_t;


  /**
   * @brief Set GNSS
   */
  typedef enum {
    eGPS = 1,
    eBeiDou,
    eGPS_BeiDou,
    eGLONASS,
    eGPS_GLONASS,
    eBeiDou_GLONASS,
    eGPS_BeiDou_GLONASS,
  }eGnssMode_t;

public:
  uint8_t i2cUartFlag = 0;

public:
  /**
   * @fn DFRobot_GNSS
   * @brief Constructor
   * @return None
   */
  DFRobot_GNSS();

  /**
   * @fn getUTC
   * @brief Get UTC, standard time
   * @return sTim_t type, represents the returned hour, minute and second
   * @retval sTim_t.hour hour
   * @retval sTim_t.minute minute
   * @retval sTim_t.second second
   */
  sTim_t getUTC(void);

  /**
   * @fn getDate
   * @brief Get date information, year, month, day
   * @return sTim_t type, represents the returned year, month, day
   * @retval sTim_t.year year
   * @retval sTim_t.month month
   * @retval sTim_t.day day
   */
  sTim_t getDate(void);

  /**
   * @fn getLat
   * @brief Get latitude
   * @return sLonLat_t type, represents the returned latitude
   * @retval sLonLat_t.latDD   Latitude degree(0-90)
   * @retval sLonLat_t.latMM   The first and second digits behind the decimal point
   * @retval sLonLat_t.latMMMMM Latitude  The third and seventh digits behind the decimal point
   * @retval sLonLat_t.latitude Latitude value with 7 decimal digits
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
   * @retval 1 gps
   * @retval 2 beidou
   * @retval 3 gps + beidou
   * @retval 4 glonass
   * @retval 5 gps + glonass
   * @retval 6 beidou +glonass
   * @retval 7 gps + beidou + glonass
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

  // /**
  //  * @fn setRgbOn
  //  * @brief Turn rgb on
  //  * @return null
  //  */
  // void setRgbOn(void);

  // /**
  //  * @fn setRgbOn
  //  * @brief Turn rgb off
  //  * @return null
  //  */
  // void setRgbOff(void);

  /**
   * @fn setCallback
   * @brief Set callback function type
   * @param  call function name
   * @return null
   */
  void setCallback(void (*call)(char*, uint8_t));

  void (*callback)(char* data, uint8_t len);

private:
  /**
   * @fn getGnssLen
   * @brief Get length of gnss data
   * @return Length
   */
  uint16_t getGnssLen(void);

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

};

#endif
