using namespace vex;

extern brain Brain;

// VEXcode devices
extern motor FrontLeft;
extern motor FrontRight;
extern motor MiddleLeft;
extern motor MiddleRight;
extern motor BackLeft;
extern motor BackRight;
extern motor Flywheel;
extern inertial Inertial;
extern digital_out Piston;
extern controller Controller1;
extern motor Intake;
extern digital_out AngleChanger;
extern digital_out AirRelease;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );