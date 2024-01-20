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
extern digital_out Ratchet;
extern digital_out CatapultRelease;
extern digital_out CatapultTop;
extern digital_out CatapultBottom;
extern digital_out MatchLoadLock;
extern inertial Inertial;
extern controller Controller;

void  vexcodeInit( void );