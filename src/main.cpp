#include "SplitFlap.h"
#include <AccelStepper.h> //https://github.com/waspinator/AccelStepper
#include <Arduino.h>
SplitFlap module1 = SplitFlap( //a split flap needs a pointer to an accelStepper and pins and info
    &AccelStepper(AccelStepper::HALF4WIRE, 8, 9, 10, 11), //https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab
    2, 100, 100); //zero sensor pin, speed, accel
SF_MAKE_ISR_MACRO(module1); //create interrupt service routine
void setup()
{
    Serial.begin(9600);
    module1.begin();
    module1.setUpInterrupts(module1_SF_ISR);
}

void loop()
{
    module1.run();
}

char letterToFlapNumber(char letter)
{
    return letter - 64; //A=1, B=2 ...
}

char returnCharOnTimer()
{
    //use for testing, return 0 if nothing to display yet
    return 0;
}

char returnCharSerialComm()
{
    return 0;
}