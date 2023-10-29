#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
motor FrontLeft = motor(PORT1, ratio18_1, true);
motor FrontRight = motor(PORT2, ratio18_1, false);
motor MiddleLeft = motor(PORT3, ratio18_1, false);
motor MiddleRight = motor(PORT4, ratio18_1, true);
motor BackLeft = motor(PORT5, ratio18_1, false);
motor BackRight = motor(PORT6, ratio18_1, true);
digital_out LeftWing = digital_out(Brain.ThreeWirePort.A);
digital_out RightWing = digital_out(Brain.ThreeWirePort.B);

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