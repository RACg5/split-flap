#ifndef _SPLIT_FLAP_H_
#define _SPLIT_FLAP_H_

#define SF_MAKE_ISR_MACRO(name) \
    void name##_SF_ISR() { name.interruptCallback(); } //helps with setting up interrupts

#include <AccelStepper.h>
class SplitFlap {
public:
    AccelStepper* motor;
    //variables added here can be accessed by any method in the SplitFlap class, add more as needed
    int speedSetting;
    int accelSetting;
    byte zeroSensorPin;

    SplitFlap(AccelStepper* _motor, byte _zeroSensorPin, int _speed, int _accel)
        : motor(_motor)
    {
        zeroSensorPin = _zeroSensorPin;
        speedSetting = _speed;
        accelSetting = _accel;
    }

    /**
     * @brief  call this in setup()
     * @retval None
     */
    void begin()
    {
        motor->setMaxSpeed(speedSetting);
        motor->setAcceleration(accelSetting);
    }

    /**
     * @brief  this function converts what number flap you want to show with a position the wheel should go to
     * @param  flapNumber: (char) flap number to move to
     * @retval (long) position to move to
     */
    static long flapNumberToPosition(char flapNumber)
    {
        //TODO: write conversion code (add variables to constructor for any settings needed)
        return 0;
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
    }

    /**
     * @brief  call this in loop()
     * @retval None
     */
    void run()
    {
        //TODO: does anything else need to go here? maybe disable the motor when not moving to save power?
        motor->run();
    }

    /**
     * @brief  ISRs can't be inside classes, so make an ISR that calls this
     * @retval None
     */
    void interruptCallback()
    {
        //TODO: this gets called when the hall sensor interrupt runs, use that info for zeroing
    }

    /**
     * @note interrupt created by SF_MAKE_ISR_MACRO(name of SplitFlap) will be of the form <name of SplitFlap instance>_SF_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {
        pinMode(zeroSensorPin, INPUT); //TODO: change to INPUT_PULLUP if we decide not to add external resistor

        attachInterrupt(digitalPinToInterrupt(zeroSensorPin), _isrPointer, CHANGE);
    }
};

#endif