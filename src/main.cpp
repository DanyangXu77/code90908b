#include <stdio.h>
#include <string>
#include <cmath>
#include <robot-config.h>
#include <future>
#include <vex.h>
using namespace vex;
#include <iostream>
using namespace std;

competition Competition;

double dummy = 0;

double pi = 3.14159265358979323846264338327950288419716939937510582097494459;

double driveInches = 57;
double driveDegrees = 5.67;

bool reversed = false;

double lateralkP = 0.12, lateralkI = 0.000001, lateralkD = 0.01;
double turnkP = 0.125, turnkI = 0.000001, turnkD = 0.025;
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

bool cata2 = true;

bool autonStarted = false;

bool killPID = false;

bool stopMotorsInPID = false;

bool arcade = true;

bool arcade2 = true;

char* mode = "no_auton";

bool sussyAmoger = false;

// extern uint32_t pic_map[];

vex::directionType opposite(vex::directionType direction) {
  if (direction == vex::forward) return vex::reverse;
  return vex::forward;
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

void setMotorsType(vex::brakeType type) {
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

void startIntake(vex::directionType direction) {
  Intake.spin(direction, 200, vex::rpm);
}

void stopIntake() {
  Intake.stop();
}

void unIntake() {
  cout << "intake forward" << endl;
  startIntake(vex::forward);
  vex::wait(500, vex::msec);
  startIntake(vex::reverse);
  cout << "intake reverse" << endl;
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
      
      wait(20, vex::msec);

      cout << leftMotorPosition << ", " << rightMotorPosition << ", " << lateralError << endl;

      if (sussyAmoger && lateralError > -800) {
        cout << "SUS AMOGUS WARNING!!11!1!!!1" << endl;
        Brain.Screen.print("sus amogus warning");
        Wings.set(false);
      }

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
        FrontLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, vex::percent);
        FrontRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, vex::percent);
        MiddleLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, vex::percent);
        MiddleRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, vex::percent);
        BackLeft.spin(vex::forward, (lateralMotorPower + turnMotorPower) * leftMultiplier * pidDampening, vex::percent);
        BackRight.spin(vex::forward, (lateralMotorPower - turnMotorPower) * rightMultiplier * pidDampening, vex::percent);
      }
    }

    if (killPID) {
      break;
    }
  }
}

void drive(double angle) {
  cout << "start drive " << angle << endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = -1 * angle * driveInches;
  desiredTurnValue = 0;
  wait(20, vex::msec);
  while (abs(lateralError) > 3) {
    wait(20, vex::msec);
  }
  stopMotorsInPID = true;
  cout << "end drive " << angle << endl;
  stopMotors();
}

void susDrive(double angle) {
  cout << "start drive " << angle << endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = -1 * angle * driveInches;
  desiredTurnValue = 0;
  sussyAmoger = true;
  wait(20, vex::msec);
  while (abs(lateralError) > 3){
    wait(20, vex::msec);
  }
  stopMotorsInPID = true;
  cout << "end drive " << angle << endl;
  stopMotors();
}

void turn(double angle) {
  cout << "start turn " << angle << endl;
  resetDriveSensors();
  pidOn = true;
  stopMotorsInPID = false;
  totalLateralError = 0;
  totalTurnError = 0;
  desiredLateralValue = 0;
  desiredTurnValue = angle * driveDegrees;
  wait(20, vex::msec);
  while (abs(turnError) > 3) {
    wait(20, vex::msec);
  }
  stopMotorsInPID = true;
  cout << "end turn " << angle << endl;
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
    wait(20, vex::msec);
  }
}

