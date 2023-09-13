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
#include <stdlib.h>
#include "debug.h"
#include "temp.h"

<<<<<<< HEAD
#define MAX_PWM_VALUE 204 // limit to 80 percent of 255
=======
#include "temp.h"
#include <stdio.h>
#include <project.h>
#include <stdlib.h>

#define MAX_PEL_PWM_VALUE 204 // limit to 80 percent of 255
#define MAX_FAN_PWM_VALUE 160 // limit to 62.5 percent of 255
>>>>>>> fd2816e499b7e6b93ab263ed5a40f24409255266

void set_temp(int value){

    // Limit the value to [-100, 100]
    value = (value < -100) ? -100 : (value > 100) ? 100 : value;
    
    // Scale the value to the PWM range
<<<<<<< HEAD
    int scaled_pwm = ( abs(value) * MAX_PWM_VALUE) / 100;
=======
    int scaled_pel_pwm = ( abs(value) * MAX_PEL_PWM_VALUE) / 100;
>>>>>>> fd2816e499b7e6b93ab263ed5a40f24409255266
    
    if(value > 0){
        // Turn off PEL2
        PWM_PEL2_SetCompare0(0);
        //PWM_PEL2_Disable();
        
        // Turn on PEL1
        //PWM_PEL1_Enable();
        Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);  //Enable is high
<<<<<<< HEAD
        PWM_PEL1_SetCompare0(scaled_pwm); 
=======
        PWM_PEL1_SetCompare0(scaled_pel_pwm); 
>>>>>>> fd2816e499b7e6b93ab263ed5a40f24409255266
    } else if (value < 0) {
        // Turn off PEL1
        PWM_PEL1_SetCompare0(0);
        //PWM_PEL1_Disable();
        
        // Turn on PEL2
        //PWM_PEL2_Enable();
        Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);  //Enable is high
<<<<<<< HEAD
        PWM_PEL2_SetCompare0(scaled_pwm);
=======
        PWM_PEL2_SetCompare0(scaled_pel_pwm);
>>>>>>> fd2816e499b7e6b93ab263ed5a40f24409255266
    } else { 
        // Turn off both PELs to save power
        PWM_PEL1_SetCompare0(0);
        PWM_PEL2_SetCompare0(0);
        
        //PWM_PEL1_Disable();
        //PWM_PEL2_Disable();
        Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 0);  //Enable is low
    } 
}
<<<<<<< HEAD
/* [] END OF FILE */
=======

void set_fan(int value){
    
    // limit to 100
    value = ( abs(value) > 100) ? 100 : value;
    
    // scale the value to the PWM range
    int scaled_fan_pwm = ( abs(value) * MAX_FAN_PWM_VALUE) / 100;
    
    //Update Fan
    if (value == 0) {
        //Turn fan off
        PWM_FAN_SetCompare0(0);
    } else {
        //Adjust fan
        PWM_FAN_SetCompare0(value);
    }
}
/* [] END OF FILE */
>>>>>>> fd2816e499b7e6b93ab263ed5a40f24409255266
