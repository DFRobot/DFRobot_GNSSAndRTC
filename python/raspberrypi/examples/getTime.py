# -*- coding:utf-8 -*-
'''!
  @file getTime.py
  @brief Run this routine, set internal clock first, and then circularly get clock, temperature and voltage data
  @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license      The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V0.0.1
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
import sys
import time
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../')))

# I2C_UART_FLAG = "I2C"
I2C_UART_FLAG = "UART"
if I2C_UART_FLAG == "I2C":
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_I2C
    rtc = DFRobot_GNSSAndRTC_I2C(1)
else:
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_UART
    rtc = DFRobot_GNSSAndRTC_UART("/dev/serial0")


def setup():
    while not rtc.begin():
        print("Failed to init chip, please check if the chip connection is fine. ")
        time.sleep(1)
    rtc.set_hour_system(rtc.E12HOURS)
    rtc.set_time(2023, 7, 27, 11, 59, 0)
    

def loop():
    s_time = rtc.get_rtc_time()
    print("{}/{}/{} ({}) {}:{}:{} {}".format(s_time.year, s_time.month, s_time.day, s_time.week.decode(), 
                                             s_time.hour, s_time.minute, s_time.second, rtc.get_am_or_pm()))
    print("{}℃, {}V".format(rtc.get_temperature_c(), rtc.get_voltage()))
    time.sleep(0.6)


if __name__ == "__main__":
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        exit()
