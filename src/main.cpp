#include <stdio.h>
#include <cmath>
#include "robot-config.h"
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

double lateralError, previousLateralError, lateralErrorDifference, totalLateralError;
double turnError, previousTurnError, turnErrorDifference, totalTurnError;

double distanceError, previousDistanceError, distanceErrorDifference, totalDistanceError;
double gpsTurnError, previousGpsTurnError, gpsTurnErrorDifference, totalGpsTurnError;

double pidDampening = 1;

double desiredLateralValue = 0, desiredTurnValue = 0;
double desiredX = 0, desiredY = 0, desiredRotation = 0;
double currentX, currentY;

bool pidOn = true;
bool usingGPS = false;

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

int steps;
int totalSteps;

bool pidStarted = false;
int a = 0;

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

// QOL FUNCTIONS
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

void centrePrintAt(int xPos, int yPos, std::string txt) {
  int xDiff = Brain.Screen.getStringWidth(txt.c_str());
  int yDiff = Brain.Screen.getStringHeight(txt.c_str());
  Brain.Screen.printAt(xPos - xDiff / 2, yPos + yDiff / 2, txt.c_str());
}

void progressBarUpdate(int steps, int total) {
  int width = steps / total * 240;
  Brain.Screen.setPenWidth(10);
  Brain.Screen.setPenColor(white);
  Brain.Screen.drawRectangle(100, 40, 280, 40);
  Brain.Screen.drawRectangle(120, 60, width, 0);
}

void unIntake() {
  std::cout << "intake forward" << std::endl;
  startIntake(vex::forward);
  wait(500, msec);
  startIntake(reverse);
  std::cout << "intake reverse" << std::endl;
  steps++;
  progressBarUpdate(steps, totalSteps);
}

char* dtc(double d) {
  char* buffer = new char[sizeof(d)];
  sprintf(buffer, "%f", d);
  return buffer;
}

// PID FUNCTIONS
void pid() {
  while (true) {
    a++;
    pidStarted = true;
    Controller.Screen.clearLine();
    Controller.Screen.print(a);
    centrePrintAt(440, 200, dtc(a));
    if (pidOn) {
      if (usingGPS) {
        distanceError = sqrt(pow(desiredX - currentX, 2) + pow(desiredY - currentY, 2));
        gpsTurnError = desiredRotation - Inertial.rotation(degrees);

        distanceErrorDifference = distanceError - previousDistanceError;
        gpsTurnErrorDifference = gpsTurnError - previousGpsTurnError;

        totalDistanceError += distanceError;
        totalGpsTurnError += gpsTurnError;

        if (totalDistanceError > 10) totalDistanceError = 10;
        if (totalDistanceError < -10) totalDistanceError = -10;
        if (totalGpsTurnError > 10) totalGpsTurnError = 10;
        if (totalGpsTurnError < -10) totalGpsTurnError = -10;

        // NOT DONE
      } else {
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
    }

    if (killPID) {
      Brain.Screen.setPenColor(red);
      Brain.Screen.setPenWidth(80);
      Brain.Screen.drawRectangle(0, 0, 0, 0);
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
    centrePrintAt(240, 210, dtc(lateralError / desiredLateralValue));
  }
  stopMotorsInPID = true;
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "end drive");
  std::cout << "end drive " << angle << std::endl;
  stopMotors();
  steps++;
  progressBarUpdate(steps, totalSteps);
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
    centrePrintAt(240, 210, dtc(turnError / desiredTurnValue));
  }
  stopMotorsInPID = true;
  brainLines++;
  Brain.Screen.printAt(1, brainLines, "end turn");
  std::cout << "end turn " << angle << std::endl;
  stopMotors();
  steps++;
  progressBarUpdate(steps, totalSteps);
}

void getLocation() {
  currentX = GPS.xPosition();
  currentY = GPS.yPosition();
}

// PREAUTON
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

