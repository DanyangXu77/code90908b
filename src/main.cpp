#include <stdio.h>
#include <string>
#include <cmath>
#include <robot-config.h>
#include "amogus.cpp"
#include "vex.h"
using namespace vex;
#include <iostream>

competition Competition;

// Variables 
double dummy = 0;

double pi = 3.14159265358979323846264338327950288419716939937510582097494459;

bool reversed = false;

double lateralError, previousLateralError, turnError, previousTurnError;
double leftMultiplier = 1.0, rightMultiplier = 1.0;

double lateralErrorDifference, turnErrorDifference;

double totalLateralError, totalTurnError;

double pidDampening = 1;

double desiredLateralValue = 0;
double desiredTurnValue = 0;

bool pidOn = true;

bool wingsOn = false;
bool wingsOn2 = true;

bool cataOn = false;
bool cataOn2 = true;

bool autonStarted = false;

bool killPID = false;

bool stopMotorsInPID = false;

bool arcade = true;
bool arcade2 = true;

bool ratchet = false;
bool ratchet2 = true;

char* mode = "no_auton";

directionType opposite(directionType direction) {
  if (direction == forward) return reverse;
  return forward;
}

double sign(double i) {
  return std::round(abs(i) / i);
}

void resetDriveSensors() {
  FrontLeft.resetPosition();
  FrontRight.resetPosition();
  MiddleLeft.resetPosition();
  MiddleRight.resetPosition();
  BackLeft.resetPosition();
  BackRight.resetPosition();
}

void setMotorsType(brakeType type) {
  FrontLeft.setBrake(type);
  FrontRight.setBrake(type);
  MiddleLeft.setBrake(type);
  MiddleRight.setBrake(type);
  BackLeft.setBrake(type);
  BackRight.setBrake(type);
}

void stopMotors() {
  FrontLeft.stop();
  FrontRight.stop();
  MiddleLeft.stop();
  MiddleRight.stop();
  BackLeft.stop();
  BackRight.stop();
}

void startIntake(directionType direction) {
  Intake.spin(direction, 200, rpm);
}

void stopIntake() {
  Intake.stop();
}

void unIntake() {
  std::cout << "intake forward" << std::endl;
  startIntake(forward);
  wait(500, msec);
  startIntake(reverse);
  std::cout << "intake reverse" << std::endl;
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
      double leftMotorPosition = MiddleLeft.position(degrees);
      double rightMotorPosition = MiddleRight.position(degrees);
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

      std::cout << leftMotorPosition << ", " << rightMotorPosition << ", " << lateralError << std::endl;

      if (stopMotorsInPID) {
        desiredLateralValue = 0;
        desiredTurnValue = 0;
        resetDriveSensors();
        FrontLeft.stop();
        FrontRight.stop();
        MiddleLeft.stop();
        MiddleRight.stop();
        BackLeft.stop();
        BackRight.stop();
      } else {
        double signMultiplier = sign(desiredLateralValue + desiredTurnValue);
        FrontLeft.spin(forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        FrontRight.spin(forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
        MiddleLeft.spin(forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        MiddleRight.spin(forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
        BackLeft.spin(forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        BackRight.spin(forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
      }
    }

    if (killPID) {
      break;
    }
  }
}

void drive(double angle) {
  std::cout << "start drive " << angle << std::endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = -1 * angle * driveInches;
  desiredTurnValue = 0;
  wait(20, msec);
  while (abs(lateralError) > 3) {
    wait(20, msec);
  }
  stopMotorsInPID = true;
  std::cout << "end drive " << angle << std::endl;
  stopMotors();
}

void turn(double angle) {
  std::cout << "start turn " << angle << std::endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = 0;
  desiredTurnValue = angle * driveDegrees;
  wait(20, msec);
  while (abs(turnError) > 3) {
    wait(20, msec);
  }
  stopMotorsInPID = true;
  std::cout << "end turn " << angle << std::endl;
  stopMotors();
}

void preauton() {
  Intake.setStopping(brake);

  Brain.Screen.setPenWidth(20);

  Wings.set(true);

  Brain.Screen.setPenColor(white);
  Brain.Screen.drawRectangle(10, 10, 210, 210);
  Brain.Screen.drawRectangle(250, 10, 210, 210);
  Brain.Screen.setFont(mono30);
  centrePrintAt(120, 120, "CLOSE AUTON");
  centrePrintAt(360, 120, "FAR AUTON");

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
      }
    }
    wait(20, msec);
  }
}

void autonomous(void) {
  thread p(pid);
  setMotorsType(brake);
  autonStarted = true;
  Brain.Screen.clearScreen();
  centrePrintAt(240, 120, "using mode");
  centrePrintAt(240, 180, mode);

  std::cout << "program start" << std::endl;

  bool testing = false;

  resetDriveSensors();

  // mode = "no_auton";

  if (testing) {
  } else if (mode == "close_auton") {
    std::cout << "start close_auton" << std::endl;
    Wings.set(true);
    drive(48);
    turn(50);
    drive(10);
    drive(-10);
    turn(-135);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-6);
    drive(8);
  } else if (mode == "far_auton") {
    
  } else {
    killPID = true;
    pidOn = false;
    stopMotors();
  }
  killPID = true;
  pidOn = false;
  stopMotors();
}

