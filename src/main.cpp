#include <stdio.h>
#include <cmath>
#include <robot-config.h>
#include "amogus.cpp"
#include "vex.h"
using namespace vex;
#include <iostream>
using namespace std;

competition Competition;

// Variables 
double dummy = 0;

double pi = 3.14159265358979323846264338327950288419716939937510582097494459;

bool reversed = false;

double lateralError, previousLateralError, turnError, previousTurnError;

double lateralErrorDifference, turnErrorDifference;

double totalLateralError, totalTurnError;

double pidDampening = 1;

double desiredLateralValue = 0;
double desiredTurnValue = 0;

bool pidOn = true;

bool wingsOn = false;
bool wingsOn2 = true;

bool cata1 = false;
bool cata2 = true;

bool cataOn = false;
bool cataOn2 = true;

bool autonStarted = false;

bool killPID = false;

bool stopMotorsInPID = false;

bool arcade = true;
bool arcade2 = true;

bool ratchet = false;
bool ratchet2 = true;

int brainLines = 0;

bool pidStarted = false;

std::string mode = "no_auton";

directionType opposite(directionType direction) {
  if (direction == vex::forward) return reverse;
  return vex::forward;
}

bool getController(std::string input) {
  if (input == "L1") return Controller.ButtonL1.pressing();
  if (input == "L2") return Controller.ButtonL2.pressing();
  if (input == "R1") return Controller.ButtonR1.pressing();
  if (input == "R2") return Controller.ButtonR2.pressing();
  if (input == "A") return Controller.ButtonA.pressing();
  if (input == "B") return Controller.ButtonB.pressing();
  if (input == "X") return Controller.ButtonX.pressing();
  if (input == "Y") return Controller.ButtonY.pressing();
  if (input == "R") return Controller.ButtonRight.pressing();
  if (input == "D") return Controller.ButtonDown.pressing();
  if (input == "L") return Controller.ButtonLeft.pressing();
  if (input == "U") return Controller.ButtonUp.pressing();
  // switch (input) {
  //   case 10:
  //     return Controller.ButtonA.pressing();
  //     break;
  //   case 11:
  //     return Controller.ButtonB.pressing();
  //     break;
  //   case 12:
  //     return Controller.ButtonX.pressing();
  //     break;
  //   case 13:
  //     return Controller.ButtonY.pressing();
  //     break;
  //   case 20:
  //     return Controller.ButtonRight.pressing();
  //     break;
  //   case 21:
  //     return Controller.ButtonDown.pressing();
  //     break;
  //   case 22:
  //     return Controller.ButtonLeft.pressing();
  //     break;
  //   case 23:
  //     return Controller.ButtonUp.pressing();
  //     break;
  //   case 30:
  //     return Controller.ButtonL1.pressing();
  //     break;
  //   case 31:
  //     return Controller.ButtonL2.pressing();
  //     break;
  //   case 32: 
  //     return Controller.ButtonR1.pressing();
  //     break;
  //   case 33:
  //     return Controller.ButtonL2.pressing();
  //     break;
  // }
  return false;
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
  startIntake(vex::forward);
  wait(500, msec);
  startIntake(reverse);
  std::cout << "intake reverse" << std::endl;
}

char* dtc(double d) {
  char* buffer = new char[sizeof(d)];
  sprintf(buffer, "%f", d);
  return buffer;
}

void centrePrintAt(int xPos, int yPos, std::string txt) {
  int xDiff = Brain.Screen.getStringWidth(txt.c_str());
  int yDiff = Brain.Screen.getStringHeight(txt.c_str());
  Brain.Screen.printAt(xPos - xDiff / 2, yPos + yDiff / 2, txt.c_str());
}

void pid() {
  while (true) {
    pidStarted = true;
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
        FrontLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        FrontRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
        MiddleLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        MiddleRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
        BackLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, percent);
        BackRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, percent);
      }
    }

    if (killPID) {
      break;
    }
  }
}

void drive(double angle) {
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "start turn");
  std::cout << "start drive " << angle << std::endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = -1 * angle * driveInches;
  desiredTurnValue = 0;
  wait(20, msec);
  while (fabs(desiredLateralValue - (MiddleLeft.position(degrees) + MiddleRight.position(degrees)) / 2) > 3) {
    wait(20, msec);
  }
  stopMotorsInPID = true;
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "end drive");
  std::cout << "end drive " << angle << std::endl;
  stopMotors();
}

