/*!
 * @file DFRobot_GNSS.cpp
 * @brief Define the basic structure of the DFRobot_GNSS class, the implementation of the basic methods
 * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [ZhixinLiu](zhixin.liu@dfrobot.com)
 * @version V1.0
 * @date 2022-10-26
 * @url https://github.com/DFRobot/DFRobot_GNSS
 */
#include "DFRobot_GNSS.h"

DFRobot_GNSS::DFRobot_GNSS()
{

}

DFRobot_GNSS::sTim_t DFRobot_GNSS::getDate(void)
{
  sTim_t data;
  uint8_t _sendData[10] = { 0 };
  readReg(REG_YEAR_H, _sendData, 4);
  data.year = ((uint16_t)_sendData[0] << 8) | _sendData[1];
  data.month = _sendData[2];
  data.date = _sendData[3];
  return data;
}

DFRobot_GNSS::sTim_t DFRobot_GNSS::getUTC(void)
{
  sTim_t data;
  uint8_t _sendData[10] = { 0 };
  readReg(REG_HOUR, _sendData, 3);
  data.hour = _sendData[0];
  data.minute = _sendData[1];
  data.second = _sendData[2];
  return data;
}

DFRobot_GNSS::sLonLat_t DFRobot_GNSS::getLat(void)
{
  sLonLat_t data;
  uint8_t _sendData[10] = { 0 };
  readReg(REG_LAT_1, _sendData, 6);
  data.latDD = _sendData[0];
  data.latMM = _sendData[1];
  data.latMMMMM = ((uint32_t)_sendData[2] << 16) | ((uint32_t)_sendData[3] << 8) | ((uint32_t)_sendData[4]);
  data.latitude = (double)data.latDD * 100.0 + ((double)data.latMM) + ((double)data.latMMMMM / 100000.0);
  data.latitudeDegree = (double)data.latDD + (double)data.latMM / 60.0 + (double)data.latMMMMM / 100000.0 / 60.0;
  data.latDirection = _sendData[5];
  return data;
}

DFRobot_GNSS::sLonLat_t DFRobot_GNSS::getLon(void)
{
  sLonLat_t data;
  uint8_t _sendData[10] = { 0 };
  readReg(REG_LON_1, _sendData, 6);
  data.lonDDD = _sendData[0];
  data.lonMM = _sendData[1];
  data.lonMMMMM = ((uint32_t)_sendData[2] << 16) | ((uint32_t)_sendData[3] << 8) | ((uint32_t)_sendData[4]);
  data.lonitude = (double)data.lonDDD * 100.0 + ((double)data.lonMM) + ((double)data.lonMMMMM / 100000.0);

  data.lonitudeDegree = (double)data.lonDDD + (double)data.lonMM / 60.0 + (double)data.lonMMMMM / 100000.0 / 60.0;
  data.lonDirection = _sendData[5];
  return data;
}

uint8_t DFRobot_GNSS::getNumSatUsed(void)
{
  uint8_t _sendData[10] = { 0 };
  readReg(REG_USE_STAR, _sendData, 1);
  return _sendData[0];
}

double DFRobot_GNSS::getAlt(void)
{
  double high;
  uint8_t _sendData[10] = { 0 };
  readReg(REG_ALT_H, _sendData, 3);
  if (_sendData[0] & 0x80) {
    high = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  } else {
    high = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  }
  return high;
}

double DFRobot_GNSS::getSog(void)
{
  double speed;
  uint8_t _sendData[10] = { 0 };

  readReg(REG_SOG_H, _sendData, 3);
  if (_sendData[0] & 0x80) {
    speed = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  } else {
    speed = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  }
  return speed;
}

double DFRobot_GNSS::getCog(void)
{
  double cog;
  uint8_t _sendData[10] = { 0 };

  readReg(REG_COG_H, _sendData, 3);
  if (_sendData[0] & 0x80) {
    cog = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  } else {
    cog = (double)((uint16_t)(_sendData[0] & 0x7F) << 8 | _sendData[1]) + (double)_sendData[2] / 100.0;
  }
  return cog;
}


