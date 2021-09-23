#include "SplitFlap.h"
#include <AccelStepper.h> //https://github.com/waspinator/AccelStepper
#include <Arduino.h>
SplitFlap module1 = SplitFlap( //a split flap needs a pointer to an accelStepper and pins and info
    &AccelStepper(AccelStepper::HALF4WIRE, 8, 9, 10, 11), //https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab
    2); //zero sensor pin,
void setup()
{
}

void loop()
{
    module1.run();
}

char letterToFlapNumber(char letter)
{
    return letter - 64;
}
