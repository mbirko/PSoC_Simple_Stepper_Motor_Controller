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

// Includes
#include "project.h"
#include "stepperControl.h"

#include <stdio.h>
#include <stdbool.h>

//typedefs 
typedef enum {
    waveDrive,
    fullStep,
    halfStep
} Mode;


// Interrupt handlers
CY_ISR_PROTO(UART_rx_handler);
CY_ISR_PROTO(TC_handler);

// Global variables
char buffer[256];
uint8_t speed = 250;
uint8_t step = 0;
uint8_t numberOfSteps = 0;
Mode mode = 0;

// flags 
bool newStep = 0;
bool direction = 1;
bool slowing = 0; 
bool speeding = 0; 
bool startingMotor = 0;
bool stoppingMotor = 0;
bool motorRunning = 1;
bool turn = 0; 



int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    // Initialization
    UART_Start();
    Timer_Start();
    
    
    isr_UART_rx_StartEx(UART_rx_handler);
    isr_TIMER_StartEx( TC_handler );
    
    // sets tje start speed in the periode of the timer.
    Timer_WritePeriod(speed); 
    
    char buffer[256];
    UART_PutString("Program started\r\n");
    snprintf(buffer, sizeof(buffer), "current speed: %d (5-250, lower is faster) \r\n", speed);
    UART_PutString(buffer);
    
    snprintf(buffer, sizeof(buffer), "current direction: %d (0 forward 1 backwards) \r\n", direction);
    UART_PutString(buffer);
    
    snprintf(buffer, sizeof(buffer), "current mode: %d (0 wave-drive, 1 fullstep, 2 halfstep)  \r\n", mode);
    UART_PutString(buffer);
    
    // Loop
    for(;;)
    {   
        if (stoppingMotor)
        {
            isr_TIMER_Disable(); 
            stoppingMotor = false;
            motorRunning = 0;
        }
            
        if (startingMotor)
        {
            isr_TIMER_Enable(); 
            startingMotor = false;
            motorRunning = 1;
        }
    
        if (newStep)
        {
            /*
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "mode: %d step %d\r\n", mode, step);
            UART_PutString(buffer);
            */
            //makes next steps according to the state
            nextStep(direction, mode, step); 
            newStep = false;
        }
        
        if (slowing)
        {
            // slows down and saves the new speed
            speed = slowDown(speed); 
            
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "slowing(flowser is faster): %d \r\n", speed);
            UART_PutString(buffer);
                
            slowing = false;
        }
        
        if(speeding)
        {
            //speeds up and saves the new speed 
            speed = speedUp(speed); 
            
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "speed is now(lower is faster): %d \r\n", speed);
            UART_PutString(buffer);
                
            speeding = false;
        }
        
        if (turn) 
        {
             //if the motor is running, it wont take number of steps
            if (motorRunning)
            {
                UART_PutString("motor running, cant turn 90 when its already turning\r\n");
            }
            else
            {
                // turns a number of steps and saves the step its stops at to.
                step = turnSteps(direction, mode, step, numberOfSteps); 
            }
             turn = false; 
        }
    }
}

CY_ISR(TC_handler) // drives the motor forward 
{
    newStep = true;
    step++;
    if (step > 7) // if it excedes 7, roll to 0
    {
         step = 0;
    }

}


CY_ISR(UART_rx_handler) // handels the inputs, sets flags and updates states. 
{
    uint8_t numberOfBytes = UART_GetRxBufferSize();
    for (uint8_t i = 0; i < numberOfBytes; i++)
    {
        char c = UART_GetChar();
        
        // Mainly sets flags for main to handle
        switch (c)
        {
             // change the drive mode to wave-drive
            case 'w' :             
            {
                
                UART_PutString("mode is Wave \r\n");
                mode = waveDrive;
                
            }
            break;
            // change the drive mode to full-step
            case 'f' :              
            {
                UART_PutString("mode is full \r\n");
                mode = fullStep;
            }
            break;
            // change the drive mode to half-step
            case 'h' :             
            {
                UART_PutString("mode is half \r\n");
                mode = halfStep;
            }
            break;
            // Driving forwards  
            case 'u' :              
            {
                
                UART_PutString("driving forwards\r\n");
                
                direction = 0;            
            }
            break;
            // drive backwards 
            case 'd' :              
            {
                UART_PutString("driving backwards\r\n");
                direction = 1;
            }
            break;
            // increase speed
            case '+' :              
            {
                
                UART_PutString("speeding up\r\n");
                speeding = true;            
            }
            break;
            // decrease speed
            case '-' :              
            {
                UART_PutString("slowing down\r\n");
                slowing = true;
                
            }
            break;
            // stops the moor  
            case 's' :              
            {
                UART_PutString("motor stopped\r\n");
                stoppingMotor = true;
                
            }
            break;
            // stats the motor  
            case 'b' :              
            {
                UART_PutString("motor started\r\n");
                startingMotor = true;
                
            }
            break;
            // take one step 
            case '1' :              
            {
                UART_PutString("turning 1 step\r\n");
                numberOfSteps = 1;
                turn = true;
                
            }
            break;
            // take 48 steps 
            case '9' :              
            {
                UART_PutString("turning 9 degrees\r\n");
                numberOfSteps = 48;
                turn = true;
            }
            break;
            default:                
            {
            }
            break;
        }
    }
} // end of uart handeler



/* [] END OF FILE */
