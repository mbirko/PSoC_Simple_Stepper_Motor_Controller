/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

    #include <stdbool.h>
    #include <stdint.h>

   
    void nextStep(uint8_t direction, uint8_t mode, uint8_t step); //takes the next step, acording to the current state of the stepper
    uint8_t speedUp(uint8_t oldSpeed); //increses speed by decresing the periode
    uint8_t slowDown(uint8_t oldSpeed);//decresses the speed by incressing the periode
    uint8_t turnSteps(uint8_t dir, uint8_t mode, uint8_t step, uint8_t number); //turns the motor a number of times, acording to the state of the stepper
    
#endif /* STEPPERCONTROL_H */   
