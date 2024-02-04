#include "robot-config.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

brain Brain;

controller Controller; 

motor FrontLeft = motor(PORT13, ratio6_1, false);
motor FrontRight = motor(PORT19, ratio6_1, true);
motor MiddleLeft = motor(PORT14, ratio6_1, false);
motor MiddleRight = motor(PORT15, ratio6_1, true);
motor BackLeft = motor(PORT11, ratio6_1, false);
motor BackRight = motor(PORT20, ratio6_1, true);
digital_out Wings = digital_out(Brain.ThreeWirePort.H);
digital_out Ratchet = digital_out(Brain.ThreeWirePort.G);
digital_out MatchLoadLock = digital_out(Brain.ThreeWirePort.C);
motor Intake = motor(PORT16, ratio18_1, false);
motor CatapultLift = motor(PORT17, ratio36_1, false);
motor Catapult = motor(PORT12, ratio18_1, false);

bool RemoteControlCodeEnabled = true;

void vexcodeInit( void ) {}
