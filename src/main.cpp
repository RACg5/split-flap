#include "SplitFlap.h"
#include <AccelStepper.h> //https://github.com/waspinator/AccelStepper, reference: http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
#include <Arduino.h>

AccelStepper module1motor = AccelStepper(AccelStepper::FULL4WIRE, 3, 4, 5, 6);
SplitFlap module1 = SplitFlap( //a split flap needs a pointer to an accelStepper and pins and info
    &module1motor, //https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab
    2, 2038, 200, 400); //zero sensor pin, speed, accel
SF_MAKE_ISR_MACRO(module1); //create interrupt service routine

char lettersToShow[] = { ' ', ' ' };

void setup()
{
    Serial.begin(115200);
    module1.begin();
    module1.setUpInterrupts(module1_SF_ISR);
}

char letterToFlapNumber(char letter)
{
    //TODO: update once stickers are on flaps, verify the letter exists too.
    return letter - 64; //A=1, B=2 ...
}

/**
 * @brief  this function reads characters from the serial port
 * @param  out: pointer to char array to put characters to display in
 * @param  start: within the received string, the first character that gets displayed
 * @param  end: within the received string, the last character that gets displayed
 * @note (example1: start=0 end=0 gets the first character) (example2: start=1 end=2 gets the second and third character) end-start+1 must equal the size of lettersToShow
 * @retval (bool) true if new characters are ready to be displayed
 */
bool runSerialComm(char* out, char start, char end)
{
    static char serialIn[] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; //8 characters
    static byte serialCounter = 0;

    char c = Serial.read();
    if (c == -1)
        return false;
    if (c == '\n') { //end of string
        if (serialCounter <= end) {
            serialCounter = 0;
            return false; //not enough characters were received
        }
        for (int i = start; i <= end; i++) { //copy characters to display to output array
            out[i - start] = serialIn[i];
        }
        serialCounter = 0;
        return true;
    } else { //not end of string yet, read characters into array
        serialIn[serialCounter] = c;
        serialCounter++;
        if (serialCounter >= 8) //don't go past the end of serialIn array
            serialCounter = 7;
    }
    return false; //not at end of string yet
}

void loop()
{
    if (runSerialComm(lettersToShow, 0, 0)) {
        module1.display(letterToFlapNumber(lettersToShow[0]));
    }

    module1.run();
}
