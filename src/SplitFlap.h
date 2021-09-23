#ifndef _SPLIT_FLAP_H_
#define _SPLIT_FLAP_H_

#define SF_MAKE_ISR_MACRO(name) \
    void name##_SF_ISR() { name.interruptCallback(); } //helps with setting up interrupts

#include <AccelStepper.h>
class SplitFlap {
public:
    AccelStepper* motor;
    byte zeroSensorPin;
    SplitFlap(AccelStepper* _motor, byte _zeroSensorPin)
        : motor(_motor)
    {
        zeroSensorPin = _zeroSensorPin;
    }
    /**
     * @brief  call this in setup()
     * @retval None
     */
    void begin()
    {
        motor->setMaxSpeed(200);
        motor->setAcceleration(1000);
    }
    /**
     * @brief  
     * @note   
     * @retval 
     */
    static long flapNumberToPosition(char flapNumber)
    {
    }
    /**
     * @brief  
     * @param  flapNumber: (char)
     * @retval None
     */
    void display(char flapNumber)
    {
    }
    /**
     * @brief  call this in loop()
     * @retval None
     */
    void run()
    {
        motor->run();
    }
    /**
     * @brief  ISRs can't be inside classes, so make an ISR that calls this
     * @retval None
     */
    void interruptCallback()
    {
    }

    /**
     * @note interrupt created by SF_MAKE_ISR_MACRO(name of SplitFlap) will be of the form <name of SplitFlap instance>_SF_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {
        pinMode(zeroSensorPin, INPUT);

        attachInterrupt(digitalPinToInterrupt(zeroSensorPin), _isrPointer, CHANGE);
    }
};

#endif