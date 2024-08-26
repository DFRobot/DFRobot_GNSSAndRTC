# -*- coding:utf-8 -*-
'''!
    @file __init__.py
    @brief Define the basic structure of class DFRobot_SD3031
    @details Define SD3031 functions
    @copyright	Copyright (c) 2024 DFRobot Co.Ltd (http://www.dfrobot.com)
    @license The MIT License (MIT)
    @author [thdyyl](yuanlong.yu@dfrobot.com)
    @version V0.0.1
    @date 2024-07-10
    @url https://github.com/DFRobot/DFRobot_SD3031
'''
from abc import ABCMeta, abstractmethod
import time
from ctypes import *

days_in_month = [31,28,31,30,31,30,31,31,30,31,30,31]


def date2days(y, m, d):
    if y >= 2000:
        y -= 2000
    days = d
    for i in range(m-1):
        days += days_in_month[i]
    if m > 2 and y % 4 == 0 and (y % 100 != 0 or y % 400 == 0):
        days = days + 1
    return days + 365 * y + (y + 3) // 4 - 1


class DFRobot_SD3031(object):
    __metaclass__ = ABCMeta
    # The address is offset 0x30 because it shares a register with the GNSS module
    SD3031_REG_SEC = (0x00 + 0x30)  # < RTC Seconds Register
    SD3031_REG_MIN = (0x01 + 0x30)  # < RTC Minutes Register
    SD3031_REG_HOUR = (0x02 + 0x30)  # < RTC Hours Register
    SD3031_REG_WEEK = (0x03 + 0x30)  # < RTC Week Register
    SD3031_REG_DAY = (0x04 + 0x30)  # < RTC Day Register
    SD3031_REG_MONTE = (0x05 + 0x30)  # < RTC Month Register
    SD3031_REG_YEAR = (0x06 + 0x30)  # < RTC Year Register
    SD3031_REG_ALARM_SEC = (0x07 + 0x30)  # < RTC Seconds Alarm Register
    SD3031_REG_ALARM_MIN = (0x08 + 0x30)  # < RTC Minutes Alarm Register
    SD3031_REG_ALARM_HOUR = (0x09 + 0x30)  # < RTC Hours Alarm Register
    SD3031_REG_ALARM_WEEK = (0x0A + 0x30)  # < RTC Week Alarm Register
    SD3031_REG_ALARM_DAY = (0x0B + 0x30)  # < RTC Day Alarm Register
    SD3031_REG_ALARM_MONNTE = (0x0C + 0x30)  # < RTC Month Alarm Register
    SD3031_REG_ALARM_YEAR = (0x0D + 0x30)  # < RTC Year Alarm Register
    SD3031_REG_ALARM_CON = (0x0E + 0x30)  # < RTC Alarm Control Register
    SD3031_REG_CTR1 = (0x0F + 0x30)  # < Control Register 1
    SD3031_REG_CTR2 = (0x10 + 0x30)  # < Control Register 2
    SD3031_REG_CTR3 = (0x11 + 0x30)  # < Control Register 3
    SD3031_REG_COUNTDOWM = (0X13 + 0x30)  # < Countdown Register
    SD3031_REG_TEMP = (0x16 + 0x30)  # < Internal Temperature Register
    SD3031_REG_I2C_CON = (0x17 + 0x30)  # < I2C Control
    SD3031_REG_BAT_VAL = (0x1A + 0x30)  # < Battery Level

    class STimeData_t(Structure):
        '''!
          @struct STimeData_t
          @brief Structure for storing time data
        '''
        _fields_ = [('year', c_uint16),
                    ('month', c_uint8),
                    ('day', c_uint8),
                    ('week', c_char_p),
                    ('hour', c_uint8),
                    ('minute', c_uint8),
                    ('second', c_uint8)]

    '''!
      @biref Time system: 12hours, 24hours
    '''
    E12HOURS = 0
    E24HOURS = 1 << 7

    '''!
      @brief Enumerate week definition
    '''
    ESUNDAY = 0x01
    EMONDAY = 0x02
    ETUESDAY = 0x04
    EWEDNESDAY = 0x08
    ETHURSDAY = 0x10
    EFRIDAY = 0x20
    ESATURDAY = 0x40
    EEVERYDAY = 0x7f
    EWORKDAY = 0x3e

    '''!
    @brief Enumerate interrupt definition
    '''
    EYEAR_ENABLE = 0x40
    EMONDAY_ENABLE = 0x20
    EDAY_ENABLE = 0x10
    EHOURS_ENABLE = 0x04
    EMINUTE_ENABLE = 0x02
    ESECOND_ENABLE = 0x01

    def get_rtc_time(self):
        '''!
          @brief Get information of year in RTC module
          @return Return the obtained year
        '''
        s_time = self.STimeData_t()
        buffer = [0x00] * 7
        data = 0x00
        self._read_reg(self.SD3031_REG_SEC, buffer, 7)
        s_time.year = 2000 + self.__bcd2bin(buffer[6])
        s_time.month = self.__bcd2bin(buffer[5])
        s_time.day = self.__bcd2bin(buffer[4])
        data = self.__bcd2bin(buffer[3])

        switchs = {
            0: "Sunday",
            1: "Monday",
            2: "Tuesday",
            3: "Wednesday",
            4: "Thursday",
            5: "Friday",
            6: "Saturday"
        }

        s_time.week = switchs.get(data, "").encode('utf-8')
        data = buffer[2]
        if self.__mode == self.E24HOURS:
            s_time.hour = self.__bcd2bin(data & 0x7f)
        else:
            s_time.hour = self.__bcd2bin(data & 0x1f)

        s_time.minute = self.__bcd2bin(buffer[1])
        s_time.second = self.__bcd2bin(buffer[0])
        return s_time

    def set_hour_system(self, mode):
        '''!
          @brief Set clock as 24-hour or 12-hour format
          @param mode Clock calculation method
        '''
        bcd_time = 0x00
        bin_time = 0x00
        buffer = [0x00] * 7
        self._read_reg(self.SD3031_REG_SEC, buffer, 7)
        bcd_time = buffer[2]

        if self.__mode != (bcd_time & 0x80):
            if self.__mode != mode:
                self.__mode = mode
                return
        else:
            if self.__mode == mode:
                return
            self.__mode = mode

        if self.E24HOURS == (bcd_time & 0x80):
            bin_time = self.__bcd2bin(bcd_time & 0x7f)
            if bin_time == 0:
                bcd_time = 0x12
            elif bin_time > 0 and bin_time < 12:
                bcd_time = (0x00 | self.__bin2bcd(bin_time))
            elif bin_time == 12:
                bcd_time = 0x32
            elif bin_time > 12 and bin_time < 24:
                bcd_time = (0x20 | self.__bin2bcd(bin_time - 12))
        else:
            bin_time = self.__bcd2bin(bcd_time & 0x1f)
            if bcd_time & 0x20:
                if 12 != bin_time:
                    bin_time += 12
            else:
                if 12 == bin_time:
                    bin_time = 0
            bcd_time = self.__bin2bcd(bin_time) | 0x80

        buffer[2] = bcd_time
        self._write_reg(self.SD3031_REG_SEC, buffer, 7)

    def set_time(self, year, month, day, hour, minute, second):
        '''!
          @brief Set time into rtc and take effect immediately
          @param year 2000~2099
          @param month 1~12
          @param day 1~31
          @param hour 0~23
          @param minute 0~59
          @param second 0~59
        '''
        _hour, _year, week = 0x00, 0x00, 0x00
        buffer = [0x00] * 7
        _year = year - 2000
        if self.__mode == self.E24HOURS:
            _hour = self.__bin2bcd(hour) | 0x80
        else:
            if hour == 0:
                _hour = 0x12
            elif hour > 0 and hour < 12:
                _hour = 0x00 | self.__bin2bcd(hour)
            elif hour == 12:
                _hour = 0x32
            elif hour > 12 and hour < 24:
                _hour = 0x20 | self.__bin2bcd(hour - 12)

        week = (date2days(year, month, day) + 6) % 7

        buffer[0] = self.__bin2bcd(second)
        buffer[1] = self.__bin2bcd(minute)
        buffer[2] = _hour
        buffer[3] = self.__bin2bcd(week)
        buffer[4] = self.__bin2bcd(day)
        buffer[5] = self.__bin2bcd(month)
        buffer[6] = self.__bin2bcd(_year)

        self._write_reg(self.SD3031_REG_SEC, buffer, 7)

    def set_alarm(self, year, month, day):
        '''!
          @brief Set the data for triggering alarm
          @param year 2000~2099
          @param month 1~12
          @param day 1~31
        '''
        buffer = [0x00] * 8
        _year = 0x00
        data = 0x00
        data = 0x80
        self._write_reg(self.SD3031_REG_CTR3, [data], 1)
        data = 0x92
        self._write_reg(self.SD3031_REG_CTR2, [data], 1)
        _year = year - 2000
        buffer[0] = 0
        buffer[1] = 0
        buffer[2] = 0
        buffer[3] = 0
        buffer[4] = self.__bin2bcd(day)
        buffer[5] = self.__bin2bcd(month)
        buffer[6] = self.__bin2bcd(_year)
        buffer[7] = 0x70
        self._write_reg(self.SD3031_REG_ALARM_SEC, buffer, 8)

    def set_alarm(self, week, hour, minute, second):
        '''!
          @brief Set the Alarmnumber object
          @param week eWeek_t
          @param hour 0~23
          @param minute 0~59
          @param second 0~59
        '''
        buffer = [0x00] * 8
        _hour = 0x00
        data = 0x00
        data = 0x80
        self._write_reg(self.SD3031_REG_CTR3, [data], 1)
        data = 0x92
        self._write_reg(self.SD3031_REG_CTR2, [data], 1)
        if self.__mode == self.E24HOURS:
            _hour = self.__bin2bcd(hour) | 0x80
        else:
            if hour == 0:
                _hour = 0x12
            elif hour > 0 and hour < 12:
                _hour = 0x80 | self.__bin2bcd(hour)
            elif hour == 12:
                _hour = 0x32
            elif hour > 12 and hour < 24:
                _hour = 0x20 | self.__bin2bcd(hour - 12)
        buffer[0] = self.__bin2bcd(second)
        buffer[1] = self.__bin2bcd(minute)
        buffer[2] = _hour
        buffer[3] = week
        buffer[4] = 0
        buffer[5] = 0
        buffer[6] = 0
        buffer[7] = 0x0f
        self._write_reg(self.SD3031_REG_ALARM_SEC, buffer, 8)

    def get_temperature_c(self):
        '''!
          @brief Get internal temperature of the clock
          @return Return the obtained temperature, unit8: ℃
        '''
        data = [0x00]
        self._read_reg(self.SD3031_REG_TEMP, data, 1)
        return data[0]

    def get_voltage(self):
        '''!
          @brief Get voltage of onboard battery
          @return float Return the obtained voltage
        '''
        buffer = [0x00] * 2
        data = 0x00
        ret = 0.0
        self._read_reg(self.SD3031_REG_BAT_VAL, buffer, 2)
        data = (((buffer[0] & 0x80) >> 7) << 8) | buffer[1]
        ret = data / 100.0
        return ret

    def clear_alarm(self):
        '''!
          @brief Clear alarm flag bit
        '''
        buffer = [0x00]
        self._read_reg(self.SD3031_REG_CTR1, buffer, 1)

    def get_am_or_pm(self):
        '''!
          @brief output AM or PM of time
          @return AM or PM, return empty string for 24 hours mode
        '''
        if self.__mode == self.E24HOURS:
            return ""
        buffer = [0x00] * 7
        data = 0x00
        self._read_reg(self.SD3031_REG_SEC, buffer, 7)
        data = buffer[2]
        if data & 0x20:
            return "PM"
        return "AM"

    def enable_32k(self):
        '''!
          @brief enable the 32k output
        '''
        flag1 = 0x00
        data = [0x00]
        self._read_reg(self.SD3031_REG_CTR3, data, 1)
        flag1 = data[0] & 0xBF
        self._write_reg(self.SD3031_REG_CTR3, [flag1], 1)
        time.sleep(0.1)

    def disable_32k(self):
        '''!
          @brief disable the 32k output
        '''
        flag1 = 0x00
        data = [0x00]
        self._read_reg(self.SD3031_REG_CTR3, data, 1)
        flag1 = data[0] | 0x40
        self._write_reg(self.SD3031_REG_CTR3, [flag1], 1)
        time.sleep(0.1)

    def write_sram(self, addr, data):
        '''!
          @brief write the SRAM
          @param addr 0x2c~0x71
          @param data uint8_t HEX
        '''
        self._write_reg(addr, [data], 1)

    def read_sram(self, addr):
        '''!
          @brief read the SRAM
          @param addr 0x2c~0x71
          @return data stored in the SRAM
        '''
        buffer = [0x00]
        self._read_reg(addr, buffer, 1)
        return buffer[0]

    def clear_sram(self, addr):
        '''!
          @brief clear the SRAM
          @param addr 0x2c~0x71
        '''
        self._write_reg(addr, [0xff], 1)

    def count_down(self, second):
        '''!
          @brief Countdown
          @param second  countdown time 0~0xffffff
        '''
        data = 0x00
        buffer = [0x00] * 3
        _second = 0x000000
        if second > 0xffffff:
            _second = 0xffffff
        else:
            _second = second

        self.clear_alarm()

        data = 0x80
        self._write_reg(self.SD3031_REG_CTR2, [data], 1)
        data = 0xB4
        self._write_reg(self.SD3031_REG_CTR2, [data], 1)
        data = 0x20
        self._write_reg(self.SD3031_REG_CTR3, [data], 1)
        buffer[0] = _second & 0xff
        buffer[1] = (_second >> 8) & 0xff
        buffer[2] = (_second >> 16) & 0xff
        self._write_reg(self.SD3031_REG_COUNTDOWM, buffer, 3)

    def __bcd2bin(self, val):
        '''!
          @brief BCD code to BIN code
          @param val Input BCD code
          @return Return BIN code
        '''
        return val - 6 * (val >> 4)

    def __bin2bcd(self, val):
        '''!
          @brief BIN code to BCD code
          @param val Input BIN code
          @return Return BCD code
        '''
        return val + 6 * (val // 10)

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
        '''
          @brief Read register function, designed as virtual function, implemented by derived class
          @param reg  Register address 8bits
          @param pBuf Storage and buffer for data to be read
          @param size Length of data to be read
          @return uint8_t type, indicates returning read register status
          @retval 0 Reading succeeded
          @retval 1 Reading failed
        '''
        pass

    __mode = E24HOURS


