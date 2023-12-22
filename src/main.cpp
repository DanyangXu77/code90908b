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

double pi = 3.14159265358979323846264338327950288419716939937510582097494459;

double driveInches = 57;
double driveDegrees = 5.67;

bool reversed = false;

double lateralkP = 0.12, lateralkI = 0.000001, lateralkD = 0.01;
double turnkP = 0.125, turnkI = 0.000001, turnkD = 0.025;
double lateralError, previousLateralError, turnError, previousTurnError;
double leftMultiplier = 1.0048, rightMultiplier = 0.9952;

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

  int dummy = 0;

  bool testing = false;

  resetDriveSensors();

  // mode = "no_auton";

  // unIntake() doesn't work.

  if (testing) {
  } else if (mode == "close_auton") {
    resetDriveSensors();
    // cout << "start close_auton" << endl;
    // Wings.set(true);
    // turn(-90);
    // drive(20);
    // turn(45);
    // Wings.set(false);
    // wait(500, vex::msec);
    // Wings.set(true);
    // turn(-45);
    // drive(-20);
    // turn(90);
    // drive(-48);
    // turn(-90);
    // drive(-5);
    // unIntake();
    // waitUntil(!Intake.isSpinning());
    // drive(5);
    // turn(90);
    // drive(48);
    // turn(90);
    // Wings.set(false);
    cout << "start close_auton" << endl;
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
    resetDriveSensors();
    cout << "start far_auton" << endl;
    Wings.set(true);
    drive(48);
    turn(90);
    drive(2);
    unIntake();
    waitUntil(!Intake.isSpinning());
    // drive(-6);
    // drive(6);
    drive(-24);
    Wings.set(false);
    wait(1000, vex::msec);
    drive(27);
    Wings.set(true);
    // turn(-120);
    // susDrive(10);
    // turn(120);
    // waitUntil(!Intake.isSpinning());
    // turn(161.6);
    // drive(38);
    // drive(-38);
    // turn(-161.6);
    // unIntake();
    // waitUntil(!Intake.isSpinning());
    // turn(161.6);
    // drive(38);
    // drive(-38);
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
    if (Controller.ButtonDown.pressing()) {
      CatapultRelease.set(true);
      wait(200, vex::msec);
      CatapultRelease.set(false);
    }
    if (Controller.ButtonB.pressing()) {
      if (cata2) {
        cata2 = false;
        CatapultLift.spinFor(vex::forward, 250, vex::degrees, 160, vex::rpm, false);
        wait(500, vex::msec);
        Catapult.spinFor(vex::forward, 16560, vex::degrees, 80, vex::rpm, false);
        while (Catapult.isSpinning()) {
          if (Controller.ButtonX.pressing()) {
            Catapult.stop();
            CatapultLift.spinFor(vex::reverse, 250, vex::degrees, 160, vex::rpm, false);
          }
        }
        CatapultLift.spinFor(vex::reverse, 250, vex::degrees, 160, vex::rpm, false);
        CatapultRelease.set(true);
        wait(200, vex::msec);
        CatapultRelease.set(false);
      }
    } else {
      cata2 = true;
    }
  }
}

void usercontrol(void) {
  vex::thread::interruptAll();
  vex::thread c(cata);
  killPID = true;
  stopMotors();
  Intake.stop();
  setMotorsType(vex::coast);
  pidOn = false;
  killPID = true;
  while (1) {
    int axis1, axis3;

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

    // cout << axis1 << ", " << axis3 << endl;

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

    if (Controller.ButtonX.pressing()) {
      setMotorsType(vex::hold);
    } else {
      setMotorsType(vex::coast);
    }
  }

  wait(20, vex::msec);
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, vex::msec);
  }
}