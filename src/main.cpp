#include "SplitFlap.h"
#include <AccelStepper.h> //https://github.com/waspinator/AccelStepper, reference: http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
#include <Arduino.h>

#define numLetters 8 //how many letters are expected over serial or in timed array

AccelStepper module0motor = AccelStepper(AccelStepper::FULL4WIRE, 3, 4, 5, 6);
SplitFlap module0 = SplitFlap( //a split flap needs a pointer to an accelStepper and pins and info
    &module0motor,             //https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab
    2, 2038, 200, 400, 50, 0); //zero sensor pin, speed, accel, numberOfFlaps, zeroPositionOffset
SF_MAKE_ISR_MACRO(module0);    //create interrupt service routine

char lettersToShow[numLetters];

void setup()
{
    Serial.begin(115200);
    module0.begin();
    module0.setUpInterrupts(module0_SF_ISR);
}

/**
 * @brief  convert character to flap number
 * @param  letter: (char)
 * @retval (char)
 */
char letterToFlapNumber(char letter)
{
    //TODO: update once stickers are on flaps, verify the letter exists too.
    return letter - 64; //A=1, B=2 ...
}

/**
 * @brief  this function reads characters from the serial port
 * @param  out: pointer to char array to put characters to display in
 * @retval (bool) true if new characters are ready to be displayed
 */
bool runSerialComm(char *out)
{
    static char serialIn[8];
    static int serialCounter = 0;

    char c = Serial.read();
    if (c == -1) //nothing in serial queue
        return false;
    if (c == '\n')
    { //end of string
        if (serialCounter != numLetters)
        {
            serialCounter = 0;
            return false; //wrong number of characters were received!
        }
        for (int i = 0; i < numLetters; i++)
        { //copy characters to display to output array
            out[i] = serialIn[i];
        }
        serialCounter = 0;
        return true; //got a new string to display
    }
    else
    {                                   //not end of string yet, read characters into array
        if (serialCounter < numLetters) //don't go past the end of serialIn array
            serialIn[serialCounter] = c;
        serialCounter++;
    }
    return false; //not at end of string yet
}

/**
 * @brief  go through the rows of a table of characters on a timed interval
 * @param  out: pointer to char array to put characters to display in
 * @param  numLines: how many rows are in strings
 * @param  strings[][numLetters]: table of characters to go through
 * @param  interval: milliseconds between sets
 * @retval (bool) true if new characters were put in out
 */
bool timedLetters(char *out, int numLines, char strings[][numLetters], unsigned long interval)
{
    static unsigned long lastSentLetterMillis = 0;
    static int currentLine = 0;
    if (millis() - lastSentLetterMillis > interval)
    {
        for (int i = 0; i < numLetters; i++)
        { //copy characters to display to output array
            out[i] = strings[currentLine][i];
        }
        currentLine++;
        if (currentLine >= numLines)
            currentLine = 0; // restart from beginning
        lastSentLetterMillis = millis();
        return true;
    }
    return false; //not ready yet
}

void loop()
{
    // static char linesToDisplay[][numLetters] = {
    //     { '1', '2', '3', '4', '5', '6', '7', '8' },
    //     { '8', '7', '6', '5', '4', '3', '2', '1' },
    //     { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' }
    // };
    // if (timedLetters(lettersToShow, 3, linesToDisplay, 20000)) {
    //     Serial.println(lettersToShow);
    //     module0.display(letterToFlapNumber(lettersToShow[0]));
    // }

    if (runSerialComm(lettersToShow))
    {
        char flapNumberToDisplay = letterToFlapNumber(lettersToShow[0]);
        module0.display(flapNumberToDisplay);
    }

    module0.run();
}