void autonomous(void) {
  vex::thread p(pid);
  setMotorsType(vex::brake);
  autonStarted = true;
  Brain.Screen.clearScreen();
  centrePrintAt(240, 120, "using mode");
  centrePrintAt(240, 180, mode);

  cout << "program start" << endl;

  bool testing = false;

  resetDriveSensors();

  // mode = "no_auton";

  if (testing) {
  } else if (mode == "close_auton") {
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
    if (Controller.ButtonB.pressing()) {
      if (cata2) {
        cata2 = false;
        CatapultLift.spinFor(vex::forward, 540, vex::degrees, 160, vex::rpm, false);
        Catapult.spin(vex::forward, 100, vex::rpm);      
      } else {
        CatapultLift.spinFor(vex::reverse, 540, vex::degrees, 160, vex::rpm, false);
        Catapult.stop();
      }
    } else {
      cata2 = true;
    }
  }
}

void usercontrol(void) {
  vex::thread c(cata);
  killPID = true;
  stopMotors();
  Intake.stop();
  setMotorsType(vex::coast);
  pidOn = false;
  killPID = true;
  int axis1, axis2, axis3;
  while (1) {
    cout << "sus" << endl;
    axis1 = Controller.Axis1.position(vex::percent);
    axis2 = Controller.Axis2.position(vex::percent);
    axis3 = Controller.Axis3.position(vex::percent);

    if (reversed) {
      if (arcade) {
        FrontLeft.spin(vex::forward, axis3 + axis1, vex::percent);
        FrontRight.spin(vex::forward, axis3 - axis1, vex::percent);
        MiddleLeft.spin(vex::forward, axis3 + axis1, vex::percent);
        MiddleRight.spin(vex::forward, axis3 - axis1, vex::percent);
        BackLeft.spin(vex::forward, axis3 + axis1, vex::percent);
        BackRight.spin(vex::forward, axis3 - axis1, vex::percent);
      } else {
        FrontLeft.spin(vex::forward, axis3, vex::percent);
        FrontRight.spin(vex::forward, axis2, vex::percent);
        MiddleLeft.spin(vex::forward, axis3, vex::percent);
        MiddleRight.spin(vex::forward, axis2, vex::percent);
        BackLeft.spin(vex::forward, axis3, vex::percent);
        BackRight.spin(vex::forward, axis2, vex::percent);
      }
    } else {
      if (arcade) {
        FrontLeft.spin(vex::reverse, axis3 + axis1, vex::percent);
        FrontRight.spin(vex::reverse, axis3 - axis1, vex::percent);
        MiddleLeft.spin(vex::reverse, axis3 + axis1, vex::percent);
        MiddleRight.spin(vex::reverse, axis3 - axis1, vex::percent);
        BackLeft.spin(vex::reverse, axis3 + axis1, vex::percent);
        BackRight.spin(vex::reverse, axis3 - axis1, vex::percent);
      } else {
        FrontLeft.spin(vex::reverse, axis3, vex::percent);
        FrontRight.spin(vex::reverse, axis2, vex::percent);
        MiddleLeft.spin(vex::reverse, axis3, vex::percent);
        MiddleRight.spin(vex::reverse, axis2, vex::percent);
        BackLeft.spin(vex::reverse, axis3, vex::percent);
        BackRight.spin(vex::reverse, axis2, vex::percent);
      }
    }

    // cout << axis1 << ", " << axis3 << endl;

    if (Controller.ButtonY.pressing()) {
      if (arcade2) {
        arcade = !arcade;
        arcade2 = false;
      }
    } else {
      arcade2 = true;
    }

    if (Controller.ButtonR1.pressing()) {
      Intake.spin(vex::forward, 200, vex::rpm);
    } else if (Controller.ButtonR2.pressing()) {
      Intake.spin(vex::reverse, 200, vex::rpm);
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

    Controller.Screen.clearLine();
    Controller.Screen.print("drive = ");
    if (arcade) Controller.Screen.print("arcade"); else Controller.Screen.print("tank");

    if (Controller.ButtonX.pressing()) {
      setMotorsType(vex::hold);
    } else {
      setMotorsType(vex::coast);
    }
  }

  wait(20, vex::msec);
}

int main() {
  Competition.bStopAllTasksBetweenModes = true;

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, vex::msec);
  }
}