void turn(double angle) {
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "start turn");
  std::cout << "start turn " << angle << std::endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = 0;
  desiredTurnValue = angle * driveDegrees;
  wait(20, msec);
  while (fabs(fabs(desiredTurnValue - (MiddleLeft.position(degrees) - MiddleRight.position(degrees)) / 2)) > 3) {
    wait(20, msec);
  }
  stopMotorsInPID = true;
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "end turn");
  std::cout << "end turn " << angle << std::endl;
  stopMotors();
}

void preauton() {
  Intake.setStopping(brake);

  Brain.Screen.setPenWidth(20);

  Wings.set(true);
  Ratchet.set(true);

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
  
  Wings.set(true);

  waitUntil(pidStarted);

  if (testing) {
    turn(90);
  } else if (mode == "close_auton") {
    std::cout << "start close_auton" << std::endl;
    drive(50);
    turn(50);
    drive(10);
    drive(-10);
    turn(-135);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-6);
    drive(11);
  } else if (mode == "far_auton") {
    std::cout << "start far_auton" << std::endl;
    drive(47);
    turn(95);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-28);
    Wings.set(false);
    drive(29.5);
    Wings.set(true);
    drive(-4);
    turn(159);
    startIntake(vex::forward);
    drive(26);
    drive(-26);
    turn(-167);
    unIntake();
    drive(-6);
    Wings.set(false);
    drive(17);
  } else {
    killPID = true;
    pidOn = false;
    stopMotors();
  }
  killPID = true;
  pidOn = false;
  stopMotors();
}

// NOTE: LIMITSWITCHES ARE NOT INSTALLED, WHEN THEY ARE ON THE ROBOT COMMENT OUT THE `spinFor' FUNCTIONS AND UNCOMMENT THE ONES THAT ARE COMMENTED.

void cata() {
  Wings.set(true);
  Ratchet.set(false);

  CatapultLift.setStopping(hold);
  while (true) {
    if (getController(catapultControl)) {
      cata2 = false;
      cata1 = !cata1;
      MatchLoadLock.set(cata1);
      if (cata1) {
        Catapult.spin(reverse, cataSpeed, rpm);
      } else {
        Catapult.stop();
      }
    } else {
      cata2 = true;
    }
    if (getController(catapultLiftControl)) {
      if (cataOn2) {
        cataOn2 = false;
        cataOn = !cataOn;
        if (cataOn) {
          CatapultLift.spin(vex::forward, 160, rpm);
          // CatapultLift.spinFor(vex::forward, moveDegrees, degrees, 160, rpm, false);
          waitUntil(CatapultTop.value());
          CatapultLift.stop();
        } else {
          CatapultLift.spin(reverse, 160, rpm);
          // CatapultLift.spinFor(reverse, moveDegrees, degrees, 160, rpm, false);
          waitUntil(CatapultBottom.value());
          CatapultLift.stop();
        }
      }
    } else {
      cataOn2 = true;
      if (CatapultLift.velocity(rpm) == 0) {
        CatapultLift.stop();
      }
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
        FrontLeft.spin(vex::forward, axis3 + axis1, percent);
        FrontRight.spin(vex::forward, axis3 - axis1, percent);
        MiddleLeft.spin(vex::forward, axis3 + axis1, percent);
        MiddleRight.spin(vex::forward, axis3 - axis1, percent);
        BackLeft.spin(vex::forward, axis3 + axis1, percent);
        BackRight.spin(vex::forward, axis3 - axis1, percent);
      } else {
        FrontLeft.spin(vex::forward, axis3, percent);
        FrontRight.spin(vex::forward, axis2, percent);
        MiddleLeft.spin(vex::forward, axis3, percent);
        MiddleRight.spin(vex::forward, axis2, percent);
        BackLeft.spin(vex::forward, axis3, percent);
        BackRight.spin(vex::forward, axis2, percent);
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

    if (getController(driveModeControl)) {
      if (arcade2) {
        arcade = !arcade;
        arcade2 = false;
      }
    } else {
      arcade2 = true;
    }

    if (getController(intakeControl)) {
      Intake.spin(vex::forward, 200, rpm);
    } else if (getController(reverseIntakeControl)) {
      Intake.spin(reverse, 200, rpm);
    } else {
      Intake.stop();
    }

    if (getController(wingsControl)) {
      if (wingsOn2) {
        wingsOn = !wingsOn;
        Wings.set(wingsOn);
        wingsOn2 = false; 
      }
    } else {
      wingsOn2 = true;
    }

    if (getController(ratchetControl)) {
      if (ratchet2) {
        ratchet = !ratchet;
        Ratchet.set(ratchet);
        ratchet2 = false;
      }
    } else {
      ratchet2 = true;
    }

    if (getController(holdControl)) {
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