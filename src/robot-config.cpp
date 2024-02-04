#include "robot-config.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

brain Brain;

controller Controller; 

motor FrontLeft = motor(PORT9, ratio6_1, false);
motor FrontRight = motor(PORT3, ratio6_1, true);
motor MiddleLeft = motor(PORT8, ratio6_1, false);
motor MiddleRight = motor(PORT7, ratio6_1, true);
motor BackLeft = motor(PORT21, ratio6_1, false);
motor BackRight = motor(PORT2, ratio6_1, true);
digital_out Wings = digital_out(Brain.ThreeWirePort.H);
digital_out Ratchet = digital_out(Brain.ThreeWirePort.G);
digital_out MatchLoadLock = digital_out(Brain.ThreeWirePort.C);
motor Intake = motor(PORT6, ratio18_1, false);
motor CatapultLift = motor(PORT5, ratio36_1, false);
motor Catapult = motor(PORT10, ratio18_1, false);

bool RemoteControlCodeEnabled = true;

void vexcodeInit( void ) {}
