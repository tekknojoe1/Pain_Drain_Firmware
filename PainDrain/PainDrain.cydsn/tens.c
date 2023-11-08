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

#define MAX_TENS_DUR_US 1200
#define MAX_TENS_INTERVAL_MS 1000
#define MAX_TENS_PWM_VALUE 312

static int32 tens_timeout = -1;
static int32 tens_full_timeout = 0;
static int32 tens_interval_ms = 0;
static int32 tens_one_dur_us = 0;
static int32 tens_two_dur_us = 0;
static int32 tens_phase_shift = 0;

static void set_channel_one(void);
void tens_timer(void){
    if (tens_timeout > 0) {
        tens_timeout -= TENS_TIMER_INTERVAL_MS;
       
        if (tens_timeout < 0) {
            tens_timeout = 0;
        }
    }
}

void set_channel_one(void) {
    //On positive direction
    DBG_PRINTF("Channel 1 On positive direction \r\n");
    
    // Channel 1
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);        
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    
    CyDelayUs(tens_one_dur_us);
    
    //Off state in between
    DBG_PRINTF("Channel 1 Off state in between \r\n");
    
    // Channel 1
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);  
}

void set_tens_task( void ) {
    
    if(tens_phase_shift > 0 && (tens_timeout == tens_full_timeout/2)){
        if(tens_one_dur_us > 0)
            set_channel_one();
    } else if (tens_phase_shift < 0 && tens_timeout == 0){
        if(tens_one_dur_us > 0)
            set_channel_one();
    }
    
    if (tens_timeout == 0) {
        if(tens_two_dur_us > 0){
        //On positive direction
        DBG_PRINTF("Channel 2 On positive direction \r\n");
            
        // Channel 2
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        
        CyDelayUs(tens_two_dur_us);
        
        //Off state in between
        DBG_PRINTF("Channel 2 Off state in between \r\n");
        
        // Channel 2
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        }
        
        if (tens_interval_ms > 0) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;
        }
    }
}

void set_tens_freq (int period) {
    tens_interval_ms = MAX_TENS_INTERVAL_MS / period;
    
    if (tens_interval_ms > 0) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;
    }
}

void set_tens_dur (int duration) {
    tens_one_dur_us = (duration * MAX_TENS_DUR_US) / 100;
}

void set_tens_amp (int amplitude) {

    if (amplitude == 0) {
        PWM_TENS_SetCompare0(0);
        PWM_TENS2_SetCompare0(0);
        //power_flags_update(UI_MENU_TENS_AMP, 0);      
        tens_interval_ms = -1; //Disable timer
        
    } else {                                                     
        int scaled_pwm_value = (amplitude * MAX_TENS_PWM_VALUE) / 100;
        
        // Set TENS1 and wait before setting TENS2
        PWM_TENS_SetCompare0(scaled_pwm_value);
        DBG_PRINTF("Tens 1 PWM: %d\r\n", PWM_TENS_GetCompare0());
        CyDelayUs((1000000 / tens_interval_ms) / 2); //Delay of 5-50uS
        
        // Set TENS2
        PWM_TENS2_SetCompare0(scaled_pwm_value);
        DBG_PRINTF("Tens 2 PWM: %d\r\n", PWM_TENS2_GetCompare0());
    }   
}


void set_tens_signal(int amplitude, int duration, int period, int channel, int phase) {
    
    // Amplitude will be the same for both channels.
    set_tens_amp(amplitude);
    set_tens_freq(period);
    tens_phase_shift = phase;
    
    if(channel == 1){   
        tens_one_dur_us = (duration * MAX_TENS_DUR_US) / 100;
    } else if(channel == 2){   
        tens_two_dur_us = (duration * MAX_TENS_DUR_US) / 100;
    } else {
        // disable the signal
        set_tens_amp(0);
    }   
}

/* [] END OF FILE */