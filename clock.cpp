// File: clock.cpp
// Created by: Clay Jackson
// Project Co-Creator: Carter Hurd
// Created on: 04/10/2018
// Synopsis: 
//   A prototype project a part of the ME 5194, Smart Products, class.
//   Goal: Create a marble machine that tracks the current hour using
//   the DS3231 RTC module.
//   Please see consult additional project files for visuals.

//#include "I2C_Slave.h"
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
  //int i2c1_fd = open(I2C1_PATH, O_RDWR);
  unsigned char rtcAddress = 0x68;    // Address of RTC
  unsigned char secondRegister = 0x00;  // Address of seconds register
  unsigned char minuteRegister = 0x01;  // Address of minutes register
  unsigned char hourRegister = 0x02;    // Address of hours register
  unsigned char dayRegister = 0x03;     // Address of day register
  unsigned char dateRegister = 0x04;    // Address of date register
  unsigned char monthRegister = 0x05;   // Address of month register
  unsigned char yearRegister = 0x06;    // Address of year register

  unsigned char secondsChar = 0x35;
  unsigned char minutesChar = 0x0;
  unsigned char hoursChar = 0x0;
  int seconds;
  int minutes;
  int hours;

  unsigned char secondsTest;
  unsigned char minutesTest;
  int secondsTestInt;
  int minutesTestInt;

  //I2C_Slave rtcModule(rtcAddress, i2c1_fd);

  //rtcModule.i2cBegin();

  //secondsChar = rtcModule.i2cRead8(secondRegister); 
  //minutesChar = rtcModule.i2cRead8(minuteRegister);
  //hoursChar = rtcModule.i2cRead8(hourRegister);

  seconds = convertSeconds(secondsChar); 
  minutes = convertMinutes(minutesChar);
  hours = convertHours(hoursChar);

  cout << "Seconds: " << seconds << endl;
  cout << "Minutes: " << minutes << endl;
  cout << "Hours: " << hours << endl;

  bitset<8> y(hoursChar);
  cout << "Hours Char: " << y << endl;

  displayTime(seconds,minutes,hours);

  secondsTest = writeSeconds(64);

  secondsTestInt = convertSeconds(secondsTest); 

  cout << "Seconds: " << secondsTestInt << endl;

  minutesTest = writeMinutes(44);

  minutesTestInt = convertMinutes(minutesTest);

  cout << "Minutes: " << minutesTestInt << endl;

  return 0;
}

int convertSeconds(unsigned char seconds) {
  // Converts the seconds register to a single seconds integer.
  int secondsInt;
  unsigned char secondsOne;
  unsigned char secondsTen;
  unsigned char onesMask;
  unsigned char tensMask;

  bitset<8> z(seconds);
  cout << "seconds Input: " << z << endl;

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

  bitset<8> z(minutes);

  cout << "minutes Input: " << z << endl;

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
  unsigned char amMask;
  unsigned char formatMask;
  unsigned char hoursOne;
  unsigned char hoursTen;
  unsigned char format;
  unsigned char amPm;

  onesMask = 0xF;
  formatMask = 0x40;

  hoursOne = onesMask & hours;
  format = (formatMask & hours) >> 6;

  if (format == 0x0) {
    // 24 hour format.

    tensMask = 0x30;

    hoursTen = (tensMask & hours) >> 4;

    format12 = false;
  }

  else {
    // 12 hour format.

    tensMask = 0x10;
    amMask = 0x20;

    hoursTen = (tensMask & hours) >> 4;
    amPm = (amMask & hours) >> 5;

    format12 = true;

    if (amPm == 0x1) { 
      hoursModifier = 12;
    }

  }

  hoursInt = int(hoursOne) + 10 * int(hoursTen) + hoursModifier;


  if (hoursInt >= 24 && !format12) {
    cerr << "ERROR! HOURS IS GREATER THAN 23!" << endl;
    exit(12);
  }

  if (hoursInt >= 13 && format12) {
    cerr << "ERROR! HOURS IS GREATER THAN 13 IN 12 HOUR FORMAT!" << endl;
    exit(13);
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

  cout << "Seconds ten: " << secondsTen << endl;
  cout << "Seconds one: " << secondsOne << endl;

  secondsChar = secondsTen;
  secondsChar = secondsChar << 4;

  bitset<8> y(secondsChar);
  cout << "Seconds ten: " << y << endl;

  mask = secondsOne;
  secondsChar = secondsChar | mask;

  bitset<8> y_t(secondsChar);
  cout << "Seconds Char: " << y_t << endl;

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

  cout << "Minutes ten: " << minutesTen << endl;
  cout << "Minutes one: " << minutesOne << endl;

  minutesChar = minutesTen;
  minutesChar = minutesChar << 4;

  bitset<8> y(minutesChar);
  cout << "Minutes ten: " << y << endl;

  mask = minutesOne;
  minutesChar = minutesChar | mask;

  bitset<8> y_m(minutesChar);
  cout << "Minutes Char: " << y_m << endl;

  return minutesChar;
}
