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
#include <project.h>
#include <stdio.h>
#include "debug.h"
#include "temp.h"

#define MAX_PWM_VALUE 204 // limit to 80 percent of 255

void set_temp(int value){

    // Limit the value to [-100, 100]
    if (value < -100) {
        value = -100;
    } else if (value > 100) {
        value = 100;
    }

    // Scale the value to the PWM range
    int scaled_pwm = (value * MAX_PWM_VALUE) / 100;
    
    if(scaled_pwm > 0){
        // Turn off PEL2
        PWM_PEL2_SetCompare0(0);
        PWM_PEL2_Disable();
        
        // Turn on PEL1
        PWM_PEL1_Enable();
        PWM_PEL1_SetCompare0(scaled_pwm);
        
        // Testing
        DBG_PRINTF("PWM_PEL1_STATUS: %d\r\n", PWM_PEL1_GetStatus());
        DBG_PRINTF("PWM_PEL1_GETCOMPARE: %d\r\n", PWM_PEL1_GetCompare0());
        
    } else if (scaled_pwm < 0) {
        // Turn off PEL1
        PWM_PEL1_SetCompare0(0);
        PWM_PEL1_Disable();
        
        // Turn on PEL2
        PWM_PEL2_Enable();
        PWM_PEL2_SetCompare0(-scaled_pwm);
        
        // Testing
        DBG_PRINTF("PWM_PEL2_STATUS: %d\r\n", PWM_PEL2_GetStatus());
        DBG_PRINTF("PWM_PEL2_GETCOMPARE: %d\r\n", PWM_PEL2_GetCompare0());
    } else { 
        // Turn off both PELs to save power
        PWM_PEL1_SetCompare0(0);
        PWM_PEL2_SetCompare0(0);
        
        // Testing
        DBG_PRINTF("PWM_PEL1_STATUS: %d\r\n", PWM_PEL1_GetStatus());
        DBG_PRINTF("PWM_PEL2_STATUS: %d\r\n", PWM_PEL1_GetStatus());
        
        DBG_PRINTF("PWM_PEL1_GETCOMPARE: %d\r\n", PWM_PEL1_GetCompare0());
        DBG_PRINTF("PWM_PEL2_GETCOMPARE: %d\r\n", PWM_PEL2_GetCompare0());
        
        PWM_PEL1_Disable();
        PWM_PEL2_Disable();
        

    } 
}
/* [] END OF FILE */
