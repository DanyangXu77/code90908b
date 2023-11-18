#include <stdio.h>
#include <string>
#include <cmath>
#include <robot-config.h>
#include <vex.h>
using namespace vex;
#include <iostream>
using namespace std;


competition Competition;

double pi = 3.14159265358979323846264338327950288419716939937510582097494459;

double inches = 1;
double turns = 1;

bool reversed = false;

double lateralkP = 0.5, lateralkI = 0, lateralkD = 0;
double turnkP = 0, turnkI = 0, turnkD = 0;
double lateralError, previousLateralError, turnError, previousTurnError;

double lateralErrorDifference, turnErrorDifference;

double totalLateralError, totalTurnError;

double desiredLateralValue;
double desiredTurnValue;

bool pidOn = true;

bool wingsOn = false;
bool wingsOn2 = true;

bool cata2 = true;

bool autonStarted = false;

char* mode = "no_auton";

// extern uint32_t pic_map[];

void resetDriveSensors() {
  FrontLeft.resetPosition();
  FrontRight.resetPosition();
  MiddleLeft.resetPosition();
  MiddleRight.resetPosition();
  BackLeft.resetPosition();
  BackRight.resetPosition();
}

char* dtc(double d) {
  char* buffer = new char[sizeof(d)];
  sprintf(buffer, "%f", d);
  return buffer;
}

void centrePrintAt(int xPos, int yPos, char* txt) {
  int xDiff = Brain.Screen.getStringWidth(txt);
  int yDiff = Brain.Screen.getStringHeight(txt);
  Brain.Screen.printAt(xPos - xDiff / 2, yPos + yDiff / 2, txt);
}

void pid() {
  while (true) {
    if (pidOn) {
      double leftMotorPosition = MiddleLeft.position(vex::degrees);
      double rightMotorPosition = MiddleRight.position(vex::degrees);
      double averageMotorPosition = (leftMotorPosition + rightMotorPosition) / 2;
      double turnDifference = leftMotorPosition - rightMotorPosition;

      lateralError = desiredLateralValue - averageMotorPosition;

      lateralErrorDifference = lateralError - previousLateralError;

      turnError = desiredTurnValue - turnErrorDifference;

      turnErrorDifference = turnError - previousTurnError;

      totalLateralError += lateralError;

      totalTurnError += turnError;

      double lateralMotorPower = lateralError * lateralkP + totalLateralError * lateralkI + lateralErrorDifference * lateralkD;

      double turnMotorPower = turnError * turnkP + totalTurnError * turnkI + turnErrorDifference * turnkD;

      previousLateralError = lateralError;
      previousTurnError = turnError;
      
      wait(20, msec);

      cout << leftMotorPosition << ", " << rightMotorPosition;

      FrontLeft.spin(vex::forward, lateralMotorPower + turnMotorPower, vex::percent);
      FrontRight.spin(vex::forward, lateralMotorPower - turnMotorPower, vex::percent);
      MiddleLeft.spin(vex::forward, lateralMotorPower + turnMotorPower, vex::percent);
      MiddleRight.spin(vex::forward, lateralMotorPower - turnMotorPower, vex::percent);
      BackLeft.spin(vex::forward, lateralMotorPower + turnMotorPower, vex::percent);
      BackRight.spin(vex::forward, lateralMotorPower - turnMotorPower, vex::percent);
    }
  }
}

void drive(double angle) {
  cout << "start";
  pidOn = true;
  resetDriveSensors();
  desiredLateralValue = angle;
  desiredTurnValue = 0;
  while ((MiddleLeft.position(vex::degrees) - angle) > 2 && (MiddleRight.position(vex::degrees) - angle) > 2) wait(20, msec);
  pidOn = false;
  cout << "end";
}

void turn(double angle) {
  cout << "start";
  pidOn = true;
  resetDriveSensors();
  desiredLateralValue = 0;
  desiredTurnValue = angle;
  while ((MiddleLeft.position(vex::degrees) - angle) > 2 && (MiddleRight.position(vex::degrees) + angle) > 2) wait(20, msec);
  pidOn = false;
  cout << "end";
}

