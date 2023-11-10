#include <vex.h>

using namespace vex;

extern brain Brain;

extern motor FrontLeft;
extern motor FrontRight;
extern motor MiddleLeft;
extern motor MiddleRight;
extern motor BackLeft;
extern motor BackRight;
extern digital_out Wings;
extern motor Intake;
extern motor CatapultLift;
extern motor Catapult;
extern digital_out CatapultRelease;
extern controller Controller;

void  vexcodeInit( void );