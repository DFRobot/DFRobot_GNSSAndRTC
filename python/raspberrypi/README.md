# DFRobot_GNSS
- [中文版](./README_CN.md)

This product is a module that integrates GNSS (Global Navigation Satellite System) and RTC (Real-Time Clock) chips. It is capable of receiving signals from various satellite systems such as BeiDou and GPS, allowing it to obtain accurate time information for RTC synchronization. This ensures high precision and stability of timekeeping. The module offers users a simple and convenient way to calibrate and maintain device time, making it suitable for a wide range of applications requiring precise time synchronization.

In many application domains, time synchronization is of paramount importance. This includes scenarios involving a large number of Internet of Things (IoT) devices, automated systems, and scientific research, all of which rely on accurate and consistent time as a foundational element.

![effect picture](../../resources/images/DFR1103.png)

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
To use this library, first download it to Raspberry Pi and then open the use case folder. To execute a use case demox.py, enter python demox.py in the command line. For example, to execute the control32k.py use case, you need to enter:<br>

```
python control32k.py
```



## Methods

```python
'''!
  @brief subclass initialization function
  @return bool type, means returning initialization status
  @retval true NO_ERROR
'''
  def begin(self):

'''!
  @brief Calibrate RTC immediately with GNSS
  @note This is a single calibration
  @n If the GNSS module signal is weak, time calibration may encounter issues
'''
  def calib_rtc(self):

'''!
  @brief The loop automatically performs GNSS timing based on the set interval
  @param hour Automatic calibration of the time interval. range: 0~255, unit: hour
  @note When set to zero, automatic time calibration is disabled
  @n Enabling it will trigger an immediate calibration
  @n If the GNSS module signal is weak, time calibration may encounter issues
'''
  def calib_rtc_hour(self, hour):

'''!
  @brief Current clock calibration status
  @param mode By default, it is set to true, indicating access to the calibration status only
  @n  If continuous calibration for one minute does not return a successful calibration
  @n  you can pass in false to manually terminate this calibration session
  @return uint8_t type, indicates current clock calibration status
  @retval 0 Not calibrated
  @retval 1 Calibration complete
  @retval 2 Under calibration
  @note Note: To avoid affecting subsequent calibration status,
  @n    "Calibration completed Status (1)" is automatically zeroed after a successful read
'''
  def calib_status(self, mode=True):

'''!
/******************************************************************
 *                  RTC(SD3031) module API
 ******************************************************************/
'''

'''!
  @brief Get information of year in RTC module
  @return Return the obtained year
'''
  def get_rtc_time(self):

'''!
  @brief Set clock as 24-hour or 12-hour format
  @param mode Clock time format
  @n	E24HOURS
  @n	E12HUORS
'''
  def set_hour_system(self, mode):

'''!
  @brief Set time into rtc and take effect immediately
  @param year 2000~2099
  @param month 1~12
  @param day 1~31
  @param hour 0~23
  @param minute 0~59
  @param second 0~59
'''
  def set_time(self, year, month, day, hour, minute, second):

'''!
  @brief Set the data for triggering alarm
  @param year 2000~2099
  @param month 1~12
  @param day 1~31
'''
  def set_alarm(self, year, month, day):

'''!
  @brief Set the Alarmnumber object
  @param week
  @n ---------------------------------------------------------------------------------------------------------
  @n | EWORKDAY | EEVERYDAY | ESATURDAY | EFRIDAY | ETHURSDAY | EWEDNESDAY | ETUESDAY | EMONDAY | ESUNDAY |
  @n ---------------------------------------------------------------------------------------------------------
  @param hour 0~23
  @param minute 0~59
  @param second 0~59
'''
  def set_alarm(self, week, hour, minute, second):

'''!
  @brief Get internal temperature of the clock
  @return Return the obtained temperature, unit8: ℃
'''
  def get_temperature_c(self):

'''!
  @brief Get voltage of onboard battery
  @return float Return the obtained voltage
'''
  def get_voltage(self):

'''!
  @brief Clear alarm flag bit
'''
  def clear_alarm(self):
    
'''!
  @brief output AM or PM of time
  @return AM or PM, return empty string for 24 hours mode
'''
  def get_am_or_pm(self):

'''!
  @brief enable the 32k output
'''
  def enable_32k(self):

'''!
  @brief disable the 32k output
'''
  def disable_32k(self):

'''!
  @brief write the SRAM
  @param addr 0x2c~0x71
  @param data 0x00~0xff uint8_t HEX
'''
  def write_sram(self, addr, data):

'''!
  @brief read the SRAM
  @param addr 0x2c~0x71
  @return data stored in the SRAM
'''
  def read_sram(self, addr):

'''!
  @brief clear the SRAM
  @param addr 0x2c~0x71
'''
  def clear_sram(self, addr):
  
'''!
  @brief Countdown
  @param second  countdown time 0~0xffffff
'''
  def count_down(self, second):
'''!
/******************************************************************
 *                  GNSS(L76K) module API
 ******************************************************************/
'''

'''!
  @brief Get UTC, standard time
  @return STim_t type, represents the returned hour, minute and second
  @retval STim_t.hour hour
  @retval STim_t.minute minute
  @retval STim_t.second second
'''
  def get_utc(self):

'''!
  @brief Get date information, year, month, day
  @return STim_t type, represents the returned year, month, day
  @retval STim_t.year year
  @retval STim_t.month month
  @retval STim_t.date date
'''
  def get_date(self):

'''!
  @brief Get latitude
  @return SLonLat_t type, represents the returned latitude
  @retval SLonLat_t.latDD   Latitude degree(0-90)
  @retval SLonLat_t.latMM   The first and second digits behind the decimal point
  @retval SLonLat_t.latMMMMM Latitude  The third and seventh digits behind the decimal point
  @retval SLonLat_t.latitude Latitude value with 7 decimal digits
  @retval SLonLat_t.latDirection Direction of latitude
'''
  def get_lat(self):

'''!
  @brief Get longitude
  @return SLonLat_t Type, represents the returned longitude
  @retval SLonLat_t.lonDDD  Longitude degree(0-90)
  @retval SLonLat_t.lonMM   Longitude  The first and second digits behind the decimal point
  @retval SLonLat_t.lonMMMMM Longitude The third and seventh digits behind the decimal point
  @retval SLonLat_t.lonitude Longitude value with 7 decimal digits
  @retval SLonLat_t.lonDirection Direction of longi
'''
  def get_lon(self):

'''!
  @brief Get the number of the used satellite used
  @return uint8_t type, represents the number of the used satellite
'''
  def get_num_sat_used(self):

'''!
  @brief Get altitude
  @return double type, represents altitude
'''
  def get_alt(self):

'''!
  @brief Get speed over ground
  @return speed Float data(unit: knot)
'''
  def get_sog(self):

'''!
  @brief Get course over ground
  @return Float data(unit: degree)
'''
  def get_cog(self):

'''!
  @brief Set GNSS to be used
  @param mode
  @n   EGPS              use gps
  @n   EBEIDOU           use beidou
  @n   EGPS_BEIDOU       use gps + beidou
  @n   EGLONASS          use glonass
  @n   EGPS_GLONASS      use gps + glonass
  @n   EBEIDOU_GLONASS   use beidou +glonass
  @n   EGPS_BEIDOU_GLONASS use gps + beidou + glonass
'''
  def set_gnss(self, mode):

'''!
  @brief Get the used gnss mode
  @return mode
  @retval 1 gps
  @retval 2 beidou
  @retval 3 gps + beidou
  @retval 4 glonass
  @retval 5 gps + glonass
  @retval 6 beidou +glonass
  @retval 7 gps + beidou + glonass
'''
  def get_gnss_mode(self):

'''!
  @brief Get GNSS data, call back and receive
'''
  def get_all_gnss(self):

'''!
  @brief Enable gnss power
'''
  def enable_power(self):

'''!
  @brief Disable gnss power
'''
  def disable_power(self):

'''!
  @brief Set callback function type
  @param  callback function name
'''
  def set_callback(self, callback):

```

## Compatibility

|              |           |            |          |         |
| ------------ | --------- | ---------- | -------- | ------- |
| MCU          | Work Well | Work Wrong | Untested | Remarks |
| RaspberryPi2 |           |            | √        |         |
| RaspberryPi3 |           |            | √        |         |
| RaspberryPi4 | √         |            |          |         |

- Python version

|         |           |            |          |         |
| ------- | --------- | ---------- | -------- | ------- |
| Python  | Work Well | Work Wrong | Untested | Remarks |
| Python2 | √         |            |          |         |
| Python3 | √         |            |          |         |


## History

- 2024/07/15 - Version 0.0.1 released.

## Credits

Written by thdyyl(yuanlong.yu@dfrobot.com), 2024. (Welcome to our website)
