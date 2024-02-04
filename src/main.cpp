// 1
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
double lateralMultiplier = -56, turnMultiplier = -5.35;

double lateralError, previousLateralError, lateralErrorDifference, totalLateralError;
double turnError, previousTurnError, turnErrorDifference, totalTurnError;
double desiredLateralValue = 0, desiredTurnValue = 0;

bool allowCatapultControl, allowCatapultLiftControl, allowRatchetControl, allowWingsControl;
bool catapultOn = false, catapultLiftOn = false, ratchetOn = true, wingsOn = true;

bool autonStarted = false, killPID = false;

string autonMode = "no_auton";

bool pidOn = false;

void resetDriveSensors() {
  Left.resetPosition();
  Right.resetPosition();
}
void stopMotors() {
  Left.stop();
  Right.stop();
}
void startIntake(vex::directionType direction) {
  Intake.spin(direction, 200, vex::rpm);
}
void unIntake() {
  cout << "intake forward" << endl;
  startIntake(vex::reverse);
  wait(500, msec);
  startIntake(vex::forward);
  cout << "intake reverse" << endl;
}
void stopIntake() {
  Intake.stop();
}

void pid() {
  while (true) {
    if (killPID) {
      resetDriveSensors();
      stopMotors();
      break;
    }
    if (pidOn) {
      double leftMotorPosition = Left.position(degrees);
      double rightMotorPosition = Right.position(degrees);
      double averageMotorPosition = (leftMotorPosition + rightMotorPosition) / 2;
      double turnDifference = (leftMotorPosition - rightMotorPosition) / 2;

      lateralError = desiredLateralValue - averageMotorPosition;
      lateralErrorDifference = lateralError - previousLateralError;
      turnError = desiredTurnValue - turnDifference;
      turnErrorDifference = turnError - previousTurnError;
      totalLateralError += lateralError;
      totalTurnError += turnError;

      if (totalLateralError > 1000) totalLateralError = 1000;
      if (totalTurnError < -1000) totalTurnError = -1000;
      if (totalLateralError > 1000) totalLateralError = 1000;
      if (totalTurnError < -1000) totalTurnError = -1000;

      double lateralMotorPower = lateralError * lateralkP + totalLateralError * lateralkI + lateralErrorDifference * lateralkD;

      double turnMotorPower = turnError * turnkP + totalTurnError * turnkI + turnErrorDifference * turnkD;

      previousLateralError = lateralError;

      previousTurnError = turnError;
      
      wait(20, msec);

      cout << leftMotorPosition << ", " << rightMotorPosition << ", " << lateralError << ", " << turnError << endl;
      
      Left.spin(vex::forward, (lateralMotorPower + turnMotorPower), percent);
      Right.spin(vex::forward, (lateralMotorPower - turnMotorPower), percent);
    }
  }
}

void drive(double angle) {
  cout << "start drive " << angle << endl;
  resetDriveSensors();
  pidOn = true;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = angle * lateralMultiplier;
  desiredTurnValue = 0;
  while (fabs(desiredLateralValue - (Left.position(degrees) + Left.position(degrees)) / 2) > sqrt(fabs(angle))) {
    wait(20, msec);
  }
  cout << "end drive " << angle << endl;
  stopMotors();
}
void turn(double angle) {
  cout << "start turn " << angle << endl;
  resetDriveSensors();
  pidOn = true;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = 0;
  desiredTurnValue = angle * turnMultiplier;
  while (fabs(desiredTurnValue - (Left.position(degrees) - Right.position(degrees)) / 2) > 3) {
    wait(20, msec);
  }
  cout << "end turn " << angle << endl;
  stopMotors();
}

void preauton() {
  cout << "preauton started" << endl;
  CatapultLift.setStopping(vex::hold);
  Ratchet.set(true);
  Wings.set(true);
}

