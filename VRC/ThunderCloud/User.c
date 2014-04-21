#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in2,    poten,          sensorPotentiometer)
#pragma config(Sensor, in3,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  launSole1,      sensorDigitalOut)
#pragma config(Sensor, dgtl2,  wingSole1,      sensorDigitalOut)
#pragma config(Sensor, dgtl11, wingSole2,      sensorDigitalOut)
#pragma config(Sensor, dgtl12, launSole2,      sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           intakeRight,   tmotorVex393, openLoop)
#pragma config(Motor,  port2,           armRight,      tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port3,           armRight2,     tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port4,           backRight,     tmotorVex393, openLoop, encoder, encoderPort, I2C_1, 1233528)
#pragma config(Motor,  port5,           backLeft,      tmotorVex393, openLoop, encoder, encoderPort, I2C_2, 1233528)
#pragma config(Motor,  port6,           armLeft,       tmotorVex393, openLoop)
#pragma config(Motor,  port7,           frontRight,    tmotorVex393, openLoop)
#pragma config(Motor,  port8,           armLeft2,      tmotorVex393, openLoop)
#pragma config(Motor,  port9,           frontLeft,     tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port10,          intakeLeft,    tmotorVex393, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Note armRight and arLeft are the top motors
//orientation of left and right is from the front of the robot
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(125)

#include "Main.c"
#include "PIDController.c"
#include "Sound.c"

PIDController armPID;
PIDController turnPID;
PIDController movePID;

const int ARMAX = 3800;
const int ARMIN = 2000;

void robotShutdown() {
	motor[port1] = 0;
	motor[port2] = 0;
	motor[port3] = 0;
	motor[port4] = 0;
	motor[port5] = 0;
	motor[port6] = 0;
	motor[port7] = 0;
	motor[port8] = 0;
	motor[port9] = 0;
	motor[port10] = 0;
	SensorValue[dgtl1] = 1;
	SensorValue[dgtl12] = 1;
}
void driveArcade(int y, int x) {
	motor[frontLeft] = motor[backLeft] = y + x;
	motor[frontRight] = motor[backRight] = y - x;
}
void setArmSpeed(int z) {
	motor[armRight] = motor[armLeft] = motor[armLeft2] = motor[armRight2] = z;
}
void setIntakeSpeed(int speed) {
	motor[intakeLeft] = motor[intakeRight] = speed;
}
void resetEncoders() {
	nMotorEncoder[backRight] = nMotorEncoder[backLeft] = 0;
}

///////////////////////////////////////////////////////////
// Robot Tasks
///////////////////////////////////////////////////////////

void robotInit() {
	robotShutdown();

	// Initialize PID
	init(armPID, 0.564444, 0, 0);
	setThresholds(armPID, 128, -127);
	init(turnPID, 0.267368, 0, 0);
	setThresholds(turnPID, 128, -127);

	// Initalize Gyroscope
	SensorType[gyro] = sensorNone;
	wait1Msec(1000);
	SensorType[gyro] = sensorGyro;
	wait1Msec(2000);
	SensorValue[gyro]=0;
	SensorFullCount[gyro]=3600;
}

task robotAutonomous() {
	if (lcd_autonMode == lcd_autonModeMID) {
		setIntakeSpeed(127);				//drop buckie ball

		setArmSpeed(100);
		wait1Msec(1000);
		setArmSpeed(5);

		driveArcade(70,0);					//drive forward
		wait1Msec(1100);
		driveArcade(0,0);

		setIntakeSpeed(0);
		setArmSpeed(20);

		wait1Msec(1000);

		driveArcade(-70,0);				//drive back
		wait1Msec(1200);
		driveArcade(0,0);

		wait1Msec(5000);

		driveArcade(70,0);					//drive forward
		wait1Msec(1300);
		driveArcade(0,0);

		wait1Msec(1000);

		driveArcade(-70,0);				//drive back
		wait1Msec(1300);
		driveArcade(0,0);


	} else if (lcd_autonMode == lcd_autonModeHANG) {


	}

	robotShutdown();
}

bool pidOverride = false;
task robotTeleop() {
	// User control initialization
	setSetpoint(turnPID, 0);
	turnPID.enabled = true;
	while (true) { // User control loop
	  	int driveX = vexRT[Ch4];
	  	int driveY = vexRT[Ch3];
	  	int armSpeed = vexRT[Ch2];
	  	int intakeSpeed = 127*((vexRT[Btn5U]|vexRT[Btn5D])-(vexRT[Btn6U]|vexRT[Btn6D]));

	  	if (vexRT[Btn8U]) {
	  		pidOverride = true;
	  	}
	  	if (vexRT[Btn7U]) {
	  		pidOverride = false;
	  	}


	  	if (abs(driveX) < 16) {
				turnPID.enabled = true;
			} else {
				turnPID.enabled = false;
				SensorValue[gyro] = 0;
			}
			if (abs(armSpeed) > 8) { //////////////////////////////////////////
				armPID.enabled = false;
				setSetpoint(armPID, SensorValue[poten]);
			} else {
				armPID.enabled = true;
			}
			if (!pidOverride) {
	  		driveX = turnPID.enabled ? calculate(turnPID, SensorValue[gyro]) : driveX;
				armSpeed = armPID.enabled ? calculate(armPID, SensorValue[poten]) : armSpeed;
			}

			if (abs(driveY) < 8) driveY = 0; // Drive deadband
			if (abs(driveX) < 8) driveX = 0; // Drive deadband

			if (VexRT[Btn8D]) {
				SensorValue[launSole1] = SensorValue[launSole2] = 1;
			} else {
				SensorValue[launSole1] = SensorValue[launSole2] = 0;
			}

	  	driveArcade(driveY, driveX);
	  	setArmSpeed(armSpeed);
	  	setIntakeSpeed(intakeSpeed);
	}
}

void robotDisabled() {

}
