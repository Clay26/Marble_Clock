/*********************************************************************************************************
**********************************************************************************************************
							 DCMotor.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/25/2018
	@version	1.0.0
**********************************************************************************************************/
#include "GPIO.h"
#include "SPI_Slave.h"
#include "DCMotor.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <chrono>
#include <ctime>
#include <iomanip>

int DCMotor::setupDCMotor(PLATE_ADDR addr, DC_MOTOR mtr_in, DC_MOTOR_DIR mtr_dir, float start_speed, float accel, int A_Encoder_Pin, int B_Encoder_Pin)
{
	this->pa = addr;
	this->mtr = mtr_in;
	this->dir = mtr_dir;
	this->pin_enA = A_Encoder_Pin;
	this->pin_enB = B_Encoder_Pin;
	MotorPlate::setup();
	MotorPlate::configDC( addr, mtr_in,  mtr_dir, start_speed, accel);
	MotorPlate::GPIO::pinMode(A_Encoder_Pin, INPUT);
	MotorPlate::GPIO::pinMode(B_Encoder_Pin, INPUT);
	pFile = fopen(file_name.c_str(),"w");
	fprintf(pFile,"Time (s), Ref (deg/s), Ctrl (deg/s), Error (deg/s), Output (deg/s) \n");
	return 0;
}
int DCMotor::setupController(float Kp, float Ki, float Kd, float Ts)
{
	this->K1= Kp + Ki*Ts/2.0 + Kd/Ts;
	this->K2 = Ki*Ts/2.0-Kp-2.0*Kd/Ts;
	this->K3 = Kd/Ts;
	this->Ts = Ts;
	return 0;
}
int DCMotor::closeLogger()
{
	fclose(pFile);
	return 0;
}

float DCMotor::time()
{
	std::chrono::steady_clock::time_point time_now = std::chrono::steady_clock::now();
	std::chrono::duration<float, std::micro> time_span = time_now-this->start_time ;
	float duration =(1.0/1000000.0)* ((float)(time_span.count()));
	return duration;
}

int DCMotor::sampleHold(int delay, timeScale ts)
{
	MotorPlate::delay(delay, ts);
	return 0;
}

int DCMotor::startDCMotor()
{
	MotorPlate::startDC(this->pa, this->mtr);
	this->start_time = std::chrono::steady_clock::now();
	return 0;
}

int DCMotor::stopDCMotor()
{
	MotorPlate::stopDC(this->pa, this->mtr);
	this->end_time = std::chrono::steady_clock::now();
	return 0;
}

int DCMotor::updateSpeed(float speed)
{
	float duty_cycle = (speed/((float)MAX_DPS))*100.0;
	MotorPlate::putDCSpeed(this->pa, this->mtr, duty_cycle);
	return 0;
}

int DCMotor::logSignals(float time, float ref, float ctrl, float err, float actual)
{
	fprintf(pFile,"%f, %f, %f, %f, %f \n",time, ref, ctrl, err, actual);
	return 0;
}
int DCMotor::update_error_hist(float err)
{
	this->error_sig_km2 = this->error_sig_km1;// error for the k-2 sample
	this->error_sig_km1 =this->error_sig_km0 ;// error for the k-1 sample
	this->error_sig_km0 = err; // error for the k-0 sample
	return 0;
}

int DCMotor::update_control_hist(float ctrl)
{
	this->ctrl_sig_km1 = this->ctrl_sig_km0;// control for the k-1 sample
	this->ctrl_sig_km0 = ctrl;// control for the k-0 sample
	return 0;
}

float DCMotor::saturation(float speed)
{
	if(speed > ((float)MAX_DPS)) {speed = ((float)MAX_DPS-10);}
	if(speed<0) {speed =0;}
	return speed;
}

float DCMotor::reference(float time)
{
// Fill in Code Here
    float maxSpeed = 500;
    float slope = maxSpeed/10.0;
    float speed(0);
    
    if (time >= 0 && time < 10) {
        speed = slope * time;
    }
    
    else if  (time >= 10 && time <= 20) {
        speed = maxSpeed;
    }
    
    else {
        speed = maxSpeed - slope * (time - 20);
    }
    
	return speed;
}

float DCMotor::readSpeed()
{
// Fill In Code HERE

    float timeZero = this->time();
    float waitTime = 0.5;
    float EPSILON = 0.0000001;
    int currentStateA = GPIO::digitalRead(pin_enA);
    int currentStateB = GPIO::digitalRead(pin_enB);
    int readStateA = currentStateA;
    int readStateB = currentStateB;
    float timeOne;
    float timeDur(0);
    float totalTime(0);
    float speed;

    while (currentStateA == readStateA && currentStateB == readStateB && timeDur < waitTime) {
        readStateA = GPIO::digitalRead(pin_enA);
        readStateB = GPIO::digitalRead(pin_enB);
        timeOne = this->time();
        timeDur = timeOne - timeZero;
    }

    float timeStart = this->time();
    currentStateA = readStateA;
    currentStateB = readStateB;
    timeDur = 0;
    timeZero = this->time();

    while (currentStateA == readStateA && currentStateB == readStateB && timeDur < waitTime) {
        readStateA = GPIO::digitalRead(pin_enA);
        readStateB = GPIO::digitalRead(pin_enB);
        timeOne = this->time();
        timeDur = timeOne - timeZero;
    }

    float timeEnd = this->time();
    totalTime = timeEnd - timeStart;

    if ( (totalTime - 0) < EPSILON) {
      cout << "ERROR reading speed" << endl;
      cout << "SETTING SPEED TO ZERO!" << endl;
      speed = 0;
    }

    else {
       speed = 360.0 * (1.0 / (1120.0) ) / totalTime;
    }

	return speed;
}

int DCMotor::controlSpeed()
{
//Fill Code In Here
    float measuredSpeed = this->readSpeed();
    float timeZero = this->time();
    float desiredSpeed = this->reference(timeZero);
    float speedError(0);
    float calculatedSpeed(0);
    
    speedError = desiredSpeed - measuredSpeed;
    cout << "Desired Speed: " << desiredSpeed << endl;
    cout << "Measured Speed: " << measuredSpeed << endl;

    this->update_error_hist(speedError);
    ctrl_sig_km0 = K1 * error_sig_km0 + K2 * error_sig_km1 + K3 * error_sig_km2 + ctrl_sig_km1;
    calculatedSpeed = this->saturation(ctrl_sig_km0);
    calculatedSpeed = this->saturation(desiredSpeed); // FIXME
    
    this->updateSpeed(calculatedSpeed);
    cout << "Calculated Speed: " << calculatedSpeed << endl;
    this->logSignals(timeZero, desiredSpeed, calculatedSpeed, speedError, measuredSpeed);
    
	return 0;
}

