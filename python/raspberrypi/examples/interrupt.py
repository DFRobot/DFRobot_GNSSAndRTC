# -*- coding:utf-8 -*-
'''!
  @file interrupt.py
  @brief Run this routine, set internal clock and interrupt trigger first, interrupt will be triggered when the set time is reached
  @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license      The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V0.0.1
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
import os
import sys
import time
import RPi.GPIO as GPIO
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../')))

#I2C_UART_FLAG = "I2C"
I2C_UART_FLAG = "UART"
if I2C_UART_FLAG == "I2C":
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_I2C
    rtc = DFRobot_GNSSAndRTC_I2C(1)
else:
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_UART
    rtc = DFRobot_GNSSAndRTC_UART("/dev/serial0")
alarm_flag = 0


#Interrupt trigger function
def interrupt(channel):
    global alarm_flag
    alarm_flag = 1


def gpio_init():
    PIN = 17
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(PIN, GPIO.FALLING, callback=interrupt)


def setup():
    gpio_init()
    while not rtc.begin():
        print("Failed to init chip, please check if the chip connection is fine. ")
        time.sleep(1)

    rtc.set_hour_system(rtc.E24HOURS)
    rtc.set_time(2024, 2, 28, 23, 59, 57)

    rtc.set_alarm(rtc.EEVERYDAY, 0, 0, 4)


def loop():
    global alarm_flag
    s_time = rtc.get_rtc_time()
    print("{}/{}/{} ({}) {}:{}:{}".format(s_time.year, s_time.month, s_time.day, s_time.week.decode("utf-8"), s_time.hour, s_time.minute, s_time.second))
    if alarm_flag == 1:
        rtc.clear_alarm()
        alarm_flag = 0
        print("Alarm clock is triggered.")
    time.sleep(0.9)


if __name__ == "__main__":
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        GPIO.cleanup()
        exit()
