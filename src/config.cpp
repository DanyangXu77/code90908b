#include "amogus.cpp"
#include <string>
using namespace std;

    /* -------------------------------------------------
    |  CONTROLS CONFIG                                 |
    --------------------------------------------------*/

    // catapult (hold)
    string catapultControl = "A";

    // catapult lift (toggle)
    string catapultLiftControl = "B";

    // intake (hold)
    string intakeControl = "R1";

    // reverse intake (hold)
    string reverseIntakeControl = "R2";

    // wings (toggle)
    string wingsControl = "L1";

    // ratchet (toggle)
    string ratchetControl = "D";

    // hold drivetrain motors (hold)
    string holdControl = "X";

    // driving mode (toggle)
    string driveModeControl = "Y";


    /* -------------------------------------------------
    |  PID CONFIG                                      |
    --------------------------------------------------*/

    // lateral tuning
    double lateralkP = 0.1, lateralkI = 0.00000001, lateralkD = 0.001;

    // turn tuning
    double turnkP = 0.1, turnkI = 0.00000001, turnkD = 0.001;

    // multiplier
    double driveInches = 56, driveDegrees = -5.35;

    /* -------------------------------------------------
    |  CATAPULT CONFIG                                 |
    --------------------------------------------------*/

    // lift turn value
    int moveDegrees = 720; // will get deprecated with addition of limitswitch

    // speed of cata
    int cataSpeed = 170;


    /* -------------------------------------------------
    |  OTHER                                           |
    --------------------------------------------------*/

    // motor multiplier
    double leftMultiplier = 0.998, rightMultiplier = 1.002;
