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
#include <wiringPi.h>

using namespace std;

int convertSeconds(unsigned char seconds);
int convertMinutes(unsigned char minutes);
int convertHours(unsigned char hours);
void writeTime(int seconds, int minutes, int hours, bool format12 = false);
unsigned char writeSeconds(int seconds);
unsigned char writeMinutes(int minutes);
unsigned char writeHours(int hours, bool format12 = false);
void displayTime(int seconds, int minutes, int hours, bool format12 = true);

int main() { int i2c1_fd = open(I2C1_PATH, O_RDWR);
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
  unsigned char secondsChar;            // Contents of second register
  unsigned char minutesChar;            // Contents of minute register
  unsigned char hoursChar;              // Contents of hour register
  int seconds;                          // Number of seconds on RTC module
  int minutes;                          // Number of minutes on RTC module
  int hours;                            // Number of hours on RTC module
  int changeTime(1);                    // Whether or not user want to change
  // time
  int currentHour;                      // Current hour used in while loop
  int pwmSuccess(1);                    // Whether the PWM pin was initialized 
  // correctly
  int quit(0);                          // Whether the user wants to quit clock
  const int SERVO_PIN = 18;             // GPIO pin number servo is connected to
  // (Has to be on PWM capable pin)
  const int OPEN_GATE = 100;            // Open gate position for servo
  const int CLOSE_GATE = 50;            // Close gate position for servo

  // RTC module initialization on I2C bus.
  I2C_Slave rtcModule(rtcAddress, i2c1_fd);

  rtcModule.i2cBegin();

  // Motor for wheel set up on SPI bus.
  DCMotor wheelMotor(spi1_fd, fn);

  wheelMotor.setupDCMotor(A0, DC_1, CCW_DC, 0, 0, 26, 13);
  wheelMotor.setupController(100,1,1,0.01);
  wheelMotor.startDCMotor();
  wheelMotor.sampleHold(1, S);

  // Servo motor for gate set up on PWM pin.
  // Returns 0 is set up was a success
  pwmSuccess = wiringPiSetupGpio();

  if (pwmSuccess != 0) {
    cerr << "ERROR! WiringPi not setup!" << endl;
    exit(101);
  }

  // Set pwm to mark space mode
  pwmSetMode(PWM_MODE_MS);

  pwmSetRange(1000);

  pwmSetClock(384);

  // Servo is set up.

  // Read the current time on RTC module.
  secondsChar = rtcModule.i2cRead8(secondRegister); 
  minutesChar = rtcModule.i2cRead8(minuteRegister);
  hoursChar = rtcModule.i2cRead8(hourRegister);

  seconds = convertSeconds(secondsChar); 
  minutes = convertMinutes(minutesChar);
  hours = convertHours(hoursChar);

  // Display time on RTC module.
  // Asks if user wants to update to a different time.
  displayTime(seconds,minutes,hours,true);

  // User enters a 1 to change the time or a 0 to keep the current time.
  cout << "Would like to change the time?" << endl;  
  cin >> changeTime;

  if (changeTime == 1) {
    // User enters the upadated time.
    // Enter time with spaces in between each time component.
    // Enter hours in 24 hour format.
    cout << "Time (hours, minutes, seconds): ";
    cin >> hours;
    cin >> minutes;
    cin >> seconds;

    secondsChar = writeSeconds(seconds);
    minutesChar = writeMinutes(minutes);
    hoursChar = writeHours(hours);

    rtcModule.i2cWrite8(secondRegister, secondsChar);
    rtcModule.i2cWrite8(minuteRegister, minutesChar);
    rtcModule.i2cWrite8(hourRegister, hoursChar);
  }

  // Everything had been set up.

  // Set servo motor to close gate position.
  pwmWrite(SERVO_PIN, CLOSE_GATE);

  // Start main wheel to start carrying marbles.
  wheelMotor.updateSpeed(500);

  // Enter infinite while loop for clock.
  while(1) {
    currentHour = hours;

    // Check if the hour has changed.
    while (currentHour == hours) {
      hoursChar = rtcModule.i2cRead8(hourRegister);

      hours = convertHours(hoursChar);
    }

    // Once hour has changed, open servo gate to let one marble on to hour
    // track.
    pwmWrite(SERVO_PIN, OPEN_GATE);
    usleep(1000000);

    // Fetch time.
    secondsChar = rtcModule.i2cRead8(secondRegister); 
    minutesChar = rtcModule.i2cRead8(minuteRegister);

    seconds = convertSeconds(secondsChar); 
    minutes = convertMinutes(minutesChar);

    // Display new hour change to user.
    displayTime(seconds,minutes,hours);

    pwmWrite(SERVO_PIN, CLOSE_GATE);
    usleep(1000000);
  }

  // Turn off main wheel motor.
  wheelMotor.updateSpeed(0);
  
  // Turn off main wheel motor.
  pwmWrite(SERVO_PIN, 0);

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

  else if (hoursTens == 0x10) {
    hoursTens = 0x1;
  }

  else {
    hoursTens = 0x0;
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

  if (hoursTen > 1) {
    hoursPMChar = 0x30;

    hoursChar = hoursChar | hoursPMChar;
  }

  else if (hoursTen == 1) {
    hoursTenChar = 0x10;
    hoursChar = hoursChar | hoursTenChar;
  }

  hoursOneChar = hoursOne;

  hoursChar = hoursChar | hoursOneChar;

  return hoursChar;
}
