/*!
 * @file DFRobot_SD3031.cpp
 * @brief Implemention of DFRobot_SD3031 class
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-07-25
 * @url https://github.com/DFRobot/DFRobot_SD3031
 */
#include "DFRobot_SD3031.h"

DFRobot_SD3031::DFRobot_SD3031()
{

}

const uint8_t daysInMonth[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
  if (y >= 2000)
    y -= 2000;                              // Remove year offset
  uint16_t days = d;                          // Store numbers of days

  for (uint8_t i = 1; i < m; ++i) {
    days += pgm_read_byte(daysInMonth + i - 1); // Add number of days for each month
  } if (m > 2 && y % 4 == 0)
    ++days;                                 // Deal with leap years
  return days + 365 * y + (y + 3) / 4 - 1;    // Return computed value
}

void DFRobot_SD3031::setHourSystem(eHours_t mode)
{
  uint8_t bcdTime, binTime;
  uint8_t buffer[7] = { 0 };
  readReg(SD3031_REG_SEC, buffer, 7);
  // uint32_t t1 = micro();
  bcdTime = buffer[2];
  // readReg(SD3031_REG_HOUR, &bcdTime, 1);
  if (_mode != (bcdTime & 0x80)) {   // If _mode is inconsistent with the mode saved in the clock chip
    if (_mode != mode) {   // And _mode is inconsistent with the mode to be set
      _mode = mode;
      return;   // The saved mode is inconsistent with the current mode of the chip, but the current mode of the chip is consistent with the mode to be set
    }
  } else {
    if (_mode == mode) {
      return;   // All three modes are consistent
    }
    _mode = mode;
  }

  if (eHours_t::e24hours == (bcdTime & 0x80)) {   // 24 to 12
    binTime = bcd2bin(bcdTime & 0x7f);
    if (binTime == 0) {
      bcdTime = 0x12;
    } else if (binTime > 0 && binTime < 12) {
      bcdTime = (0x00 | bin2bcd(binTime));
    } else if (binTime == 12) {
      bcdTime = 0x32;
    } else if (binTime > 12 && binTime < 24) {
      bcdTime = (0x20 | bin2bcd(binTime - 12));
    }
  } else {   // 12 to 24
    binTime = bcd2bin(bcdTime & 0x1f);
    if (bcdTime & 0x20) {   // PM
      if (12 != binTime) {
        binTime += 12;
      }
    } else {   // AM
      if (12 == binTime) {
        binTime = 0;
      }
    }
    bcdTime = bin2bcd(binTime) | 0x80;
  }
  // uint32_t t2 = micro() - t1;
  // Serial.println(t2);
  // readReg(SD3031_REG_SEC, buffer, 7);   // Avoid taking too long up front
  // delay(0.01);
  buffer[2] = bcdTime;
  writeReg(SD3031_REG_SEC, buffer, 7);
  // writeReg(SD3031_REG_HOUR, &bcdTime, 1);   // Cannot write to a single time register
}

void DFRobot_SD3031::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  uint8_t _hour = 0, _year = 0, buffer[7], week = 0;
  _year = year - 2000;
  if (_mode == eHours_t::e24hours) {
    _hour = bin2bcd(hour) | 0x80;
  } else {   // It can only be set in 24 hours
    if (hour == 0) {
      _hour = 0x12;
    } else if (hour > 0 && hour < 12) {
      _hour = (0x00 | bin2bcd(hour));
    } else if (hour == 12) {
      _hour = 0x32;
    } else if (hour > 12 && hour < 24) {
      _hour = (0x20 | bin2bcd(hour - 12));
    }
  }
  week = (date2days(year, month, day) + 6) % 7;
  // DBG(week);
  buffer[0] = bin2bcd(second);
  buffer[1] = bin2bcd(minute);
  buffer[2] = _hour;
  buffer[3] = bin2bcd(week);
  buffer[4] = bin2bcd(day);
  buffer[5] = bin2bcd(month);
  buffer[6] = bin2bcd(_year);
  writeReg(SD3031_REG_SEC, buffer, 7);
}

DFRobot_SD3031::sTimeData_t DFRobot_SD3031::getRTCTime(void)
{
  sTimeData_t sTime;
  uint8_t buffer[7];
  uint8_t data;
  readReg(SD3031_REG_SEC, buffer, 7);
  sTime.year = 2000 + bcd2bin(buffer[6]);
  sTime.month = bcd2bin(buffer[5]);
  sTime.day = bcd2bin(buffer[4]);
  data = bcd2bin(buffer[3]);
  // DBG(data);
  switch (data) {
  case 0:
    sTime.week = "Sunday";
    break;
  case 1:
    sTime.week = "Monday";
    break;
  case 2:
    sTime.week = "Tuesday";
    break;
  case 3:
    sTime.week = "Wednesday";
    break;
  case 4:
    sTime.week = "Thursday";
    break;
  case 5:
    sTime.week = "Friday";
    break;
  case 6:
    sTime.week = "Saturday";
    break;
  default:
    break;
  }
  data = buffer[2];
  if (_mode == eHours_t::e24hours) {
    sTime.hour = bcd2bin(data & 0x7f);

  } else {
    // if (data & 0x20) {
    //   data = data << 3;
    //   data = bcd2bin(data >> 3);
    // } else {
    //   data = data << 2;
    //   data = bcd2bin(data >> 2);
    // }
    // sTime.hour = data;
    sTime.hour = bcd2bin(data & 0x1f);
  }
  sTime.minute = bcd2bin(buffer[1]);
  sTime.second = bcd2bin(buffer[0]);
  return sTime;

}

