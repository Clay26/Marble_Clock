#include <wiringPi.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int main() {
  int pwmSuccess(1);
  const int SERVO_PIN = 18;

  pwmSuccess = wiringPiSetupGpio();

  if (pwmSuccess != 0) {
    cerr << "ERROR! WiringPi not setup!" << endl;
    exit(101);
  }

  // Set pwm to mark space mode
  pwmSetMode(PWM_MODE_MS);

  pwmSetRange(1000);

  pwmSetClock(384);

  pwmWrite(SERVO_PIN, 0);

  return 0;
}
