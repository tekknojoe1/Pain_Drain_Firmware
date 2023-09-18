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
#include <stdlib.h>
#include "debug.h"

#define TENS_TIMER_INTERVAL_MS 10

static int32 tens_timeout = -1;
static int32 tens_interval_ms = 0;
static int32 tens_dur_us = 0;

void tens_timer(void){
    if (tens_timeout > 0) {
        tens_timeout -= TENS_TIMER_INTERVAL_MS;
        
        if (tens_timeout < 0) {
            tens_timeout = 0;
        }
    }
}

void set_tens_task( void ) {
    
    if (tens_timeout == 0) {
        
        //On positive direction
        DBG_PRINTF("On positive direction \r\n");
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        
        CyDelayUs(tens_dur_us);
        
        //Off state in between
        DBG_PRINTF("Off state in between \r\n");
        Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        CyDelayUs(tens_dur_us);
        
        //On negative direction
        DBG_PRINTF("On negative direction \r\n");
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 1);
        
        CyDelayUs(tens_dur_us);
        
        //Off state in between
        DBG_PRINTF("Off state in between \r\n");
        Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        if (tens_interval_ms > 0) 
            tens_timeout = tens_interval_ms;
    }
    
}

void set_tens_freq (int item) {
    
    switch (item) {
        default: tens_interval_ms = -1; break;   
        case 1: tens_interval_ms = 1000; break;
        case 2: tens_interval_ms = 500; break;
        case 3: tens_interval_ms = 333; break;
        case 4: tens_interval_ms = 250; break;
        case 5: tens_interval_ms = 200; break;
        case 6: tens_interval_ms = 167; break;
        case 7: tens_interval_ms = 143; break;
        case 8: tens_interval_ms = 125; break;
        case 9: tens_interval_ms = 111; break;
        case 10: tens_interval_ms = 100; break;
    }
    
    if (tens_interval_ms > 0) 
        tens_timeout = tens_interval_ms;
}

void set_tens_dur (int item) {
    
    switch (item) {
        default: tens_dur_us = 0; break;   
        case 1: tens_dur_us = 300; break;
        case 2: tens_dur_us = 400; break;
        case 3: tens_dur_us = 500; break;
        case 4: tens_dur_us = 600; break;
        case 5: tens_dur_us = 700; break;
        case 6: tens_dur_us = 800; break;
        case 7: tens_dur_us = 900; break;
        case 8: tens_dur_us = 1000; break;
        case 9: tens_dur_us = 1100; break;
        case 10: tens_dur_us = 1200; break;
    }
}

void set_tens_amp (int item) {

    //300 us on, 300 us off and 300 us in reverse direction
       
    if (item == 0) {
        //power_5v_off();
        PWM_TENS_SetCompare0(0);
        PWM_TENS2_SetCompare0(0);
        //power_flags_update(UI_MENU_TENS_AMP, 0);
        
        tens_interval_ms = -1; //Disable timer
        
    } else {
        //power_5v_on();
        //PWM_TENS_SetCompare0(257 * item/ 10);
        //PWM_TENS_SetCompare0(624 * item / 10);
        //power_flags_update(UI_MENU_TENS_AMP, 1);
        
        set_tens_dur(item); 
        
        set_tens_freq(item); //Enable tens timer
        
        switch (item) {
            case 1 ... 10: {
            int percent = item * 10; // Calculate the percentage, currently limiting the max to 50 perecent for testing

            // Calculate the scaled_pwm_value
            int scaled_pwm_value = (percent * 312) / 100;
            
            // Set TENS1 and wait before setting TENS2
            PWM_TENS_SetCompare0(scaled_pwm_value);
            DBG_PRINTF("Tens 1: %d\r\n", scaled_pwm_value);
            CyDelayUs((1000000 / tens_interval_ms) / 2); //Delay of 5-50uS
            
            // Set TENS2
            PWM_TENS2_SetCompare0(scaled_pwm_value);
            DBG_PRINTF("Tens 2: %d\r\n", scaled_pwm_value);
      
            break;
            
            }
            default:
            PWM_TENS_SetCompare0(0);
            PWM_TENS2_SetCompare0(0);
            break;
        }       
    }   
}
/* [] END OF FILE */
