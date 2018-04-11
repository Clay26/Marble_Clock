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
  unsigned char hoursTest;
  int secondsTestInt;
  int minutesTestInt;
  int hoursTestInt;

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

  secondsTest = writeSeconds(14);

  secondsTestInt = convertSeconds(secondsTest); 

  cout << "Seconds: " << secondsTestInt << endl;

  minutesTest = writeMinutes(44);

  minutesTestInt = convertMinutes(minutesTest);

  cout << "Minutes: " << minutesTestInt << endl;

  hoursTest = writeHours(21,false);

  bitset<8> test5(hoursTest);
  cout << "Hours Char: " << test5 << endl;

  hoursTestInt = convertHours(hoursTest);

  cout << "Hours: " << hoursTestInt << endl;

  displayTime(secondsTestInt,minutesTestInt,hoursTestInt, true);

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

unsigned char writeHours(int hours, bool format12) {
  // Converts int hours to unsigned char for register placement.
  // Conversion is based whether it is for 12 or 24 hour format.
  unsigned char hoursChar;
  unsigned char hoursPMChar;
  unsigned char hoursTenChar;
  unsigned char hoursOneChar;
  int hoursTen;
  int hoursOne;

  if (format12) {
    hoursChar = 0x1;

    bitset<8> test1(hoursChar);
    cout << "Should only be 1: " << test1 << endl;

    hoursChar = hoursChar << 6;

    bitset<8> test2(hoursChar);
    cout << "1 moved to 12 hour format: " << test2 << endl;
  }

  hoursTen = hours / 10;
  hoursOne = hours - 10 * hoursTen;

  if (hoursTen > 1) {
    hoursPMChar = 0x30;

    bitset<8> test3(hoursPMChar);
    cout << "1 moved to PM hour format: " << test3 << endl;

    hoursChar = hoursChar | hoursPMChar;

    bitset<8> test4(hoursChar);
    cout << "12 hours format and PM or 20 hour: " << test4 << endl;
  }

  else if (hoursTen == 1) {
    hoursTenChar = 0x10;
    hoursChar = hoursChar | hoursTenChar;

    bitset<8> test5(hoursChar);
    cout << "10 hour: " << test5 << endl;
  }

  hoursOneChar = hoursOne;
  hoursChar = hoursChar | hoursOneChar;

  bitset<8> test6(hoursChar);
  cout << "1 hour: " << test6 << endl;

  return hoursChar;
}
