#ifndef _SPLIT_FLAP_H_
#define _SPLIT_FLAP_H_
#include <AccelStepper.h>
class SplitFlap {
public:
    AccelStepper* motor;
    SplitFlap(AccelStepper* _motor, byte _zeroSensorPin)
        : motor(_motor)
    {
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
};

#endif