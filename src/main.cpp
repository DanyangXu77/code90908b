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

bool allowCatapultControl, allowCatapultLiftControl, allowRatchetControl, allowWingsControl;
bool catapultOn, catapultLiftOn, ratchetOn, wingsOn;

void pid() {

}

void preauton() {
}

void autonomous(void) {
}

void nothing() { cout << "input >> nothing" << endl; }

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
      Catapult.spin(vex::forward, 200, vex::rpm);
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
      Catapult.spinFor(vex::forward, 700, vex::degrees, 200, vex::rpm);
    } else {
      Catapult.spinFor(vex::reverse, 700, vex::degrees, 200, vex::rpm);
    }
  }
}

void intake() {
  cout << "input >> intake" << endl;
  Intake.spin(vex::forward, 200, rpm);
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
  Intake.spin(vex::reverse, 200, rpm);
}

void wings() {
  cout << "input >> wings" << endl;
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
  Controller.ButtonR1     .released(allowControls);
  Controller.ButtonR2     .released(allowControls);
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