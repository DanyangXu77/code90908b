#include "robot-config.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

controller Controller; 

// VEXcode device constructors
motor FrontLeft = motor(PORT15, ratio6_1, true);
motor FrontRight = motor(PORT16, ratio6_1, false);
motor MiddleLeft = motor(PORT14, ratio6_1, true);
motor MiddleRight = motor(PORT17, ratio6_1, false);
motor BackLeft = motor(PORT13, ratio6_1, true);
motor BackRight = motor(PORT18, ratio6_1, false);
digital_out Wings = digital_out(Brain.ThreeWirePort.A);
motor Intake = motor(PORT11, ratio18_1, false);
motor CatapultLift = motor(PORT20, ratio36_1, false);
motor Catapult = motor(PORT7, ratio18_1, false);
digital_out CatapultRelease = digital_out(Brain.ThreeWirePort.C);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}