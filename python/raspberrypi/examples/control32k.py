# -*- coding:utf-8 -*-
'''!
  @file control32k.py
  @brief Run this routine to control pin 32K to output square wave
  @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license      The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V1.0.0
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
import sys
import os
import time

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
        print("Failed to init chip, please check if the chip connection is fine.")
        time.sleep(1)


def loop():
    rtc.enable_32k()
    print("Enable 32k square wave output.")
    time.sleep(1)
    rtc.disable_32k()
    print("Turn off the 32k square wave output.")
    time.sleep(1)    


if __name__ == "__main__":
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        exit()
