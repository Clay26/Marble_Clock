// File: clock.cpp
// Created by: Clay Jackson
// Project Co-Creator: Carter Hurd
// Created on: 04/10/2018
// Synopsis: 
//   A prototype project a part of the ME 5194, Smart Products, class.
//   Goal: Create a marble machine that tracks the current hour using
//   the DS3231 RTC module.
//   Please see consult additional project files for visuals.

#include "I2C_Slave.h"
#include "MotorPlate.h"
#include "SPI_Slave.h"
#include "GPIO.h"
#include "DCMotor.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <bitset>
#include <cstdlib>

using namespace std;

int convertSeconds(unsigned char seconds);
int convertMinutes(unsigned char minutes);
int convertHours(unsigned char hours);
void writeTime(int seconds, int minutes, int hours, bool format12 = false);
unsigned char writeSeconds(int seconds);
unsigned char writeMinutes(int minutes);
unsigned char writeHours(int hours, bool format12 = false);
void displayTime(int seconds, int minutes, int hours, bool format12 = true);

int main() {
  int i2c1_fd = open(I2C1_PATH, O_RDWR);
  int spi1_fd = open(SPI_DEV, O_RDWR);
  string fn = "Signal_History.txt";
  unsigned char rtcAddress = 0x68;    // Address of RTC
  unsigned char secondRegister = 0x00;  // Address of seconds register
  unsigned char minuteRegister = 0x01;  // Address of minutes register
  unsigned char hourRegister = 0x02;    // Address of hours register
  unsigned char dayRegister = 0x03;     // Address of day register
  unsigned char dateRegister = 0x04;    // Address of date register
  unsigned char monthRegister = 0x05;   // Address of month register
  unsigned char yearRegister = 0x06;    // Address of year register
  unsigned char secondsChar;
  unsigned char minutesChar;
  unsigned char hoursChar;
  int seconds;
  int minutes;
  int hours;
  int changeTime(1);


  I2C_Slave rtcModule(rtcAddress, i2c1_fd);
  DCMotor wheelMotor(spi1_fd, fn);

  rtcModule.i2cBegin();

  wheelMotor.setupDCMotor(A0, DC_1, CCW_DC, 0, 0, 26, 13);
  wheelMotor.setupController(100,1,1,0.01);
  wheelMotor.startDCMotor();
  wheelMotor.sampleHold(1, S);

  secondsChar = rtcModule.i2cRead8(secondRegister); 
  minutesChar = rtcModule.i2cRead8(minuteRegister);
  hoursChar = rtcModule.i2cRead8(hourRegister);

  seconds = convertSeconds(secondsChar); 
  minutes = convertMinutes(minutesChar);
  hours = convertHours(hoursChar);

  displayTime(seconds,minutes,hours,true);

  cout << "Would like to change the time?" << endl;
  cin >> changeTime;

  if (changeTime == 1) {
    cout << "Time (hours, minutes, seconds): ";
    cin >> hours;
    cin >> minutes;
    cin >> seconds;

    secondsChar = writeSeconds(seconds);
    minutesChar = writeMinutes(minutes);
    hoursChar = writeHours(hours);

    bitset<8> s(secondsChar);
    bitset<8> m(minutesChar);
    bitset<8> h(hoursChar);

    cout << "Time (hours, minutes, seconds): " << endl;
    cout << h << endl;
    cout << m << endl;
    cout << s << endl;

    rtcModule.i2cWrite8(secondRegister, secondsChar);
    rtcModule.i2cWrite8(minuteRegister, minutesChar);
    rtcModule.i2cWrite8(hourRegister, hoursChar);
  }

  secondsChar = rtcModule.i2cRead8(secondRegister); 
  minutesChar = rtcModule.i2cRead8(minuteRegister);
  hoursChar = rtcModule.i2cRead8(hourRegister);

  seconds = convertSeconds(secondsChar); 
  minutes = convertMinutes(minutesChar);
  hours = convertHours(hoursChar);

  displayTime(seconds,minutes,hours,false); 

  wheelMotor.stopDCMotor();
  wheelMotor.closeLogger();

  return 0;
}

int convertSeconds(unsigned char seconds) {
  // Converts the seconds register to a single seconds integer.
  int secondsInt;
  unsigned char secondsOne;
  unsigned char secondsTen;
  unsigned char onesMask;
  unsigned char tensMask;

  onesMask = 0xF;
  tensMask = 0xF0;

  secondsOne = onesMask & seconds;
  secondsTen = (tensMask & seconds) >> 4;

  secondsInt = int(secondsOne) + 10 * int(secondsTen);

  if (secondsInt >= 60) {
    cerr << "ERROR! SECONDS GREATER THAN 59!" << endl;
    exit(10);
  }

  return secondsInt;
}

