/*!
 * @file interrupt.ino
 * @brief Run this routine, set internal clock and interrupt trigger first, interrupt will be triggered when the set time is reached
 * @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license      The MIT License (MIT)
 * @author [qsjhyy](yihuan.huang@dfrobot.com)
 * @version V1.0
 * @date 2022-08-30
 * @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
 */
#include "DFRobot_GNSSAndRTC.h"

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked

#ifdef  I2C_COMMUNICATION
DFRobot_GNSSAndRTC_I2C rtc(&Wire, MODULE_I2C_ADDRESS);
#else
 /* ---------------------------------------------------------------------------------------------------------------------
  *    board   |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |   m0  |
  *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |  vcc  |
  *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |  gnd  |
  *     RX     |              TX                |     Serial1 TX1      |     5     |   5/D6  |  D2   |     X      |  tx1  |
  *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D7  |  D3   |     X      |  rx1  |
  * ----------------------------------------------------------------------------------------------------------------------*/
  /* Baud rate cannot be changed  */
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
SoftwareSerial mySerial(4, 5);
DFRobot_GNSSAndRTC_UART rtc(&mySerial, UART_BAUDRATE);
#elif defined(ESP32)
DFRobot_GNSSAndRTC_UART rtc(&Serial1, UART_BAUDRATE,/*rx*/D2,/*tx*/D3);
#else
DFRobot_GNSSAndRTC_UART rtc(&Serial1, UART_BAUDRATE);
#endif
#endif

volatile  int8_t alarmFlag = 0;
#define PIN 2

void setup()
{
  Serial.begin(115200);
  /*Wait for the chip to be initialized completely, and then exit*/
  while (!rtc.begin()) {
    Serial.println("Failed to init chip, please check if the chip connection is fine. ");
    delay(1000);
  }
  rtc.setHourSystem(rtc.e24hours);//Set display format
  rtc.setTime(2022, 7, 27, 23, 59, 55);//Set default time
  //Countdown timer, schedule alarm and daily regular alarm can't be used at the same time
  //rtc.countDown(3);//Countdown
  //rtc.setAlarm(2022,7,28);//Set schedule alarm
  rtc.setAlarm(rtc.eEveryDay, 0, 0, 0);//Set daily regular alarm

#if defined(ESP32)||defined(ESP8266)
  attachInterrupt(digitalPinToInterrupt(D7)/*Query the interrupt number of the D6 pin*/, interrupt, FALLING);
#elif defined(ARDUINO_SAM_ZERO)
  // Pin 5 is used as interrupt pin by default, other non-conflicting pins can also be selected as external interrupt pins
  attachInterrupt(digitalPinToInterrupt(5)/* Query the interrupt number of the 5 pin */, interrupt, FALLING);
#else
  /*    The Correspondence Table of AVR Series Arduino Interrupt Pins And Terminal Numbers
  * ---------------------------------------------------------------------------------------
  * |                                        |  DigitalPin  | 2  | 3  |                   |
  * |    Uno, Nano, Mini, other 328-based    |--------------------------------------------|
  * |                                        | Interrupt No | 0  | 1  |                   |
  * |-------------------------------------------------------------------------------------|
  * |                                        |    Pin       | 2  | 3  | 21 | 20 | 19 | 18 |
  * |               Mega2560                 |--------------------------------------------|
  * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  | 5  |
  * |-------------------------------------------------------------------------------------|
  * |                                        |    Pin       | 3  | 2  | 0  | 1  | 7  |    |
  * |    Leonardo, other 32u4-based          |--------------------------------------------|
  * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  |    |
  * |--------------------------------------------------------------------------------------
  */
  /*                      The Correspondence Table of micro:bit Interrupt Pins And Terminal Numbers
  * ---------------------------------------------------------------------------------------------------------------------------------------------
  * |             micro:bit                       | DigitalPin |P0-P20 can be used as an external interrupt                                     |
  * |  (When using as an external interrupt,      |---------------------------------------------------------------------------------------------|
  * |no need to set it to input mode with pinMode)|Interrupt No|Interrupt number is a pin digital value, such as P0 interrupt number 0, P1 is 1 |
  * |-------------------------------------------------------------------------------------------------------------------------------------------|
  */
  attachInterrupt(digitalPinToInterrupt(2), interrupt, FALLING);
#endif
}

void loop()
{
  DFRobot_GNSSAndRTC::sTimeData_t sTime;
  sTime = rtc.getRTCTime();
  Serial.print(sTime.year, DEC);//year
  Serial.print('/');
  Serial.print(sTime.month, DEC);//month
  Serial.print('/');
  Serial.print(sTime.day, DEC);//day
  Serial.print(" (");
  Serial.print(sTime.week);//week
  Serial.print(") ");
  Serial.print(sTime.hour, DEC);//hour
  Serial.print(':');
  Serial.print(sTime.minute, DEC);//minute
  Serial.print(':');
  Serial.print(sTime.second, DEC);//second
  Serial.println(' ');
  /*Enable 12-hour time format*/
  // Serial.print(rtc.getAMorPM());
  // Serial.println();
  if (alarmFlag == 1) {
    rtc.clearAlarm();
    alarmFlag = 0;
    Serial.println("Alarm clock is triggered.");

  }
  delay(1000);
}

void interrupt(void)
{
  alarmFlag = 1;
}
