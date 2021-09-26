#ifndef _SPLIT_FLAP_H_
#define _SPLIT_FLAP_H_

#define SF_MAKE_ISR_MACRO(name) \
    void name##_SF_ISR() { name.interruptCallback(); } //this macro helps with setting up interrupts

#include <AccelStepper.h>
class SplitFlap
{
public:
    AccelStepper *motor;
    //variables added here can be accessed by any method in the SplitFlap class, add more as needed
    int speedSetting;
    int accelSetting;
    byte zeroSensorPin;
    int stepsPerRev;
    long zeroPosition;
    int numberOfFlaps;
    int zeroPositionOffset;

    SplitFlap(AccelStepper *_motor, byte _zeroSensorPin, int _stepsPerRev, int _speed, int _accel, int _numberOfFlaps, int _zeroPositionOffset)
        : motor(_motor)
    {
        zeroSensorPin = _zeroSensorPin;
        speedSetting = _speed;
        accelSetting = _accel;
        stepsPerRev = _stepsPerRev;
        zeroPosition = 0;
        numberOfFlaps = _numberOfFlaps;
        zeroPositionOffset = _zeroPositionOffset;
    }

    /**
     * @brief  call this in setup()
     * @retval None
     */
    void begin() //write code in here like it's in setup()
    {
        motor->setMaxSpeed(speedSetting);
        motor->setAcceleration(accelSetting);
        //TODO: the wheel needs to be moved so it gets zeroed on startup (maybe that job is done in run() though)
    }

    long calculateMove(long target)
    {
        if (target < motor->currentPosition() - zeroPosition)
        {
            return target - (motor->currentPosition() - zeroPosition) + stepsPerRev;
        }
        else
        {
            return target + (motor->currentPosition() - zeroPosition);
        }
    }

    /**
     * @brief  this function converts what number flap you want to show with a position the wheel should go to
     * @param  flapNumber: (char) flap number to move to
     * @retval (long) position to move to
     */
    long flapNumberToPosition(char flapNumber)
    {
        //TODO: write conversion formula (add variables to constructor for any settings needed)
        //        assume the zero point is known and doesn't move, but don't assume the zero point is exactly next to a flap

        return stepsPerRev / numberOfFlaps + zeroPositionOffset;
    }

    /**
     * @brief  call this function to display a specific flap
     * @note (currently unknown, but calling this more than once at a time may cause issues)
     * @param  flapNumber: (char)
     * @retval None
     */
    void display(char flapNumber)
    {
        //TODO: write this. The wheel should always turn the same way (probably).
        //      and maybe don't do anything if the initial zeroing hasn't been completed or a move is in progress?

        long target = flapNumberToPosition(flapNumber); // compared to zero
        long positionToMoveTo = calculateMove(target);

        motor->moveTo(positionToMoveTo);
    }

    /**
     * @brief  call this in loop()
     * @retval None
     */
    void run() //write code in here like it's in loop()
    {
        //TODO: does anything else need to go here? maybe disable the motor when not moving to save power?
        motor->run(); //runs AccelStepper motor, note -> not . because motor is a pointer not an object
    }

    /**
     * @brief  ISRs can't be inside classes, so make an ISR that calls this
     * @retval None
     */
    void interruptCallback()
    {
        //TODO: this gets called when the hall sensor interrupt runs, use that info for zeroing
        zeroPosition = motor->currentPosition();
    }

    /**
     * @note interrupt created by SF_MAKE_ISR_MACRO(name of SplitFlap) will be of the form <name of SplitFlap instance>_SF_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {
        pinMode(zeroSensorPin, INPUT); //TODO: change to INPUT_PULLUP if we decide not to add external resistor

        attachInterrupt(digitalPinToInterrupt(zeroSensorPin), _isrPointer, FALLING);
    }
};

#endif