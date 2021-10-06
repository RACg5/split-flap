#include "SplitFlap.h"
#include <AccelStepper.h> //https://github.com/waspinator/AccelStepper, reference: http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
#include <Arduino.h>

#define numLetters 20 //how many letters are expected over serial or in timed array

//warning: uncomment only one of the following!
#define SERIAL_STRING //send a string of characters to display sequentially on a timer
// #define SERIAL_LINE_BY_LINE //send characters one at a time (or multiple at a time if more than one display is connected)
// #define TIMED_LINES //cycle through preprogrammed lines of characters to display

AccelStepper module0motor = AccelStepper(AccelStepper::DRIVER, 4, 3); //step pin, direction pin
SplitFlap module0 = SplitFlap( //a split flap needs a pointer to an accelStepper and pins and info
    &module0motor, //https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab
    2, 2038, 500, 400, 50, 160); //zero sensor pin, stepsPerRev, speed, accel, numberOfFlaps, zeroPositionOffset
SF_MAKE_ISR_MACRO(module0); //create interrupt service routine

char lettersToShow[numLetters];

int currentLetter = 0;

void setup()
{
    Serial.begin(115200);
    module0.setUpInterrupts(module0_SF_ISR);
    module0.begin();
}

/**
 * @brief  convert character to flap number
 * @param  letter: (char)
 * @retval (char)
 */
char letterToFlapNumber(char letter)
{
    //TODO: update once stickers are on flaps with additional symbols
    if (letter == 32)
        return 0; //space
    if (letter >= 65 && letter <= 90) {
        return letter - 'A' + 1; //A=1, B=2
    }
    if (letter >= 97 && letter <= 122)
        return letter - 'a' + 1; //a=1, b=2
    return 0; //not a valid character, show blank flap
}

/**
 * @brief  this function reads characters from the serial port
 * @note  a \n (newline) character is expected to mark the end of the string
 * @param  out: pointer to char array to put characters to display in
 * @param  checkLength: (bool) true = reject if length not numLetters
 * @retval (bool) true if new characters are ready to be displayed
 */
bool runSerialComm(char* out, bool checkLength)
{
    static char serialIn[numLetters];
    static int serialCounter = 0;

    char c = Serial.read();
    if (c == -1) //nothing in serial queue
        return false;
    if (c == '\n') { //end of string
        if (serialCounter != numLetters && checkLength) {
            serialCounter = 0;
            return false; //wrong number of characters were received!
        }
        for (int i = 0; i < numLetters; i++) { //copy characters to display to output array
            if (i < serialCounter)
                out[i] = serialIn[i];
            else
                out[i] = 0;
        }
        serialCounter = 0;
        return true; //got a new string to display
    } else { //not end of string yet, read characters into array
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
bool timedLines(char* out, int numLines, char strings[][numLetters], unsigned long interval)
{
    static unsigned long lastSentLetterMillis = 0;
    static int currentLine = 0;
    if (millis() - lastSentLetterMillis > interval) {
        for (int i = 0; i < numLetters; i++) { //copy characters to display to output array
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

/**
 * @brief  cycle through string on a timer
 * @note  The end of the string should be indicated with a null character
 * @param  out: (char*) pointer to char to put character in
 * @param  string[numLetters]: char array to cycle through
 * @param  interval: (unsigned long) minimum time to pause between letters
 * @param  endInterval: (unsigned long) extra time to pause at end
 * @param  wait: (bool) if true, wait to give new character
 * @retval (bool) is there a new character ready to display
 */
bool timedLetters(char* out, char string[numLetters], unsigned long interval, unsigned long endInterval, bool wait)
{
    static unsigned long lastSentLetterMillis = 0;
    if (wait) {
        lastSentLetterMillis = millis();
        return false;
    }
    if (millis() - lastSentLetterMillis > interval + (currentLetter == 0 ? endInterval : 0)) {
        *out = string[currentLetter];
        currentLetter++;
        if (currentLetter >= numLetters || string[currentLetter] == '\0')
            currentLetter = 0; // restart from beginning
        lastSentLetterMillis = millis();
        return true;
    }
    return false; //not ready yet
}

void loop()
{

#ifdef SERIAL_STRING

    if (runSerialComm(lettersToShow, false))
        currentLetter = 0;
    char letterToShow;
    if (timedLetters(&letterToShow, lettersToShow, 3000, 3000, module0.motor->isRunning())) {
        // Serial.print("LETTER ");
        Serial.println(letterToShow);
        char flapNumberToDisplay = letterToFlapNumber(letterToShow);
        module0.display(flapNumberToDisplay);
    }

#endif

#ifdef TIMED_LINES
    static char linesToDisplay[][numLetters] = {
        { 'A', '2', '3', '4', '5', '6', '7', '8' },
        { 'C', '7', '6', '5', '4', '3', '2', '1' },
        { 'B', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
        { '@', 'b', 'c', 'd', 'e', 'f', 'g', 'h' }
    };
    if (timedLines(lettersToShow, 4, linesToDisplay, 9000)) {
        // Serial.print("LETTERS ");
        Serial.println(lettersToShow);
        module0.display(letterToFlapNumber(lettersToShow[0]));
    }
#endif

#ifdef SERIAL_LINE_BY_LINE
    if (runSerialComm(lettersToShow, true)) {
        // Serial.print("LETTER ");
        Serial.println(lettersToShow[0]);
        char flapNumberToDisplay = letterToFlapNumber(lettersToShow[0]);
        module0.display(flapNumberToDisplay);
    }
#endif

    module0.run();
}
