# -*- coding:utf-8 -*-
'''!
  @file gnssCalibRTC.py
  @brief Run this routine, calibration internal clock first, and then circularly get clock
  @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license      The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V1.0.0
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
import sys
import time
import os            
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../')))

#I2C_UART_FLAG = "I2C"
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
    rtc.set_hour_system(rtc.E24HOURS)

    rtc.calib_rtc_hour(1)


under_calib_count = 0


def loop():
    global under_calib_count
    status = rtc.calib_status()
    if rtc.ECALIB_COMPLETE == status:
        under_calib_count = 0
        print("Calibration success!")
    elif rtc.EUNDER_CALIB == status:
        under_calib_count = under_calib_count + 1
        if 60 <= under_calib_count:
            rtc.calib_status(False)
            under_calib_count = 0
            print("Calibration failed!")
            print("It may be due to week satellite signals.")
            print("Please proceed to an open outdoor area for time synchronization.")
    s_time = rtc.get_rtc_time()
    print("{}/{}/{} ({}) {}:{}:{}".format(s_time.year, s_time.month, s_time.day, s_time.week.decode("utf-8"), s_time.hour, s_time.minute, s_time.second))
    time.sleep(0.9)


if __name__ == "__main__":
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        exit()
