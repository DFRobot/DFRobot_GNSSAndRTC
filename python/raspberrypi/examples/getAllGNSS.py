# -*- coding:utf-8 -*-
'''!
  @file  getAllGNSS.py
  @brief read all gnss data
  @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V0.0.1
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
from __future__ import print_function
import sys
import time
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../')))

#I2C_UART_FLAG = "I2C"
I2C_UART_FLAG = "UART"
if I2C_UART_FLAG == "I2C":
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_I2C
    gnss = DFRobot_GNSSAndRTC_I2C(1)
else:
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_UART
    gnss = DFRobot_GNSSAndRTC_UART("/dev/serial0")


def callback(data, len):
    for i in range(len):
        print(chr(data[i]), end='')


def setup():
    while not gnss.begin():
        print("No Deivce!")
        time.sleep(1)
    gnss.enable_power()

    gnss.set_gnss(gnss.EGPS_BEIDOU_GLONASS)

    gnss.set_callback(callback)


def loop():
    print("\n---------------------------Raw data from L76K-------------------------------")
    gnss.get_all_gnss()
    time.sleep(3)


if __name__ == '__main__':
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        exit()