// void DFRobot_GNSS::setRgbOn(void)
// {
//   uint8_t _sendData[10] = {0};
//   _sendData[0] = RGB_ON;
//   writeReg(REG_RGB_MODE, _sendData, 1);
//   delay(50);
// }

// void DFRobot_GNSS::setRgbOff(void)
// {
//   uint8_t _sendData[10] = {0};
//   _sendData[0] = RGB_OFF;
//   writeReg(REG_RGB_MODE, _sendData, 1);
//   delay(50);
// }


void DFRobot_GNSS::enablePower(void)
{
  uint8_t _sendData[10] = { 0 };
  _sendData[0] = ENABLE_POWER;
  writeReg(REG_SLEEP_MODE, _sendData, 1);
  delay(50);
}

void DFRobot_GNSS::disablePower(void)
{
  uint8_t _sendData[10] = { 0 };
  _sendData[0] = DISABLE_POWER;
  writeReg(REG_SLEEP_MODE, _sendData, 1);
  delay(50);
}

void DFRobot_GNSS::setGnss(eGnssMode_t mode)
{
  uint8_t _sendData[10] = { 0 };
  _sendData[0] = mode;
  writeReg(REG_GNSS_MODE, _sendData, 1);
  delay(50);
}

uint8_t DFRobot_GNSS::getGnssMode(void)
{
  uint8_t _sendData[10] = { 0 };
  readReg(REG_GNSS_MODE, _sendData, 1);
  return _sendData[0];
}

uint16_t DFRobot_GNSS::getGnssLen(void)
{
  uint8_t _sendData[10] = { 0 };
  _sendData[0] = 0x55;                  // enable messure
  writeReg(REG_START_GET, _sendData, 1);
  delay(100);
  readReg(REG_DATA_LEN_H, _sendData, 2);
  return (uint16_t)_sendData[0] << 8 | _sendData[1];
}

void DFRobot_GNSS::getAllGnss(void)
{
  uint8_t _sendData[260] = { 0 };
  uint16_t len = getGnssLen();
  if (len > 1024 + 200 || len == 0) {
    return;
  }
  if (i2cUartFlag == GNSS_UART_FLAG) {
    uint8_t templen = len / UART_MAX_READ_LEN + 1;
    for (uint16_t i = 0; i < templen; i++) {
      if (i == ((uint8_t)(templen - 1))) {
        readReg(REG_ALL_DATA, _sendData, len % UART_MAX_READ_LEN);
        for (uint8_t i = 0; i < len % UART_MAX_READ_LEN; i++) {
          if (_sendData[i] == '\0') {
            _sendData[i] = '\n';
          }
        }
        if (callback) {
          callback((char*)_sendData, (uint8_t)(len % UART_MAX_READ_LEN));
        }
      } else {
        readReg(REG_ALL_DATA, _sendData, UART_MAX_READ_LEN);
        for (uint8_t i = 0; i < UART_MAX_READ_LEN; i++) {
          if (_sendData[i] == '\0') {
            _sendData[i] = '\n';
          }
        }

        if (callback) {
          callback((char*)_sendData, (uint8_t)UART_MAX_READ_LEN);
        }
      }
    }
  } else {
    uint8_t templen = len / I2C_MAX_READ_LEN + 1;
    for (uint16_t i = 0; i < templen; i++) {
      if (i == ((uint8_t)(templen - 1))) {
        readReg(REG_ALL_DATA, _sendData, len % I2C_MAX_READ_LEN);
        for (uint8_t i = 0; i < len % I2C_MAX_READ_LEN; i++) {
          if (_sendData[i] == '\0') {
            _sendData[i] = '\n';
          }
        }
        if (callback) {
          callback((char*)_sendData, (uint8_t)(len % I2C_MAX_READ_LEN));
        }
      } else {
        readReg(REG_ALL_DATA, _sendData, I2C_MAX_READ_LEN);
        for (uint8_t i = 0; i < I2C_MAX_READ_LEN; i++) {
          if (_sendData[i] == '\0') {
            _sendData[i] = '\n';
          }
        }
        if (callback) {
          callback((char*)_sendData, (uint8_t)I2C_MAX_READ_LEN);
        }
      }
    }
  }
}

void DFRobot_GNSS::setCallback(void (*call)(char*, uint8_t))
{
  this->callback = call;
}