void cata() {
  while (true) {
    if (Controller.ButtonA.pressing()) {
      Catapult.spin(reverse, 100, rpm);
    } else {
      Catapult.stop();
    }
    if (Controller.ButtonB.pressing()) {
      if (cataOn2) {
        cataOn2 = false;
        cataOn = !cataOn;
        if (cataOn) {
          CatapultLift.spinFor(forward, moveDegrees, degrees, 160, rpm, false);
        } else {
          CatapultLift.spinFor(reverse, moveDegrees, degrees, 160, rpm, false);
        }
      }
    } else {
      cataOn2 = true;
    }
  }
}

void usercontrol(void) {
  thread c(cata);
  killPID = true;
  stopMotors();
  Intake.stop();
  setMotorsType(coast);
  pidOn = false;
  killPID = true;
  int axis1, axis2, axis3;
  while (1) {
    std::cout << "sus" << std::endl;
    axis1 = Controller.Axis1.position(percent);
    axis2 = Controller.Axis2.position(percent);
    axis3 = Controller.Axis3.position(percent);

    if (reversed) {
      if (arcade) {
        FrontLeft.spin(forward, axis3 + axis1, percent);
        FrontRight.spin(forward, axis3 - axis1, percent);
        MiddleLeft.spin(forward, axis3 + axis1, percent);
        MiddleRight.spin(forward, axis3 - axis1, percent);
        BackLeft.spin(forward, axis3 + axis1, percent);
        BackRight.spin(forward, axis3 - axis1, percent);
      } else {
        FrontLeft.spin(forward, axis3, percent);
        FrontRight.spin(forward, axis2, percent);
        MiddleLeft.spin(forward, axis3, percent);
        MiddleRight.spin(forward, axis2, percent);
        BackLeft.spin(forward, axis3, percent);
        BackRight.spin(forward, axis2, percent);
      }
    } else {
      if (arcade) {
        FrontLeft.spin(reverse, axis3 + axis1, percent);
        FrontRight.spin(reverse, axis3 - axis1, percent);
        MiddleLeft.spin(reverse, axis3 + axis1, percent);
        MiddleRight.spin(reverse, axis3 - axis1, percent);
        BackLeft.spin(reverse, axis3 + axis1, percent);
        BackRight.spin(reverse, axis3 - axis1, percent);
      } else {
        FrontLeft.spin(reverse, axis3, percent);
        FrontRight.spin(reverse, axis2, percent);
        MiddleLeft.spin(reverse, axis3, percent);
        MiddleRight.spin(reverse, axis2, percent);
        BackLeft.spin(reverse, axis3, percent);
        BackRight.spin(reverse, axis2, percent);
      }
    }

    // std::cout << axis1 << ", " << axis3 << std::endl;

    if (Controller.ButtonY.pressing()) {
      if (arcade2) {
        arcade = !arcade;
        arcade2 = false;
      }
    } else {
      arcade2 = true;
    }

    if (Controller.ButtonR1.pressing()) {
      Intake.spin(forward, 200, rpm);
    } else if (Controller.ButtonR2.pressing()) {
      Intake.spin(reverse, 200, rpm);
    } else {
      Intake.stop();
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

    if (Controller.ButtonDown.pressing()) {
      if (ratchet2) {
        ratchet = !ratchet;
        Ratchet.set(ratchet);
        ratchet2 = false;
      }
    } else {
      ratchet2 = true;
    }

    if (Controller.ButtonX.pressing()) {
      setMotorsType(hold);
    } else {
      setMotorsType(coast);
    }
  }

  wait(20, msec);
}

int main() {
  Competition.bStopAllTasksBetweenModes = true;

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, msec);
  }
}