void DFRobot_SD3031::setAlarm(uint16_t year, uint8_t month, uint8_t day)
{
  uint8_t buffer[8];
  uint8_t _year = 0;
  uint8_t data = 0;
  data = 0x80;
  writeReg(SD3031_REG_CTR3, &data, 1);
  data = 0x92;
  writeReg(SD3031_REG_CTR2, &data, 1);
  _year = year - 2000;
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = bin2bcd(day);
  buffer[5] = bin2bcd(month);
  buffer[6] = bin2bcd(_year);
  buffer[7] = 0x70;
  writeReg(SD3031_REG_ALARM_SEC, buffer, 8);
}

void DFRobot_SD3031::setAlarm(eWeek_t week, uint8_t hour, uint8_t minute, uint8_t second)
{
  uint8_t buffer[8];
  uint8_t _hour = 0;
  uint8_t data = 0;
  data = 0x80;
  writeReg(SD3031_REG_CTR3, &data, 1);
  data = 0x92;
  writeReg(SD3031_REG_CTR2, &data, 1);
  if (_mode == eHours_t::e24hours) {
    _hour = bin2bcd(hour) | 0x80;
  } else {
    if (hour == 0) {
      _hour = 0x12;
    } else if (hour > 0 && hour < 12) {
      _hour = (0x00 | bin2bcd(hour));
    } else if (hour == 12) {
      _hour = 0x32;
    } else if (hour > 12 && hour < 24) {
      _hour = (0x20 | bin2bcd(hour - 12));
    }
  }
  buffer[0] = bin2bcd(second);
  buffer[1] = bin2bcd(minute);
  buffer[2] = _hour;
  buffer[3] = week;
  buffer[4] = 0;
  buffer[5] = 0;
  buffer[6] = 0;
  buffer[7] = 0x0f;
  writeReg(SD3031_REG_ALARM_SEC, buffer, 8);
}


int8_t DFRobot_SD3031::getTemperatureC(void)
{
  int8_t data = 0;
  readReg(SD3031_REG_TEMP, &data, 1);
  return data;
}

float DFRobot_SD3031::getVoltage(void)
{
  uint8_t buffer[2];
  uint16_t data = 0;
  float ret = 0.0;
  readReg(SD3031_REG_BAT_VAL, buffer, 2);
  data = (((buffer[0] & 0x80) >> 7) << 8) | buffer[1];
  ret = data / 100.0;
  return ret;
}

void DFRobot_SD3031::clearAlarm(void)
{
  uint8_t buffer[2];
  readReg(SD3031_REG_CTR1, buffer, 1);
}

uint8_t DFRobot_SD3031::bcd2bin(uint8_t val)
{
  return val - 6 * (val >> 4);
}

uint8_t DFRobot_SD3031::bin2bcd(uint8_t val)
{
  return val + 6 * (val / 10);
}

String DFRobot_SD3031::getAMorPM()
{
  uint8_t buffer[7];
  uint8_t data;
  String ret;
  readReg(SD3031_REG_SEC, buffer, 7);
  data = buffer[2];
  if (data & 0x20) {
    ret = "PM";
  } else {
    ret = "AM";
  }
  return ret;
}

void DFRobot_SD3031::enable32k()
{
  uint8_t flag1 = 0;
  uint8_t data;
  readReg(SD3031_REG_CTR3, &data, 1);
  // flag1 = data & 0x00;
  flag1 = data & 0xBF;
  writeReg(SD3031_REG_CTR3, &flag1, 1);
  delay(100);
}

void DFRobot_SD3031::disable32k()
{
  uint8_t flag1 = 0;
  uint8_t data;
  readReg(SD3031_REG_CTR3, &data, 1);
  // flag1 = data | 0x40;
  flag1 = data | 0x40;
  writeReg(SD3031_REG_CTR3, &flag1, 1);
  delay(100);
}

void DFRobot_SD3031::writeSRAM(uint8_t reg, uint8_t data)
{
  writeReg(reg, &data, 1);
}

uint8_t DFRobot_SD3031::readSRAM(uint8_t reg)
{
  uint8_t buf[2];
  readReg(reg, buf, 1);
  return buf[0];
}

void DFRobot_SD3031::clearSRAM(uint8_t reg)
{
  uint8_t buf = 0xff;
  writeReg(reg, &buf, 1);
}

void DFRobot_SD3031::countDown(uint32_t second)
{
  uint8_t data = 0;
  uint8_t buffer[3];
  uint32_t _second = 0;
  if (second > 0xffffff) {
    _second = 0xffffff;
  } else {
    _second = second;
  }
  clearAlarm();   //

  data = 0x80;
  writeReg(SD3031_REG_CTR2, &data, 1);
  data = 0xB4;
  writeReg(SD3031_REG_CTR2, &data, 1);
  data = 0x20;
  writeReg(SD3031_REG_CTR3, &data, 1);
  buffer[0] = _second & 0xff;
  buffer[1] = (_second >> 8) & 0xff;
  buffer[2] = (_second >> 16) & 0xff;
  writeReg(SD3031_REG_COUNTDOWM, buffer, 3);

}
