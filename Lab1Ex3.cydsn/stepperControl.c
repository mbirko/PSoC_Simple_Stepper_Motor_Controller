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

#include "stepperControl.h"
#include <stdbool.h>
#include <stdio.h>
#include "project.h"



uint8_t squence[2][3][8][4] = {//start of array       4D array, outer most = direction, then modes, then steps, then pins. 
                                { //forward direction
                                    {//wavedrive
                                       //A  B  A1 B1
                                        {1, 0, 0, 0}, //step 1 
                                        {0, 1, 0, 0}, //step 2
                                        {0, 0, 1, 0}, //step 3
                                        {0, 0, 0, 1}, //step 4
                                        {1, 0, 0, 0}, //step 1
                                        {0, 1, 0, 0}, //step 2
                                        {0, 0, 1, 0}, //step 3
                                        {0, 0, 0, 1}  //step 4
                                    },//end of wave

                                    {//full step
                                        {1, 1, 0, 0}, //step 1
                                        {0, 1, 1, 0}, //step 2
                                        {0, 0, 1, 1}, //step 3
                                        {1, 0, 0, 1}, //step 4
                                        {1, 1, 0, 0}, //step 1
                                        {0, 1, 1, 0}, //step 2
                                        {0, 0, 1, 1}, //step 3
                                        {1, 0, 0, 1}  //step 4
                                    },//end of full step
                            
                                    {// half step 
                                        {1, 0, 0, 0}, //step 1
                                        {1, 1, 0, 0}, //step 2
                                        {0, 1, 0, 0}, //step 3
                                        {0, 1, 1, 0}, //step 4
                                        {0, 0, 1, 0}, //step 5
                                        {0, 0, 1, 1}, //step 6
                                        {0, 0, 0, 1}, //step 7
                                        {1, 0, 0, 1}  //step 8
                                    }//end of half step
                                },//end of foreward direction 

                                {//backwards direction
                                    {//wavedrive
                                       //A  B  A1 B1
                                        {0, 0, 0, 1}, //step 4 
                                        {0, 0, 1, 0}, //step 3
                                        {0, 1, 0, 0}, //step 2
                                        {1, 0, 0, 0}, //step 1
                                        {0, 0, 0, 1}, //step 4 
                                        {0, 0, 1, 0}, //step 3
                                        {0, 1, 0, 0}, //step 2
                                        {1, 0, 0, 0}, //step 1
                                    },//end of wave

                                    {//full step
                                        {1, 0, 0, 1}, //step 4
                                        {0, 0, 1, 1}, //step 3
                                        {0, 1, 1, 0}, //step 2
                                        {1, 1, 0, 0}, //step 1
                                        {1, 0, 0, 1}, //step 4
                                        {0, 0, 1, 1}, //step 3
                                        {0, 1, 1, 0}, //step 2
                                        {1, 1, 0, 0}, //step 1
                                    },//end of full step
                            
                                    {// half step 
                                        {1, 0, 0, 1}, //step 8
                                        {0, 0, 0, 1}, //step 7
                                        {0, 0, 1, 1}, //step 6
                                        {0, 0, 1, 0}, //step 5
                                        {0, 1, 1, 0}, //step 4
                                        {0, 1, 0, 0}, //step 3
                                        {1, 1, 0, 0}, //step 2
                                        {1, 0, 0, 0}  //step 1
                                    }//end of half step
                                } // end of backwards direction  
                            }; // end of array 


void nextStep(uint8_t direction, uint8_t mode, uint8_t step)
{
    // the pin state is loaded from wished direction, mode and the next step
    Pin_A_Write(squence[direction][mode][step][0]);
    Pin_B_Write(squence[direction][mode][step][1]);
    Pin_A1_Write(squence[direction][mode][step][2]);
    Pin_B1_Write(squence[direction][mode][step][3]);
    //debug uart statement
    {
    char buffer[256];
    snprintf(buffer, sizeof(buffer),"dir = %d mode = %d step = %d, pins = %d %d %d %d\r\n",direction, mode, step, squence[direction][mode][step][0],squence[direction][mode][step][1],squence[direction][mode][step][2],squence[direction][mode][step][3] );
    UART_PutString(buffer);
    }
}

uint8_t speedUp(uint8_t oldSpeed)
{
    if (oldSpeed <= 5) //If we are at or below the  fastest, return orignal speed
    {
        UART_PutString("already at fastest speed\r\n");
        return oldSpeed;
    }
    
    oldSpeed--; // make new faster speed
    
    Timer_WritePeriod(oldSpeed); // make speed faster
    
    return Timer_ReadPeriod();//returns the acctual speed from periode registre 
}

uint8_t slowDown(uint8_t oldSpeed)
{
    
    if (oldSpeed >= 250) //If we are at or above the slowest, return orignal speed
    {
        UART_PutString("already at slowest speed\r\n");
        return oldSpeed;
    }
    
    oldSpeed++; // make new slower speed 
    
    Timer_WritePeriod(oldSpeed); //slow speed down 
    
    return Timer_ReadPeriod(); //returns the acctual speed from periode registre 
}

uint8_t turnSteps(uint8_t dir, uint8_t mode, uint8_t step, uint8_t number)
{
    
     // makes counter int and makes the local step state nStep. 
    uint8_t i, nStep = step;
    // checks if mode is half, if yes take duble steps. 
    uint8_t numberOfSteps = ((mode == 2) ? (number *= 2) : number); 
    
    for(i = 0; i < numberOfSteps; i++) 
    {   
        // makes sure we count from the state we came from, and between 0 and 7. 
        nStep = (step+(i+1))%8;
        nextStep(dir, mode, nStep);
    }
    
    return nStep; //return new state. 
    
}



/* [] END OF FILE */