void autonomous() {
  cout << "auton started" << endl;
  Left.setStopping(brake);
  Right.setStopping(brake);
  autonStarted = true;
  thread p(pid);
  pidOn = true;
  autonMode = "close";
  Wings.set(true);
  if (autonMode == "close") {
    cout << "start close_auton" << endl;
    drive(50);
    turn(50);
    drive(7);
    drive(-7);
    turn(-135);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-6);
    drive(10);
    drive(-4);
  } else if (autonMode == "far") {
    cout << "start far auton" << endl;
    drive(46.5);
    turn(95);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-28);
    Wings.set(false);
    drive(29);
    Wings.set(true);
    drive(-4);
    turn(156);
    startIntake(vex::forward);
    drive(24);
    drive(-24);
    turn(-170);
    unIntake();
    drive(-6);
    Wings.set(false);
    drive(10);
    drive(-4);
    // startIntake(vex::reverse);
    // drive(5);
    // drive(3);
    // drive(-35);
    // turn(135);
    // startIntake(vex::forward);
    // drive(24);
    // drive(-5);
    // turn(-110);
    // startIntake(vex::reverse);
    // drive(45);
    // turn(150);
    // startIntake(vex::forward);
    // drive(25);
    // drive(-25);
    // startIntake(vex::reverse);
    // Wings.set(false);
    // drive(25);
    // drive(-10);
    // drive(10);
  } else if (autonMode == "skills") {

  } else {
    Wings.set(true);
  }

  killPID = true;
}

void nothing() {
  cout << "input >> nothing" << endl;
}
void allowControls() {
  allowCatapultControl = true;
  allowCatapultLiftControl = true;
  allowRatchetControl = true;
  allowWingsControl = true;
}
void catapult() {
  cout << "input >> catapult" << endl;
  if (allowCatapultControl) {
    allowCatapultControl = false;
    catapultOn = !catapultOn;
    if (catapultOn) {
      Catapult.spin(vex::reverse, 200, vex::rpm);
    } else {
      Catapult.stop();
    }
  }
}
void catapultLift() {
  cout << "input >> catapultLift" << endl;
    if (allowCatapultLiftControl) {
    allowCatapultLiftControl = false;
    catapultLiftOn = !catapultLiftOn;
    if (catapultLiftOn) {
      CatapultLift.spinFor(vex::forward, 1400, vex::degrees, 200, vex::rpm, false);
    } else {
      CatapultLift.spinFor(vex::reverse, 1600, vex::degrees, 200, vex::rpm, false);
    }
  }
}
void intake() {
  cout << "input >> intake" << endl;
  Intake.spin(vex::reverse, 200, rpm);
}
void ratchet() {
  cout << "input >> ratchet" << endl;
  if (allowRatchetControl) {
    allowRatchetControl = false;
    ratchetOn = !ratchetOn;
    Ratchet.set(ratchetOn);
  }
}
void reverseIntake() {
  cout << "input >> reverseIntake" << endl;
  Intake.spin(vex::forward, 200, rpm);
}
void wings() {
  cout << "input >> wings" << endl;
  if (allowWingsControl) {
    allowWingsControl = false;
    wingsOn = !wingsOn;
    Wings.set(wingsOn);
  }
}

void addInputControls() {
  Controller.ButtonA      .pressed(catapult);
  Controller.ButtonB      .pressed(catapultLift);
  Controller.ButtonX      .pressed(nothing);
  Controller.ButtonY      .pressed(nothing);
  Controller.ButtonL1     .pressed(wings);
  Controller.ButtonL2     .pressed(nothing);
  Controller.ButtonR1     .pressed(intake);
  Controller.ButtonR2     .pressed(reverseIntake);
  Controller.ButtonUp     .pressed(nothing);
  Controller.ButtonRight  .pressed(nothing);
  Controller.ButtonDown   .pressed(nothing);
  Controller.ButtonLeft   .pressed(nothing);

  Controller.ButtonA      .released(allowControls);
  Controller.ButtonB      .released(allowControls);
  Controller.ButtonX      .released(nothing);
  Controller.ButtonY      .released(nothing);
  Controller.ButtonL1     .released(allowControls);
  Controller.ButtonL2     .released(nothing);
  Controller.ButtonR1     .released(stopIntake);
  Controller.ButtonR2     .released(stopIntake);
  Controller.ButtonUp     .released(nothing);
  Controller.ButtonRight  .released(nothing);
  Controller.ButtonDown   .released(nothing);
  Controller.ButtonLeft   .released(nothing);
}

void handleDriving() {
  double axis1 = Controller.Axis1.position(vex::percent);  
  double axis3 = Controller.Axis3.position(vex::percent);
  Left.spin(vex::reverse, axis3 + axis1, vex::percent);
  Right.spin(vex::reverse, axis3 - axis1, vex::percent);
}

void checkCollision() {
  if (CatapultLift.velocity(rpm) == 0 && !allowCatapultLiftControl) {
    CatapultLift.stop();
  }
}

void usercontrol() {
  Left.setStopping(coast);
  Right.setStopping(coast);
  cout << "usercontrol started" << endl;
  if (autonStarted) { killPID = true; pidOn = false; stopMotors(); }
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
