#include <stdio.h>
#include <string>
#include <cmath>
#include <robot-config.h>
#include <vex.h>

using namespace vex;

competition Competition;

double pi = 3.1415926535897932384626433;
bool reversed = false;

double lateralkP = 0, lateralkI = 0, lateralkD = 0;
double turnkP = 0, turnkI = 0, turnkD = 0;
double lateralError, previousLateralError, turnError, previousTurnError;

double lateralErrorDifference, turnErrorDifference;

double totalLateralError, totalTurnError;

double desiredLateralValue;
double desiredTurnValue;

bool pidOn = true;

bool wingsOn = false;

void resetDriveSensors() {
  FrontLeft.resetPosition();
  FrontRight.resetPosition();
  MiddleLeft.resetPosition();
  MiddleRight.resetPosition();
  BackLeft.resetPosition();
  BackRight.resetPosition();
}

void pid() {
  while (true) {
    if (pidOn) {
      double leftMotorPosition = MiddleLeft.position(degrees);
      double rightMotorPosition = MiddleRight.position(degrees);
      double averageMotorPosition = (leftMotorPosition + rightMotorPosition) / 2;
      double turnDifference = leftMotorPosition - rightMotorPosition;

      lateralError = desiredLateralValue - averageMotorPosition;

      lateralErrorDifference = lateralError - previousLateralError;

      turnError = desiredTurnValue - turnErrorDifference;

      turnErrorDifference = turnError - previousTurnError;

      totalLateralError += lateralError;

      totalTurnError += turnError;

      double lateralMotorPower = lateralError * lateralkP + totalLateralError * lateralkI + lateralErrorDifference * lateralkD;

      double turnMotorPower =
          (turnError * turnkP + totalTurnError * turnkI + turnErrorDifference * turnkD);

      previousLateralError = lateralError;
      previousTurnError = turnError;
      
      wait(20, msec);

      FrontLeft.spin(forward, lateralMotorPower + turnMotorPower, volt);
      FrontRight.spin(forward, lateralMotorPower - turnMotorPower, volt);
      MiddleLeft.spin(forward, lateralMotorPower + turnMotorPower, volt);
      MiddleRight.spin(forward, lateralMotorPower - turnMotorPower, volt);
      BackLeft.spin(forward, lateralMotorPower + turnMotorPower, volt);
      BackRight.spin(forward, lateralMotorPower - turnMotorPower, volt);
    }
  }
}

void drive(double angle) {
  pidOn = true;
  resetDriveSensors();
  desiredLateralValue = angle;
  desiredTurnValue = 0;
  while ((MiddleLeft.position(degrees) - angle) > 2 && (MiddleRight.position(degrees) - angle) > 2) wait(20, msec);
}

void turn(double angle) {
  pidOn = true;
  resetDriveSensors();
  desiredLateralValue = 0;
  desiredTurnValue = angle;
  while ((MiddleLeft.position(degrees) - angle) > 2 && (MiddleRight.position(degrees) + angle) > 2) wait(20, msec);
}

void toggleWings() {
  wingsOn = !wingsOn;
  LeftWing.set(wingsOn);
  RightWing.set(wingsOn);
}

void preauton() {

}

void autonomous(void) {
  thread p(pid);
  wait(20, msec);
}

void usercontrol(void) {
  int axis1, axis3;
  while (1) {
    axis1 = Controller1.Axis1.position(percent);
    axis3 = Controller1.Axis3.position(percent);

    if (reversed) {
      FrontLeft.spin(forward, axis3 + axis1, percent);
      FrontRight.spin(forward, axis3 - axis1, percent);
      BackLeft.spin(forward, axis3 + axis1, percent);
      BackRight.spin(forward, axis3 - axis1, percent);
      MiddleLeft.spin(forward, axis3 + axis1, percent);
      MiddleRight.spin(forward, axis3 - axis1, percent);
    } else {
      FrontLeft.spin(reverse, axis3 - axis1, percent);
      FrontRight.spin(reverse, axis3 + axis3, percent);
      BackLeft.spin(reverse, axis3 - axis3, percent);
      BackRight.spin(reverse, axis3 + axis3, percent);
      MiddleLeft.spin(reverse, axis3 - axis3, percent);
      MiddleRight.spin(reverse, axis3 + axis3, percent);
    }
  }

  wait(20, msec);
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, msec);
  }
}