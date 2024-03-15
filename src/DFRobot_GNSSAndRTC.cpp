/*!
 * @file DFRobot_GNSSAndRTC.cpp
 * @brief Implemention of DFRobot_GNSSAndRTC class
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [qsjhyy](yihuan.huang@dfrobot.com)
 * @version V1.0
 * @date 2022-08-30
 * @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
 */
#include "DFRobot_GNSSAndRTC.h"

DFRobot_GNSSAndRTC::DFRobot_GNSSAndRTC()
{

}

bool DFRobot_GNSSAndRTC::begin()
{
  uint8_t data = 0x80;
  writeReg(SD3031_REG_I2C_CON, &data, 1);
  return true;
}

void DFRobot_GNSSAndRTC::calibRTC(void)
{
  uint8_t mode = eUnderCalib;
  writeReg(REG_CALIB_STATUS_REG, &mode, 1);
}

void DFRobot_GNSSAndRTC::calibRTC(uint8_t hour)
{
  writeReg(REG_CALIB_RTC_REG, &hour, 1);
}

uint8_t DFRobot_GNSSAndRTC::calibStatus(bool mode)
{
  uint8_t status = eCalibNone;
  if (mode) {
    readReg(REG_CALIB_STATUS_REG, &status, 1);
  } else {   // The user can manually terminate the calibration
    writeReg(REG_CALIB_STATUS_REG, &status, 1);
  }
  return status;
}

DFRobot_GNSSAndRTC_I2C::DFRobot_GNSSAndRTC_I2C(TwoWire* pWire, uint8_t addr)
{
  i2cUartFlag = GNSS_I2C_FLAG;
  _pWire = pWire;
  _deviceAddr = addr;
}

bool DFRobot_GNSSAndRTC_I2C::begin()
{
  _pWire->begin();
  _pWire->beginTransmission(_deviceAddr);
  if (_pWire->endTransmission() != 0) {
    DBG("I2C init error!!!!");
    return false;
  }
  return DFRobot_GNSSAndRTC::begin();
}

void DFRobot_GNSSAndRTC_I2C::writeReg(uint8_t reg, void* pBuf, size_t size)
{
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;
  _pWire->beginTransmission(_deviceAddr);
  _pWire->write(reg);
  for (size_t i = 0; i < size; i++) {
    _pWire->write(_pBuf[i]);
  }
  _pWire->endTransmission();
  delay(50);
}

uint8_t DFRobot_GNSSAndRTC_I2C::readReg(uint8_t reg, void* pBuf, size_t size)
{
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;

  if ((reg >= 0x30) && (reg <= 0x79) && (size != 0)) {   // Update registers corresponding to RTC module data
    uint8_t data[] = { reg, (uint8_t)size };
    writeReg(REG_RTC_READ_REG, data, 2);
    delay(50);
  }

  _pWire->beginTransmission(_deviceAddr);
  _pWire->write(reg);
  if (_pWire->endTransmission() != 0) {
    DBG("endTransmission ERROR!!");
    return 1;
  }
  _pWire->requestFrom((uint8_t)_deviceAddr, (uint8_t)size);
  size_t i = 0;
  while (_pWire->available()) {
    _pBuf[i++] = _pWire->read();
  }
  return 0;
}

#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
DFRobot_GNSSAndRTC_UART::DFRobot_GNSSAndRTC_UART(SoftwareSerial* sSerial, uint32_t Baud)
{
  i2cUartFlag = GNSS_UART_FLAG;
  _serial = sSerial;
  _baud = Baud;
  // _serial->begin(_baud);
}
#else
DFRobot_GNSSAndRTC_UART::DFRobot_GNSSAndRTC_UART(HardwareSerial* hSerial, uint32_t Baud, uint8_t txpin, uint8_t rxpin)
{
  i2cUartFlag = GNSS_UART_FLAG;
  _serial = hSerial;
  _baud = Baud;
  _txpin = txpin;
  _rxpin = rxpin;
}
#endif

bool DFRobot_GNSSAndRTC_UART::begin()
{
#ifdef ESP32
  _serial->begin(_baud, SERIAL_8N1, _txpin, _rxpin);
  // #elif defined(ARDUINO_AVR_UNO) || defined(ESP8266)
    // nothing use software
#else
  _serial->begin(_baud);  // M0 cannot create a begin in a construct
#endif

  uint8_t data[2] = { 0 };
  readReg(REG_CS32_PID, data, 2);
  if (MODULE_DFR1103_PID != (data[0] | (data[1] << 8))) {
    DBG((data[0] | (data[1] << 8)), HEX);
    return false;
  }
  return DFRobot_GNSSAndRTC::begin();
}

void DFRobot_GNSSAndRTC_UART::writeReg(uint8_t reg, void* pBuf, size_t size)
{
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;

  _serial->write(UART0_WRITE_REGBUF);
  _serial->write(reg);
  _serial->write(size);
  for (size_t i = 0; i < size; i++) {
    _serial->write(_pBuf[i]);
  }
  delay(50);
}

uint8_t DFRobot_GNSSAndRTC_UART::readReg(uint8_t reg, void* pBuf, size_t size)
{
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;

  if ((reg >= 0x30) && (reg <= 0x79) && (size != 0)) {
    uint8_t data[] = { reg, (uint8_t)size };
    writeReg(REG_RTC_READ_REG, data, 2);
    delay(50);
  }

  _serial->write(UART0_READ_REGBUF);   // read type
  _serial->write(reg);   // read reg
  _serial->write(size);   // read len
  uint32_t nowtime = millis();
  size_t i = 0;
  while (millis() - nowtime < TIME_OUT) {
    while (_serial->available()) {
      _pBuf[i++] = _serial->read();
      // delay(1);
    }
    if (i == size) {
      // DBG(i)
      // DBG(millis() - nowtime);
      break;
    }
  }
  return 0;
}
