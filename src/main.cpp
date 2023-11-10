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
  pidOn = false;
}

void turn(double angle) {
  pidOn = true;
  resetDriveSensors();
  desiredLateralValue = 0;
  desiredTurnValue = angle;
  while ((MiddleLeft.position(degrees) - angle) > 2 && (MiddleRight.position(degrees) + angle) > 2) wait(20, msec);
  pidOn = false;
}

void centrePrintAt(int xPos, int yPos, char* txt) {
  int xDiff = Brain.Screen.getStringWidth(txt);
  int yDiff = Brain.Screen.getStringHeight(txt);
  Brain.Screen.printAt(xPos - xDiff / 2, yPos + yDiff / 2, txt);
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
  // Brain.Screen.drawImageFromBuffer(pic_map, 0, 0, 480, 240);
  while (!autonStarted) {
    if (Brain.Screen.pressing()) {
      if (Brain.Screen.xPosition() < 240) {
        if (mode == "close_auton") {
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
        Brain.Screen.drawImageFromFile("pic.png", 0, 0);
      } else {
        if (mode == "far_auton") {
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
        Brain.Screen.drawImageFromFile("pic.png", 0, 0);
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
  if (mode == "close_auton") {

  } else if (mode == "far_auton") {

  }
}

void usercontrol(void) {
  int axis1, axis3;
  while (1) {
    axis1 = Controller.Axis1.position(percent);
    axis3 = Controller.Axis3.position(percent);

    if (reversed) {
      FrontLeft.spin(forward, axis3 + axis1, percent);
      FrontRight.spin(forward, axis3 - axis1, percent);
      MiddleLeft.spin(forward, axis3 + axis1, percent);
      MiddleRight.spin(forward, axis3 - axis1, percent);
      BackLeft.spin(forward, axis3 + axis1, percent);
      BackRight.spin(forward, axis3 - axis1, percent);
    } else {
      FrontLeft.spin(reverse, axis3 - axis1, percent);
      FrontRight.spin(reverse, axis3 + axis1, percent);
      MiddleLeft.spin(reverse, axis3 - axis1, percent);
      MiddleRight.spin(reverse, axis3 + axis1, percent);
      BackLeft.spin(reverse, axis3 - axis1, percent);
      BackRight.spin(reverse, axis3 + axis1, percent);
    }
    
    if (Controller.ButtonR1.pressing()) {
      Intake.spin(forward, 200, rpm);
    } else {
      Intake.stop();
    }

    if (Controller.ButtonB.pressing()) {
      if (cata2) {
        cata2 = false;
        CatapultLift.spinFor(forward, 250, degrees, 160, rpm, false);
        wait(500, msec);
        Catapult.spinFor(forward, 16560, degrees, 140, rpm, true);
        CatapultLift.spinFor(reverse, 250, degrees, 160, rpm, false);
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