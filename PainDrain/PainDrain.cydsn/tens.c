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
static int32 tens_interval_ms = 0;
static int32 tens_dur_us = 0;

static void set_phase_shift(int phase_degrees);

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

void set_tens_freq (int period) {
    tens_interval_ms = MAX_TENS_INTERVAL_MS / period;
    
    if (tens_interval_ms > 0) 
        tens_timeout = tens_interval_ms;
}

void set_tens_dur (int duration) {
    tens_dur_us = (duration * MAX_TENS_DUR_US) / 100;
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

void set_phase_shift(int phase_degrees) {   
   // Phase shift: PS = td/p * 360  -> td = (ps * p) / 360
   // Time delay for phase shit
   int tens_time_delay_ps = (phase_degrees * tens_interval_ms) / 360;
   CyDelay(tens_time_delay_ps);
}

void set_tens_signal(int amplitude, int duration, int period, int channel, int phase) {
    
    set_tens_amp(amplitude);
    
    if(channel == 1){       
        set_tens_freq(period);
        set_tens_dur(duration);
    } else if(channel == 2){
        set_phase_shift(phase);
        
        set_tens_freq(period);
        set_tens_dur(duration);
    } else {
        // disable the signal
        set_tens_amp(0);
    }   
}

/* [] END OF FILE */