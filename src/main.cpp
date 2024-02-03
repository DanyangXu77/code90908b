#include <stdio.h>
#include <cmath>
#include "robot-config.h"
#include "vex.h"
#include <iostream>
using namespace std;

competition Competition;

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

void usercontrol(void) {
  while (true) {
    Controller.ButtonL1.pressed(cout << "a" << endl);
    vex::wait(20, msec);
  }
}

int main() {
  cout << "main() started" << endl;
  Competition.bStopAllTasksBetweenModes = true;

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, msec);
  }
}