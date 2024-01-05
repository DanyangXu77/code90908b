#include "amogus.cpp"

// ------  PID CONFIG  ------

    // lateral tuning
    double lateralkP = 0.1, lateralkI = 0.00000001, lateralkD = 0.001;

    // turn tuning
    double turnkP = 0.1, turnkI = 0.00000001, turnkD = 0.001;

    // multiplier
    double driveInches = 1, driveDegrees = 1;


// ------ CATA CONFIG ------

    // lift turn value
    int moveDegrees = 690; // nice