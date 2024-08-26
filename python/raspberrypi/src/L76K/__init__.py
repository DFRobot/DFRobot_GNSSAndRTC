# -*- coding:utf-8 -*-
'''!
    @file __init__.py
    @brief Define the basic structure of the DFRobot_GNSS class, the implementation of the basic methods
    @copyright	Copyright (c) 2024 DFRobot Co.Ltd (http://www.dfrobot.com)
    @license The MIT License (MIT)
    @author [thdyyl](yuanlong.yu@dfrobot.com)
    @version V0.0.1
    @date 2024-07-10
    @url https://github.com/DFRobot/DFRobot_GNSS
'''
from __future__ import unicode_literals
from abc import ABCMeta, abstractmethod
import time
from ctypes import *
import six

class DFRobot_GNSS(object):
    __metaclass__ = ABCMeta
    REG_YEAR_H = 0
    REG_YEAR_L = 1
    REG_MONTH = 2
    REG_DATE = 3
    REG_HOUR = 4
    REG_MINUTE = 5
    REG_SECOND = 6

    REG_LAT_1 = 7
    REG_LAT_2 = 8
    REG_LAT_X_24 = 9
    REG_LAT_X_16 = 10
    REG_LAT_X_8 = 11
    REG_LAT_DIS = 12

    REG_LON_1 = 13
    REG_LON_2 = 14
    REG_LON_X_24 = 15
    REG_LON_X_16 = 16
    REG_LON_X_8 = 17
    REG_LON_DIS = 18

    REG_USE_STAR = 19

    REG_ALT_H = 20
    REG_ALT_L = 21
    REG_ALT_X = 22

    REG_SOG_H = 23
    REG_SOG_L = 24
    REG_SOG_X = 25

    REG_COG_H = 26
    REG_COG_L = 27
    REG_COG_X = 28

    REG_START_GET = 29
    REG_I2C_ADDR = 30
    REG_DATA_LEN_H = 31
    REG_DATA_LEN_L = 32
    REG_ALL_DATA = 33

    REG_GNSS_MODE = 34
    REG_SLEEP_MODE = 35

    GNSS_I2C_FLAG = 1
    GNSS_UART_FLAG = 2

    TIME_OUT = 200  # < 200 ms time out

    ENABLE_POWER = 0
    DISABLE_POWER = 1

    UART_MAX_READ_LEN = 250
    I2C_MAX_READ_LEN = 32

    class STim_t(Structure):
        '''!
          @struct STim_t
          @brief Store the time and date information obtained from GPS
        '''
        _fields_ = [('year', c_uint16),
                    ('month', c_uint8),
                    ('date', c_uint8),
                    ('hour', c_uint8),
                    ('minute', c_uint8),
                    ('second', c_uint8)]

    class SLonLat_t(Structure):
        '''!
          @struct SLonLat_t
          @brief Store latitude, longitude and direction information obtained from GPS
        '''
        _fields_ = [('lonDDD', c_uint8),
                    ('lonMM', c_uint8),
                    ('lonMMMMM', c_uint),
                    ('lonDirection', c_char),
                    ('latDD', c_uint8),
                    ('latMM', c_uint8),
                    ('latMMMMM', c_uint),
                    ('latDirection', c_char),
                    ('latitude', c_double),
                    ('latitudeDegree', c_double),
                    ('lonitude', c_double),
                    ('lonitudeDegree', c_double)]

    '''!
      @brief GNSS MODE ENUM
    '''
    EGPS = 1
    EBEIDOU = 2
    EGPS_BEIDOU = 3
    EGLONASS = 4
    EGPS_GLONASS = 5
    EBEIDOU_GLONASS = 6
    EGPS_BEIDOU_GLONASS = 7

    i2c_uart_flag = 0x00

    def get_utc(self):
        '''!
          @brief Get UTC, standard time
          @return STim_t type, represents the returned hour, minute and second
          @retval STim_t.hour hour
          @retval STim_t.minute minute
          @retval STim_t.second second
        '''
        data = self.STim_t()
        _send_data = [0x00] * 10
        self._read_reg(self.REG_HOUR, _send_data, 3)
        data.hour = _send_data[0]
        data.minute = _send_data[1]
        data.second = _send_data[2]
        return data

    def get_date(self):
        '''!
          @brief Get date information, year, month, day
          @return sTim_t type, represents the returned year, month, day
          @retval sTim_t.year year
          @retval sTim_t.month month
          @retval sTim_t.date date
        '''
        data = self.STim_t()
        _send_data = [0x00] * 10
        self._read_reg(self.REG_YEAR_H, _send_data, 4)
        data.year = (_send_data[0] << 8) | _send_data[1]
        data.month = _send_data[2]
        data.date = _send_data[3]
        return data

    def get_lat(self):
        '''!
          @brief Get latitude
          @return SLonLat_t type, represents the returned latitude
          @retval SLonLat_t.latDD   Latitude degree(0-90)
          @retval SLonLat_t.latMM   The first and second digits behind the decimal point
          @retval SLonLat_t.latMMMMM Latitude  The third and seventh digits behind the decimal point
          @retval SLonLat_t.latitude Latitude value with 7 decimal digits
          @retval SLonLat_t.latDirection Direction of latitude
        '''
        data = self.SLonLat_t()
        _send_data = [0x00] * 10
        self._read_reg(self.REG_LAT_1, _send_data, 6)
        data.latDD = _send_data[0]
        data.latMM = _send_data[1]
        data.latMMMMM = (_send_data[2] << 16) | (_send_data[3] << 8) | _send_data[4]
        data.latitude = data.latDD * 100.0 + data.latMM + data.latMMMMM / 100000.0
        data.latitudeDegree = data.latDD + data.latMM / 60.0 + data.latMMMMM / 100000.0 / 60.0
        if six.PY3:
            data.latDirection = _send_data[5]
        else:
            data.latDirection = c_char(chr(_send_data[5]))
        # data.latDirection = _send_data[5]
        return data

    def get_lon(self):
        '''!
          @brief Get longitude
          @return SLonLat_t Type, represents the returned longitude
          @retval SLonLat_t.lonDDD  Longitude degree(0-90)
          @retval SLonLat_t.lonMM   Longitude  The first and second digits behind the decimal point
          @retval SLonLat_t.lonMMMMM Longitude The third and seventh digits behind the decimal point
          @retval SLonLat_t.lonitude Longitude value with 7 decimal digits
          @retval SLonLat_t.lonDirection Direction of longi
        '''
        data = self.SLonLat_t()
        _send_data = [0x00] * 10
        self._read_reg(self.REG_LON_1, _send_data, 6)
        data.lonDDD = _send_data[0]
        data.lonMM = _send_data[1]
        data.lonMMMMM = (_send_data[2] << 16) | (_send_data[3] << 8) | _send_data[4]
        data.lonitude = data.lonDDD * 100.0 + data.lonMM + data.lonMMMMM / 100000.0

        data.lonitudeDegree = data.lonDDD + data.lonMM / 60.0 + data.lonMMMMM / 100000.0 / 60.0
        if six.PY3:
            data.lonDirection = _send_data[5]
        else:
            data.lonDirection = c_char(chr(_send_data[5]))
        return data

    def get_num_sat_used(self):
        '''!
          @brief Get the number of the used satellite used
          @return uint8_t type, represents the number of the used satellite
        '''
        _send_data = [0x00] * 10
        self._read_reg(self.REG_USE_STAR, _send_data, 1)
        return _send_data[0] & 0xff

    def get_alt(self):
        '''!
          @brief Get altitude
          @return double type, represents altitude
        '''
        high = 0.0
        _send_data = [0x00] * 10
        self._read_reg(self.REG_ALT_H, _send_data, 3)
        if _send_data[0] & 0x80:
            high = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        else:
            high = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        return high

    def get_sog(self):
        '''!
          @brief Get speed over ground
          @return speed Float data(unit: knot)
        '''
        speed = 0.0
        _send_data = [0x00] * 10
        self._read_reg(self.REG_SOG_H, _send_data, 3)
        if _send_data[0] & 0x80:
            speed = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        else:
            speed = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        return speed

    def get_cog(self):
        '''!
          @brief Get course over ground
          @return Float data(unit: degree)
        '''
        cog = 0.0
        _send_data = [0x00] * 10
        self._read_reg(self.REG_COG_H, _send_data, 3)
        if _send_data[0] & 0x80:
            cog = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        else:
            cog = ((_send_data[0] & 0x7F) << 8 | _send_data[1]) + _send_data[2] / 100.0
        return cog

    def set_gnss(self, mode):
        '''!
          @brief Set GNSS to be used
          @param mode
          @n   eGPS              use gps
          @n   eBeiDou           use beidou
          @n   eGPS_BeiDou       use gps + beidou
          @n   eGLONASS          use glonass
          @n   eGPS_GLONASS      use gps + glonass
          @n   eBeiDou_GLONASS   use beidou +glonass
          @n   eGPS_BeiDou_GLONASS use gps + beidou + glonass
        '''
        _send_data = [0x00] * 10
        _send_data[0] = mode
        self._write_reg(self.REG_GNSS_MODE, _send_data, 1)
        time.sleep(0.05)

    def get_gnss_mode(self):
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
        _send_data = [0x00] * 10
        self._read_reg(self.REG_GNSS_MODE, _send_data, 1)
        return _send_data[0] & 0xff

    def get_all_gnss(self):
        '''!
          @brief Get GNSS data, call back and receive
        '''
        _send_data = [0x00] * 260
        len = self.__get_gnss_len()
        if len > 1024 + 200 or len == 0:
            return
        if self.i2c_uart_flag == self.GNSS_UART_FLAG:
            templen = len // self.UART_MAX_READ_LEN + 1
            for i in range(0, templen):
                if i == templen - 1:
                    if self._read_reg(self.REG_ALL_DATA, _send_data, len % self.UART_MAX_READ_LEN) == 1:
                        return
                    for j in range(0, len % self.UART_MAX_READ_LEN):
                        if _send_data[j] == 0x00:
                            _send_data[j] = ord('\n')
                    if self.callback:
                        self.callback(_send_data, len % self.UART_MAX_READ_LEN)
                else:
                    if self._read_reg(self.REG_ALL_DATA, _send_data, self.UART_MAX_READ_LEN) == 1:
                        return
                    for j in range(0, self.UART_MAX_READ_LEN):
                        if _send_data[j] == 0x00:
                            _send_data[j] = ord('\n')
                    if self.callback:
                        self.callback(_send_data, self.UART_MAX_READ_LEN)
        else:
            templen = len // self.I2C_MAX_READ_LEN + 1
            for i in range(0, templen):
                if i == templen - 1:
                    if self._read_reg(self.REG_ALL_DATA, _send_data, len % self.I2C_MAX_READ_LEN) == 1:
                        return
                    for j in range(0, len % self.I2C_MAX_READ_LEN):
                        if _send_data[j] == 0x00:
                            _send_data[j] = ord('\n')
                    if self.callback:
                        self.callback(_send_data, len % self.I2C_MAX_READ_LEN)
                else:
                    if self._read_reg(self.REG_ALL_DATA, _send_data, self.I2C_MAX_READ_LEN) == 1:
                        return
                    for j in range(0, self.I2C_MAX_READ_LEN):
                        if _send_data[j] == 0x00:
                            _send_data[j] = ord('\n')
                    if self.callback:
                        self.callback(_send_data, self.I2C_MAX_READ_LEN)
    def enable_power(self):
        '''!
          @brief Enable gnss power
        '''
        _send_data = [0x00] * 10
        _send_data[0] = self.ENABLE_POWER
        self._write_reg(self.REG_SLEEP_MODE, _send_data, 1)
        time.sleep(0.05)

    def disable_power(self):
        '''!
          @brief Disable gnss power
        '''
        _send_data = [0x00] * 10
        _send_data[0] = self.DISABLE_POWER
        self._write_reg(self.REG_SLEEP_MODE, _send_data, 1)
        time.sleep(0.05)

    def set_callback(self, callback):
        '''!
          @brief Set callback function type
          @param  callback function name
        '''
        self.callback = callback

    callback = None

    def __get_gnss_len(self):
        '''!
          @brief Get length of gnss data
          @return Length
        '''
        _send_data = [0x00] * 10
        _send_data[0] = 0x55
        if self._write_reg(self.REG_START_GET, _send_data, 1) == 1:
            return 0
        time.sleep(0.1)
        if self._read_reg(self.REG_DATA_LEN_H, _send_data, 2) == 1:
            return 0
        return _send_data[0] << 8 | _send_data[1]

    @abstractmethod
    def _write_reg(self, reg, p_buf, size):
        '''!
          @brief Write register function, designed as virtual function, implemented by derived class
          @param reg  Register address 8bits
          @param pBuf Storage and buffer for data to be written
          @param size Length of data to be written
        '''
        pass

    @abstractmethod
    def _read_reg(self, reg, p_buf, size):
        '''!
          @brief Read register function, designed as virtual function, implemented by derived class
          @param reg  Register address 8bits
          @param p_buf Storage and buffer for data to be read
          @param size Length of data to be read
          @return uint8_t type, indicates returning read register status
          @retval 0 Reading succeeded
          @retval 1 Reading failed
        '''
        pass

