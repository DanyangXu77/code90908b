#include "amogus.cpp"

// ------  PID CONFIG  ------

// lateral tuning
double lateralkP = 0.12, lateralkI = 0.000001, lateralkD = 0.01;

// turn tuning
double turnkP = 0.125, turnkI = 0.000001, turnkD = 0.025;

// multiplier
double driveInches = 57, driveDegrees = 5.67;

// ------ CATAP CONFIG ------

// lift turn value
int moveDegrees = 690; // nice