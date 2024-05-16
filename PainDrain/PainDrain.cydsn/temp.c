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

#include "temp.h"
#include "debug.h"
#include <stdio.h>
#include <project.h>
#include <stdlib.h>
#include "debug.h"


#define MAX_PEL_PWM_VALUE 204 // limit to 80 percent of 255
#define MAX_FAN_PWM_VALUE 80 // limit to 62.5 percent of 255

static int set_fan_based_on_temp(int temp_value);

void temp_init(void) {
  
    PWM_PEL1_Start();
    PWM_PEL2_Start();
    
    PWM_PEL1_SetCompare0(0);
    PWM_PEL2_SetCompare0(0);
}

void set_temp(int value){

    // Limit the value to [-100, 100]
    value = (value < -100) ? -100 : (value > 100) ? 100 : value;
    
    // Scale the value to the PWM range
    int scaled_pel_pwm = ( abs(value) * MAX_PEL_PWM_VALUE) / 100;
    
    if(value > 0){
        // Turn off PEL2
        PWM_PEL2_SetCompare0(0);
        //PWM_PEL2_Disable();
        
        // Turn on PEL1
        //Cy_GPIO_Write(TEMP_USER_EN1_PORT, TEMP_USER_EN1_NUM, 1);  //Enable is high
        PWM_PEL1_SetCompare0(scaled_pel_pwm);
        DBG_PRINTF("PWM1 Value: %d\r\n", scaled_pel_pwm);
        DBG_PRINTF("PWM1 GetCompare: %d\r\n", PWM_PEL1_GetCompare0());
        //set_fan(set_fan_based_on_temp(abs(value)));
        set_fan(value);
    } else if (value < 0) {
        // Turn off PEL1
        PWM_PEL1_SetCompare0(0);
        //PWM_PEL1_Disable();
        
        // Turn on PEL2
        //Cy_GPIO_Write(TEMP_USER_EN1_PORT, TEMP_USER_EN1_NUM, 1);  //Enable is high
        PWM_PEL2_SetCompare0(scaled_pel_pwm);
        DBG_PRINTF("PWM2 Value: %d\r\n", scaled_pel_pwm);
        DBG_PRINTF("PWM2 GetCompare: %d\r\n", PWM_PEL2_GetCompare0());
       // set_fan(set_fan_based_on_temp(abs(value)));
        set_fan(value);
    } else { 
        // Turn off both PELs to save power
        PWM_PEL1_SetCompare0(0);
        PWM_PEL2_SetCompare0(0);
        set_fan(0);
        //PWM_PEL1_Disable();
        //PWM_PEL2_Disable();
        //Cy_GPIO_Write(TEMP_USER_EN1_PORT, TEMP_USER_EN1_NUM, 0);  //Enable is low
        DBG_PRINTF("Disabled PWM1 GetCompare: %d\r\n", PWM_PEL1_GetCompare0());
        DBG_PRINTF("Disabled PWM2 GetCompare: %d\r\n", PWM_PEL2_GetCompare0());
        set_fan(0);
    } 
}

void set_fan(int value){
    
    // limit to 100
    value = ( abs(value) > 100) ? 100 : value;
    
    // scale the value to the PWM range
    int scaled_fan_pwm = ( abs(value) * MAX_FAN_PWM_VALUE) / 100;
    
    //Update Fan
    if (value == 0) {
        //Turn fan off
        Cy_GPIO_Write(FAN_EN_PORT, FAN_EN_NUM, 0);
    } else {
        //Adjust fan
        Cy_GPIO_Write(FAN_EN_PORT, FAN_EN_NUM, 1);
        //PWM_FAN_SetCompare0(scaled_fan_pwm);
    }
    DBG_PRINTF("Fan value: %d\r\n", scaled_fan_pwm);
}

int set_fan_based_on_temp(int temp_value) {
    int adjusted_value;

    if (temp_value == 100) {
        adjusted_value = 100;
    } else if (temp_value > 80) {
        adjusted_value = 90;
    } else if (temp_value > 60) {
        adjusted_value = 70;
    } else if (temp_value > 40) {
        adjusted_value = 50;
    } else if (temp_value > 20) {
        adjusted_value = 30;
    } else{
        adjusted_value = 20;
    }
    
    return adjusted_value;
}
/* [] END OF FILE */