int convertMinutes(unsigned char minutes) {
  // Converts the minutes register to an int.
  int minutesInt;
  unsigned char minutesOne;
  unsigned char minutesTen;
  unsigned char onesMask;
  unsigned char tensMask;

  onesMask = 0xF;
  tensMask = 0xF0;

  minutesOne = onesMask & minutes;
  minutesTen = (tensMask & minutes) >> 4;

  minutesInt = int(minutesOne) + 10 * int(minutesTen);

  if (minutesInt >= 60) {
    cerr << "ERROR! MINUTES GREATER THAN 59!" << endl;
    exit(11);
  }

  return minutesInt;
}

int convertHours(unsigned char hours) {
  // Converts hour register to 24-hour format.
  int hoursInt;
  int hoursModifier(0);
  bool format12;
  unsigned char onesMask;
  unsigned char tensMask;
  unsigned char hoursTens;
  unsigned char hoursOnes;

  onesMask = 0xF;
  tensMask = 0x30;

  hoursOnes = hours & onesMask;
  hoursTens = hours & tensMask;

  if (hoursTens == 0x30) {
    hoursTens = 0x2;
  }

  else {
    hoursTens = 0x1;
  }

  hoursInt = int(hoursOnes) + 10 * int(hoursTens) + hoursModifier;


  if (hoursInt >= 24) {
    cerr << "ERROR! HOURS IS GREATER THAN 23!" << endl;
    exit(12);
  }

  return hoursInt;
}

void displayTime(int seconds, int minutes, int hours, bool format12) {
  // Display time in 12 or 24 hour format depending on format12. 

  if (format12) {
    cout << hours % 12 << ":" << minutes << ":"
      << seconds << " ";

    if (hours > 12) {
      cout << "PM" << endl;
    }

    else {
      cout << "AM" << endl;
    }

  }

  else {
    cout << hours << ":" << minutes << ":"
      << seconds << endl;
  }

  return;
}

unsigned char writeSeconds(int seconds) {
  // Converts the int seconds to an unsigned char to pass into register
  unsigned char secondsChar;
  unsigned char mask;
  int secondsOne;
  int secondsTen;

  if (seconds >= 60) {
    cerr << "ERROR! SECONDS GREATER THAN 60!" << endl;
    exit(20);
  }

  mask = 0x0;

  secondsTen = seconds / 10;
  secondsOne = seconds - 10 * secondsTen;

  secondsChar = secondsTen;
  secondsChar = secondsChar << 4;

  mask = secondsOne;
  secondsChar = secondsChar | mask;

  return secondsChar;
}

unsigned char writeMinutes(int minutes) {
  // Converts the int minutes to an unsigned char to pass into register
  unsigned char minutesChar;
  unsigned char mask;
  int minutesOne;
  int minutesTen;

  if (minutes >= 60) {
    cerr << "ERROR! MINUTES GREATER THAN 60!" << endl;
    exit(21);
  }

  mask = 0x0;

  minutesTen = minutes / 10;
  minutesOne = minutes - 10 * minutesTen;

  minutesChar = minutesTen;
  minutesChar = minutesChar << 4;

  mask = minutesOne;
  minutesChar = minutesChar | mask;

  return minutesChar;
}

unsigned char writeHours(int hours, bool format12) {
  // Converts int hours to unsigned char for register placement.
  // Conversion is based whether it is for 12 or 24 hour format.
  unsigned char hoursChar(0x0);
  unsigned char hoursPMChar(0x0);
  unsigned char hoursTenChar(0x0);
  unsigned char hoursOneChar(0x0);
  int hoursTen;
  int hoursOne;

  if (format12) {
    hoursChar = 0x1;

    hoursChar = hoursChar << 6;
  }

  hoursTen = hours / 10;
  hoursOne = hours - 10 * hoursTen;

  cout << "hoursOne: " << hoursOne << endl;

  if (hoursTen > 1) {
    hoursPMChar = 0x30;

    hoursChar = hoursChar | hoursPMChar;
  }

  else if (hoursTen == 1) {
    hoursTenChar = 0x10;
    hoursChar = hoursChar | hoursTenChar;
  }

  hoursOneChar = hoursOne;

  bitset<8> hours1(hoursOneChar);
  cout << "hoursOneChar: " << hours1 << endl;

  hoursChar = hoursChar | hoursOneChar;

  bitset<8> hours2(hoursChar);
  cout << "hoursChar: " << hours2 << endl;

  return hoursChar;
}