// AUTONOMOUS 
void autonomous(void) {
  thread p(pid);
  setMotorsType(brake);
  autonStarted = true;
  Brain.Screen.clearScreen(black);
  centrePrintAt(240, 120, mode);

  std::cout << "program start" << std::endl;

  steps = 0;

  bool testing = false;

  resetDriveSensors();

  // mode = "no_auton";

  pidOn = true;
  
  Wings.set(true);

  waitUntil(pidStarted);
  centrePrintAt(240, 180, "pid started");

  if (testing) {
    turn(90);
  } else if (mode == "skills_auton") { // PROGRAMMING SKILLS AUTON
    std::cout << "start skills_auton" << std::endl;
    progressBarUpdate(0, 1);
    totalSteps = 4294967296;
    centrePrintAt(240, 150, "auton started");

  } else if (mode == "close_auton") { // CLOSE SIDE AUTON
    std::cout << "start close_auton" << std::endl;
    progressBarUpdate(0, 1);
    totalSteps = 10;
    centrePrintAt(240, 150, "auton started");
    drive(50);
    turn(50);
    drive(10);
    drive(-10);
    turn(-135);
    drive(3);
    unIntake();
    waitUntil(!Intake.isSpinning());
    drive(-6);
    drive(10);
    drive(-4);
  } else if (mode == "far_auton") { // FAR SIDE AUTON
    std::cout << "start far_auton" << std::endl;
    progressBarUpdate(0, 1);
    totalSteps = 15;
    centrePrintAt(240, 150, "auton started");
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
    drive(26);
    drive(-26);
    turn(-170);
    unIntake();
    drive(-6);
    Wings.set(false);
    drive(10);
    drive(-4);
  } else {
    progressBarUpdate(0, 1);
    centrePrintAt(240, 150, "no auton started");
    killPID = true;
    pidOn = false;
    stopMotors();
  }
  killPID = true;
  pidOn = false;
  stopMotors();
}

// NOTE: LIMITSWITCHES ARE NOT INSTALLED, WHEN THEY ARE ON THE ROBOT COMMENT OUT THE `spinFor' FUNCTIONS AND UNCOMMENT THE ONES THAT ARE COMMENTED.

void cata() { // CATAPULT CONTROL
  cout << "buttons pressed: " << "l1 - " << Controller.ButtonL1.pressing() << "; l2 - " << Controller.ButtonL2.pressing() << "; r1 - " << Controller.ButtonR1.pressing() << "; r2 - " << Controller.ButtonR2.pressing() << "; a - " << Controller.ButtonA.pressing() << "; b - " << Controller.ButtonB.pressing() << "; x - " << Controller.ButtonX.pressing() << "; y - " << Controller.ButtonY.pressing() << "; right - " << Controller.ButtonRight.pressing() << "; down - " << Controller.ButtonDown.pressing() << "; left - " << Controller.ButtonLeft.pressing() << "; up - " << Controller.ButtonUp.pressing() << endl;

  Wings.set(true);
  Ratchet.set(false);

  CatapultLift.setStopping(hold);
  while (true) {
    if (getController(catapultControl)) {
      if (cata2) {
        cata2 = false;
        cata1 = !cata1;
        MatchLoadLock.set(cata1);
        if (cata1) {
          Catapult.spin(reverse, cataSpeed, rpm);
        } else {
          Catapult.stop();
        }
      }
    } else {
      cata2 = true;
    }
    
    if (getController(catapultLiftControl)) {
      if (cataOn2) {
        cout << "input recieved" << endl;
        centrePrintAt(240, 220, "input recieved");
        cataOn2 = false;
        cataOn = !cataOn;
        if (cataOn) {
          cout << "raising catapult" << endl;
          centrePrintAt(240, 220, "raising catapult");
          // CatapultLift.spin(vex::forward, 160, rpm);
          CatapultLift.spinFor(vex::forward, moveDegrees, degrees, 160, rpm, false);
          // waitUntil(getController(catapultLiftControl) || CatapultTop.value());
          // CatapultLift.stop();
        } else {
          cout << "lowering catapult" << endl;
          centrePrintAt(240, 220, "lowering catapult");
          // CatapultLift.spin(reverse, 160, rpm);
          CatapultLift.spinFor(reverse, moveDegrees, degrees, 160, rpm, false);
          // CatapultLift.stop();
        }
      }
      wait(20, msec);
    } else {
      cataOn2 = true;
      if (cataOn && CatapultLift.velocity(rpm) == 0) {
        cout << "stopping catapult" << endl;
        centrePrintAt(240, 220, "stopping catapult");
        CatapultLift.stop();
      }
    }
  }
}

void usercontrol(void) {
  thread c(cata);
  if (autonStarted) {
    killPID = true;
  }
  stopMotors();
  Intake.stop();
  setMotorsType(coast);
  pidOn = false;
  int axis1, axis2, axis3;
  while (1) {
    // DRIVETRAIN CONTROL
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

    // OTHER CONTROLS
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
  cout << "main() started" << endl;
  Competition.bStopAllTasksBetweenModes = true;

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  preauton();

  while (true) {
    wait(100, msec);
  }
}