void preauton() {
  Intake.setStopping(brake);

  Brain.Screen.setPenWidth(20);

  Brain.Screen.setPenColor(white);
  Brain.Screen.drawRectangle(10, 10, 210, 210);
  Brain.Screen.drawRectangle(250, 10, 210, 210);
  Brain.Screen.setFont(mono30);
  centrePrintAt(120, 120, "CLOSE AUTON");
  centrePrintAt(360, 120, "FAR AUTON");
  while (!autonStarted) {
    if (Brain.Screen.pressing()) {
      if (Brain.Screen.xPosition() < 240) {
        if (strncmp(mode, "close_auton", 11)) {
          continue;
        }
        mode = "close_auton";
        Brain.Screen.setPenColor(red);
        Brain.Screen.drawRectangle(10, 10, 210, 210);
        centrePrintAt(120, 120, "CLOSE AUTON");
        Brain.Screen.setFont(mono15);
        centrePrintAt(120, 180, "(selected)");
        Brain.Screen.setPenColor(black);
        Brain.Screen.drawRectangle(250, 10, 210, 210);
        Brain.Screen.setFont(mono30);
        Brain.Screen.setPenColor(white);
        centrePrintAt(360, 120, "FAR AUTON");
      } else {
        if (strncmp(mode, "close_auton", 11)) {
          continue;
        }
        mode = "far_auton";
        Brain.Screen.setPenColor(red);
        Brain.Screen.drawRectangle(250, 10, 210, 210);
        centrePrintAt(360, 120, "FAR AUTON");
        Brain.Screen.setFont(mono15);
        centrePrintAt(360, 180, "(selected)");
        Brain.Screen.setPenColor(black);
        Brain.Screen.drawRectangle(10, 10, 210, 210);
        Brain.Screen.setFont(mono30);
        Brain.Screen.getStringWidth("");
        Brain.Screen.setPenColor(white);
        centrePrintAt(120, 120, "CLOSE AUTON");
      }
    }
    wait(20, msec);
  }
}

void autonomous(void) {
  thread p(pid);
  autonStarted = true;
  Brain.Screen.clearScreen();
  centrePrintAt(240, 120, "using mode");
  centrePrintAt(240, 180, mode);

  cout << "program start";

  if (strncmp(mode, "close_auton", 11)) {
    
  } else if (strncmp(mode, "close_auton", 11)) {

  }
}

void usercontrol(void) {
  int axis1, axis3;
  while (1) {
    axis1 = Controller.Axis1.position(vex::percent);
    axis3 = Controller.Axis3.position(vex::percent);

    if (reversed) {
      FrontLeft.spin(vex::forward, axis3 + axis1, vex::percent);
      FrontRight.spin(vex::forward, axis3 - axis1, vex::percent);
      MiddleLeft.spin(vex::forward, axis3 + axis1, vex::percent);
      MiddleRight.spin(vex::forward, axis3 - axis1, vex::percent);
      BackLeft.spin(vex::forward, axis3 + axis1, vex::percent);
      BackRight.spin(vex::forward, axis3 - axis1, vex::percent);
    } else {
      FrontLeft.spin(vex::reverse, axis3 - axis1, vex::percent);
      FrontRight.spin(vex::reverse, axis3 + axis1, vex::percent);
      MiddleLeft.spin(vex::reverse, axis3 - axis1, vex::percent);
      MiddleRight.spin(vex::reverse, axis3 + axis1, vex::percent);
      BackLeft.spin(vex::reverse, axis3 - axis1, vex::percent);
      BackRight.spin(vex::reverse, axis3 + axis1, vex::percent);
    }
    
    if (Controller.ButtonR1.pressing()) {
      Intake.spin(vex::forward, 200, vex::rpm);
    } else if (Controller.ButtonR2.pressing()) {
      Intake.spin(vex::reverse, 200, vex::rpm);
    } else {
      Intake.stop();
    }

    if (Controller.ButtonB.pressing()) {
      if (cata2) {
        cata2 = false;
        CatapultLift.spinFor(vex::forward, 250, vex::degrees, 160, vex::rpm, false);
        wait(500, msec);
        Catapult.spinFor(vex::forward, 16560, vex::degrees, 140, vex::rpm, true);
        CatapultLift.spinFor(vex::reverse, 250, vex::degrees, 160, vex::rpm, false);
        CatapultRelease.set(false);
      }
    } else {
      cata2 = true;
    }

    if (Controller.ButtonL1.pressing()) {
      if (wingsOn2) {
        wingsOn = !wingsOn;
        Wings.set(wingsOn);
        wingsOn2 = false;
      }
    } else {
      wingsOn2 = true;
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