# -*- coding:utf-8 -*-
'''!
  @file  getGNSS.py
  @brief Get gnss simple data
  @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V1.0.0
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''

import sys
import time
import os
import six
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../')))

#I2C_UART_FLAG = "I2C"
I2C_UART_FLAG = "UART"

if I2C_UART_FLAG == "I2C":
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_I2C
    gnss = DFRobot_GNSSAndRTC_I2C(1)
else:
    from DFRobot_GNSSAndRTC import DFRobot_GNSSAndRTC_UART
    gnss = DFRobot_GNSSAndRTC_UART("/dev/serial0")


def setup():
    while not gnss.begin():
        print("No Deivces!")
        time.sleep(1)
    gnss.enable_power()

    gnss.set_gnss(gnss.EGPS_BEIDOU_GLONASS)


def loop():
    utc = gnss.get_utc()
    date = gnss.get_date()
    lat = gnss.get_lat()
    lon = gnss.get_lon()

    high = gnss.get_alt()
    star_userd = gnss.get_num_sat_used()
    sog = gnss.get_sog()
    cog = gnss.get_cog()

    print("\n{}/{}/{}/{}:{}:{}\n".format(date.year, date.month, date.date, utc.hour, utc.minute, utc.second))
    if six.PY3:
        print(lat.latDirection.decode('utf-8'))
        print(lon.lonDirection.decode('utf-8'))
    else:
        print(lat.latDirection)
        print(lon.lonDirection)
    print("lat degree = {:.6f}".format(lat.latitudeDegree))
    print("lon degree = {:.6f}".format(lon.lonitudeDegree))

    print("star userd = {}".format(star_userd))
    print("alt high = {}".format(high))
    print("sog = {}".format(sog))
    print("cog = {}".format(cog))
    print("gnss mode = {}".format(gnss.get_gnss_mode()))
    time.sleep(1)


if __name__ == "__main__":
    try:
        setup()
        while True:
            loop()
    except KeyboardInterrupt:
        exit()
