using namespace vex;

extern brain Brain;

// VEXcode devices
extern motor FrontLeft;
extern motor FrontRight;
extern motor MiddleLeft;
extern motor MiddleRight;
extern motor BackLeft;
extern motor BackRight;
extern digital_out LeftWing;
extern digital_out RightWing;
extern controller Controller;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );