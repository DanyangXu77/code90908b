#include "robot-config.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

controller Controller; 

// VEXcode device constructors
motor FrontLeft = motor(PORT4, ratio6_1, false);
motor FrontRight = motor(PORT3, ratio6_1, true);
motor MiddleLeft = motor(PORT6, ratio6_1, false);
motor MiddleRight = motor(PORT1, ratio6_1, true);
motor BackLeft = motor(PORT8, ratio6_1, false);
motor BackRight = motor(PORT2, ratio6_1, true);
// digital_out Wings = digital_out(Brain.ThreeWirePort.H);
// digital_out CatapultTop = digital_out(Brain.ThreeWirePort.A);
// digital_out CatapultBottom = digital_out(Brain.ThreeWirePort.B);
motor Intake = motor(PORT10, ratio18_1, false);
motor CatapultLift = motor(PORT5, ratio36_1, false);
motor Catapult = motor(PORT9, ratio18_1, false);

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
