# -*- coding:utf-8 -*-
'''!
  @file ramReadAndWrite.py
  @brief Run this routine to read and write RAM data in the RTC module
  @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license      The MIT License (MIT)
  @author [thdyyl](yuanlong.yu@dfrobot.com)
  @version V1.0.0
  @date 2024-07-10
  @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
import os
import sys
import time
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
    # The address where the operation is performed.
    op_addr = 0x2D
    # Initialize module.
    while not rtc.begin():
        print("Failed to init chip, please check if the chip connection is fine. ")
        time.sleep(1)
    # Write data to RAM and then read it out.
    rtc.write_sram(op_addr, 50)
    time.sleep(1)
    data = rtc.read_sram(op_addr)
    print("The data read to address {:02X} is: {}".format(op_addr, data))
    time.sleep(0.1)

    # After erasing the data from RAM, read it out.
    rtc.clear_sram(op_addr)
    print("Erasing address {:02X} data completed.".format(op_addr))
    time.sleep(0.1)
    data = rtc.read_sram(op_addr)
    print("The data read to address {:02X} is: {}".format(op_addr, data))


if __name__ == "__main__":
    try:
        setup()
    except KeyboardInterrupt:
        exit()
