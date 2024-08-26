# -*- coding:utf-8 -*-
'''!
    @file DFRobot_GNSSAndRTC.py
    @brief Define the basic structure of class DFRobot_GNSSAndRTC
    @details Define SD3031 and L76K functions
    @copyright	Copyright (c) 2024 DFRobot Co.Ltd (http://www.dfrobot.com)
    @license The MIT License (MIT)
    @author [thdyyl](yuanlong.yu@dfrobot.com)
    @version V0.0.1
    @date 2024-07-10
    @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
from __future__ import absolute_import
import os
import sys
import time
import serial
import smbus
import struct
import six
import logging
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), './')))
from src.L76K import DFRobot_GNSS
from src.SD3031 import DFRobot_SD3031

logger = logging.getLogger()
logger.setLevel(logging.INFO)
ph = logging.StreamHandler()
formatter = logging.Formatter("%(asctime)s - [%(filename)s %(funcName)s]:%(lineno)d - %(levelname)s: %(message)s")
ph.setFormatter(formatter)
logger.addHandler(ph)

class DFRobot_GNSSAndRTC(DFRobot_GNSS, DFRobot_SD3031):
    MODULE_I2C_ADDRESS = 0x66  # < Sensor device address

    # Custom function registers 0x24 ~ 0x2F
    REG_CALIB_STATUS_REG = 0x2A
    REG_CALIB_RTC_REG = 0x2B

    REG_RTC_READ_REG = 0x2E
    REG_RTC_READ_LEN = 0x2F

    # Custom function registers 0xAA ~ (0xB0 - 1)
    REG_CS32_PID = 0xAA
    REG_CS32_VID = 0xAC
    REG_CS32_VERSION = 0xAE

    MODULE_DFR1103_PID = 0x444F  # Sensor PID
    MODULE_DFR1103_VID = 0x3343  # < Sensor VID
    MODULE_DFR1103_VERSION = 0x0100  # < Sensor VERSION

    # RTC 模块校准状态
    ECALIB_NONE = 0x00
    ECALIB_COMPLETE = 0x01
    EUNDER_CALIB = 0x02

    def begin(self):
        '''!
          @brief subclass initialization function
          @return bool type, means returning initialization status
          @retval true NO_ERROR
        '''
        data = [0x80]
        self._write_reg(DFRobot_SD3031.SD3031_REG_I2C_CON, data, 1)
        return True

    def calib_rtc(self):
        '''!
          @brief Calibrate RTC immediately with GNSS
          @note This is a single calibration;
          @n If the GNSS module signal is weak, time calibration may encounter issues.
        '''
        mode = [self.EUNDER_CALIB]
        self._write_reg(self.REG_CALIB_STATUS_REG, mode, 1)

    def calib_rtc_hour(self, hour):
        '''!
          @brief The loop automatically performs GNSS timing based on the set interval
          @param hour Automatic calibration of the time interval. range: 0~255, unit: hour.
          @note When set to zero, automatic time calibration is disabled.
          @n Enabling it will trigger an immediate calibration.
          @n If the GNSS module signal is weak, time calibration may encounter issues.
        '''
        self._write_reg(self.REG_CALIB_RTC_REG, [hour], 1)

    def calib_status(self, mode=True):
        '''!
          @brief Current clock calibration status
          @param mode By default, it is set to true, indicating access to the calibration status only.
          @n  If continuous calibration for one minute does not return a successful calibration,
          @n  you can pass in false to manually terminate this calibration session.
          @return uint8_t type, indicates current clock calibration status
          @retval 0 Not calibrated
          @retval 1 Calibration complete
          @retval 2 Under calibration
          @note Note: To avoid affecting subsequent calibration status,
          @n    "Calibration completed Status (1)" is automatically zeroed after a successful read
        '''
        status = [self.ECALIB_NONE]
        if mode:
            self._read_reg(self.REG_CALIB_STATUS_REG, status, 1)
        else:
            self._write_reg(self.REG_CALIB_STATUS_REG, status, 1)
        return status[0] & 0xff


class DFRobot_GNSSAndRTC_I2C(DFRobot_GNSSAndRTC):
    def __init__(self, i2c_bus=1, addr=DFRobot_GNSSAndRTC.MODULE_I2C_ADDRESS):
        super(DFRobot_GNSSAndRTC_I2C, self).__init__()
        self.i2c_uart_flag = DFRobot_GNSSAndRTC.GNSS_I2C_FLAG
        self.__i2c_bus = smbus.SMBus(i2c_bus)
        self.__device_addr = addr

    def begin(self):
        if not self.scan():
            return False
        return super(DFRobot_GNSSAndRTC_I2C, self).begin()

    def _write_reg(self, reg, p_buf, size):
        if not p_buf:
            logger.warning("p_buf ERROR!")
            return 1
        buf = p_buf[:size]
        try:
            self.__i2c_bus.write_i2c_block_data(self.__device_addr, reg, buf)
            # !!!
            time.sleep(0.05)
            return 0
        except KeyboardInterrupt:
            raise
        except:
            logger.warning("Write: I2C communication failed, please check the peripherals.!")
            time.sleep(0.05)
            return 1

    def _read_reg(self, reg, p_buf, size):
        if not p_buf:
            logger.warning("p_buf ERROR!")
            return 1
        if (reg >= 0x30) and (reg <= 0x79) and (size != 0):
            if self._write_reg(self.REG_RTC_READ_REG, [reg, size], 2) == 1:
                return 1
            time.sleep(0.05)
        try:
            #buf = self.__i2c_bus.read_i2c_block_data(self.__device_addr, reg, size)
            for i in range(size):
                #p_buf[i] = buf[i]
                if reg == self.REG_ALL_DATA:
                    p_buf[i] = self.__i2c_bus.read_byte_data(self.__device_addr, reg)
                else:
                    p_buf[i] = self.__i2c_bus.read_byte_data(self.__device_addr, reg + i)
            return 0
        except KeyboardInterrupt:
            raise
        except:
            logging.warning("Read: I2C communication failed, please check the peripherals.!")
            return 1

    def scan(self):
        try:
            self.__i2c_bus.read_byte(self.__device_addr)
            return True
        except KeyboardInterrupt:
            raise
        except:
            return False


class DFRobot_GNSSAndRTC_UART(DFRobot_GNSSAndRTC):
    UART_BAUDRATE = 57600
    UART0_READ_REGBUF = 0xBB
    UART0_WRITE_REGBUF = 0xCC

    UART_SERIAL_NAME = "/dev/serial0"

    __baud = UART_BAUDRATE
    __rxpin = 0x00
    __txpin = 0x00

    __serial = None
    __serial_name = UART_SERIAL_NAME

    def __init__(self, serial_name=UART_SERIAL_NAME, baud=UART_BAUDRATE):
        super(DFRobot_GNSSAndRTC_UART, self).__init__()
        self.i2c_uart_flag = self.GNSS_UART_FLAG
        self.__serial_name = serial_name
        self.__baud = baud

    def begin(self):
        self.__serial = serial.Serial(self.__serial_name, self.__baud)
        self.__serial.flush()
        if not self.__serial.isOpen():
            return False
        data = [0x00] * 2
        self._read_reg(self.REG_CS32_PID, data, 2)
        if self.MODULE_DFR1103_PID != (data[0] | (data[1] << 8)):
            return False
        return super(DFRobot_GNSSAndRTC_UART, self).begin()
    
    def int_to_bytes(self, n, length, byteorder='big'):
        if byteorder == 'big':
            return struct.pack('>I', n)[-length:]
        elif byteorder == 'little':
            return struct.pack('<I', n)[:length]
        else:
            raise ValueError("byteorder 必须是 'big' 或 'little'")

    def _write_reg(self, reg, p_buf, size):
        if not p_buf:
            return 1
        '''
        self.__serial.write(self.UART0_WRITE_REGBUF.to_bytes(1, byteorder='big'))
        self.__serial.write(reg.to_bytes(1, byteorder='big'))
        self.__serial.write(size.to_bytes(1, byteorder='big'))
        for i in range(0, size):
            self.__serial.write(p_buf[i].to_bytes(1, byteorder='big'))
        '''
        try:
            self.__serial.write(self.int_to_bytes(self.UART0_WRITE_REGBUF, 1))
            self.__serial.write(self.int_to_bytes(reg, 1))
            self.__serial.write(self.int_to_bytes(size, 1))
            for i in range(0, size):
                self.__serial.write(self.int_to_bytes(p_buf[i], 1))
            time.sleep(0.05)
            return 0
        except KeyboardInterrupt:
            raise
        except:
            logger.warning("Write: UART communication failed, please check the peripherals!")
            return 1

    def _read_reg(self, reg, p_buf, size):
        if not p_buf:
            return 1
        try:
            if (reg >= 0x30) and (reg <= 0x79) and (size != 0):
                data = [reg, size]
                self._write_reg(self.REG_RTC_READ_REG, data, 2)
                time.sleep(0.05)
            '''
            self.__serial.write(self.UART0_READ_REGBUF.to_bytes(1, byteorder='big'))
            self.__serial.write(reg.to_bytes(1, byteorder='big'))
            self.__serial.write(size.to_bytes(1, byteorder='big'))
            '''
            self.__serial.write(self.int_to_bytes(self.UART0_READ_REGBUF, 1))
            self.__serial.write(self.int_to_bytes(reg, 1))
            self.__serial.write(self.int_to_bytes(size, 1))
            nowtime = time.time() * 1000
            i = 0
            while time.time() * 1000 - nowtime < self.TIME_OUT:
                while self.__serial.in_waiting > 0:
                    if six.PY3:
                        p_buf[i] = self.__serial.read(1)[0]
                    else:
                        p_buf[i] = ord(self.__serial.read(1)[0])
                    i += 1
                    if i == size:
                        break
                if i == size:
                    break
            return 0
        except KeyboardInterrupt:
            raise
        except:
            logger.warning("Read: UART communication failed, please check the peripherals!")
            return 1

