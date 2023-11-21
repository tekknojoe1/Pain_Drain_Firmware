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
#define TENS_INTERVAL_MS 100
#define MAX_TENS_PWM_VALUE 312

static int32 tens_timeout = -1;
static int32 tens_full_timeout = 0; // Used for Phase Shift 180 Degrees
static int32 tens_interval_ms = 0;
static int32 tens_one_dur_us = 0;
static int32 tens_two_dur_us = 0;
static int32 tens_phase_shift = 0;

static void set_channel_one(void);

void tens_timer(void){
    if (tens_timeout > 0) {
        tens_timeout -= TENS_TIMER_INTERVAL_MS;
        DBG_PRINTF("tens_timeout: %d \r\n", tens_timeout);
        if (tens_timeout < 0) {
            tens_timeout = 0;
            tens_full_timeout = tens_timeout;
        }
    }
}

void set_channel_one(void) {
    /*
    //On positive direction
    DBG_PRINTF("Channel 1 On positive direction \r\n");
    
    //Enable channel 1
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);      
    
    // Channel 1 Positive Direction
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_0_NUM, 0);
    
    CyDelayUs(tens_one_dur_us);
    
    //Off state in between
    DBG_PRINTF("Channel 1 Off state in between \r\n");
    
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_0_NUM, 0);
    
    
    CyDelayUs(tens_one_dur_us);
    
    // Channel 1 Negative Direction
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_0_NUM, 1);
    
    CyDelayUs(tens_one_dur_us);
    
    //Disable channel and direction controls until next cycle
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);    
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_0_NUM, 0);
    */
    
    //On positive direction
    DBG_PRINTF("Channel 1 On positive direction \r\n");
    DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_us);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    
    CyDelayUs(tens_one_dur_us);
    
    //Off state in between
    DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
    
    CyDelayUs(tens_one_dur_us);
    
    //On negative direction
    DBG_PRINTF("Channel 1 On negative direction \r\n");
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);
    
    CyDelayUs(tens_one_dur_us);
    
    //Off state in between
    DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
}

void set_tens_task( void ) {
    // Checks if there is a phase delay selected and executes at 50% of the tens_timer for 180 degrees shift
    if(tens_phase_shift > 0 && (tens_timeout == tens_full_timeout/2) && tens_one_dur_us > 0){
        set_channel_one();
    } else if (tens_phase_shift <= 0 && tens_timeout == 0 && tens_one_dur_us > 0){
        set_channel_one();
    }
    // Checks if Channel 2 is on, otherwise update the tens_timeout
    if (tens_timeout == 0 && tens_two_dur_us > 0) {
        //On positive direction
        DBG_PRINTF("Channel 2 On positive direction \r\n");
        DBG_PRINTF("Channel 2 duration: %d\r\n", tens_two_dur_us);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        
        CyDelayUs(tens_one_dur_us);
        
        //Off state in between
        DBG_PRINTF("Channel 2 Off state in between \r\n");
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        CyDelayUs(tens_one_dur_us);
        
        //On negative direction
        DBG_PRINTF("Channel 2 On negative direction \r\n");
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 1);
        
        CyDelayUs(tens_one_dur_us);
        
        //Off state in between
        DBG_PRINTF("Channel 2 Off state in between \r\n");
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        /*
        //On positive direction
        DBG_PRINTF("Channel 2 On positive direction \r\n");
            
        //Enable channel 2
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);      
        
        // Channel 1 Positive Direction
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_0_NUM, 0);
        
        CyDelayUs(tens_one_dur_us);
        
        //Off state in between
        DBG_PRINTF("Channel 1 Off state in between \r\n");
        
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_0_NUM, 0);
        
        
        CyDelayUs(tens_one_dur_us);
        
        // Channel 1 Negative Direction
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_0_NUM, 1);
        
        CyDelayUs(tens_one_dur_us);
        
        //Disable channel and direction controls until next cycle
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);    
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_0_NUM, 0);
        */ 
    } 
    if (tens_timeout == 0 && tens_interval_ms > 0 && (tens_one_dur_us > 0 || tens_two_dur_us > 0)) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;     
    }
}

void set_tens_freq (double period) {
    
    tens_interval_ms = (int) (period*TENS_INTERVAL_MS);
    
    DBG_PRINTF("tens_interval_ms: %d \r\n", tens_interval_ms);
    if (tens_interval_ms > 0) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;
    }
}

void set_tens_dur(int duration, int32 *tens_dur_us){
    switch (duration) {
        default: *tens_dur_us = 0; break;   
        case 1: *tens_dur_us = 100; break;
        case 2: *tens_dur_us = 200; break;
        case 3: *tens_dur_us = 300; break;
        case 4: *tens_dur_us = 400; break;
        case 5: *tens_dur_us = 500; break;
        case 6: *tens_dur_us = 600; break;
        case 7: *tens_dur_us = 700; break;
        case 8: *tens_dur_us = 800; break;
        case 9: *tens_dur_us = 900; break;
        case 10: *tens_dur_us = 1000; break;
    }
}

void set_tens_amp (int amplitude) {

    if (amplitude == 0) {
        PWM_TENS_SetCompare0(0);
        PWM_TENS2_SetCompare0(0);
        //power_flags_update(UI_MENU_TENS_AMP, 0);      
        tens_interval_ms = -1; //Disable timer
    } else {           
        // Limiting PWM Value
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

// Helper function to make setting a signal easier to channel 1 and 2
void set_tens_signal(int amplitude, double duration, double period, int channel, int phase) {
    
    int duration_case = (int)(duration*10);
    tens_phase_shift = phase;
    
    if(channel == 0){
        // Amplitude and Period will be the same for both channels.
        set_tens_amp(amplitude);
        set_tens_freq(period);
    } else if(channel == 1){
        set_tens_dur(duration_case, &tens_one_dur_us);
    } else if(channel == 2){  
        set_tens_dur(duration_case, &tens_two_dur_us);        
    } else {
        // disable the signal
        //set_tens_amp(0);
        //set_tens_freq(0);
        //set_tens_dur(0, &tens_one_dur_us);
        //set_tens_dur(0, &tens_two_dur_us);
    }   
}
/* [] END OF FILE */