// 0

#include <stdio.h>
#include <cmath>
#include "robot-config.h"
#include "vex.h"
#include <iostream>
using namespace std;

competition Competition;

motor_group Left(FrontLeft, MiddleLeft, BackLeft);
motor_group Right(FrontRight, MiddleRight, BackRight);

double lateralkP = 0.1, lateralkI = 0.00000001, lateralkD = 0.001, turnkP = 0.1, turnkI = 0.00000001, turnkD = 0.001;
double lateralMultiplier = 56, turnMultiplier = -5.35;

double lateralError, previousLateralError, lateralErrorDifference, totalLateralError;
double turnError, previousTurnError, turnErrorDifference, totalTurnError;
double desiredLateralValue = 0, desiredTurnValue = 0;

void pid() {

}

void preauton() {
}

void autonomous(void) {
}

void nothing() { cout << "input >> nothing" << endl; }

void catapult() {
  cout << "input >> catapult" << endl;
}

void catapultLift() {
  cout << "input >> catapultLift" << endl;
}

void intake() {
  cout << "input >> intake" << endl;
}

void reverseIntake() {
  cout << "input >> reverseIntake" << endl;
}

void addInputControls() {
  Controller.ButtonA      .pressed(nothing);
  Controller.ButtonB      .pressed(nothing);
  Controller.ButtonX      .pressed(nothing);
  Controller.ButtonY      .pressed(nothing);
  Controller.ButtonL1     .pressed(nothing);
  Controller.ButtonL2     .pressed(nothing);
  Controller.ButtonR1     .pressed(nothing);
  Controller.ButtonR2     .pressed(nothing);
  Controller.ButtonUp     .pressed(nothing);
  Controller.ButtonRight  .pressed(nothing);
  Controller.ButtonDown   .pressed(nothing);
  Controller.ButtonLeft   .pressed(nothing);
}

void handleDriving() {
  double axis1 = Controller.Axis1.value();  
  double axis3 = Controller.Axis3.value();
  Left.spin(vex::forward, axis3 + axis1, vex::percent);
  Right.spin(vex::forward, axis3 - axis1, vex::percent);
}

void usercontrol(void) {
  addInputControls();
  while (true) {
    handleDriving();
    vex::wait(20, msec);
  }
}

int main() {
  Competition.bStopAllTasksBetweenModes = true;

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    vex::wait(20, msec);
  }
}