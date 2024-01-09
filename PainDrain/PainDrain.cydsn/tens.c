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
static int32 tens_one_dur_ms = 0;
static int32 tens_two_dur_ms = 0;
static int32 tens_phase_shift = 0;

static void set_channel_one(void);

void tens_timer(void){
    if (tens_timeout > 0) {
        tens_timeout -= TENS_TIMER_INTERVAL_MS;
        //DBG_PRINTF("tens_timeout: %d \r\n", tens_timeout);
        if (tens_timeout < 0) {
            tens_timeout = 0;
            tens_full_timeout = tens_timeout;
        }
    }
}

void set_channel_one(void) {
    //On positive direction
    //DBG_PRINTF("Channel 1 On positive direction \r\n");
    //DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_ms);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    //DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_ms);
    CyDelay(tens_one_dur_ms);
    
    //Off state in between
    //DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
    
    CyDelay(tens_one_dur_ms);
    
    //On negative direction
    //DBG_PRINTF("Channel 1 On negative direction \r\n");
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);
    
    CyDelay(tens_one_dur_ms);
    
    //Off state in between
    //DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0); 
}

void set_tens_task( void ) {
    // Checks if there is a phase delay selected and executes at 50% of the tens_timer for 180 degrees shift
    if(tens_phase_shift > 0 && (tens_timeout == tens_full_timeout/2) && tens_one_dur_ms > 0){
        set_channel_one();
    } else if (tens_phase_shift <= 0 && tens_timeout == 0 && tens_one_dur_ms > 0){
        set_channel_one();
    }
    // Checks if Channel 2 is on, otherwise update the tens_timeout
    if (tens_timeout == 0 && tens_two_dur_ms > 0) {
        //On positive direction
        //DBG_PRINTF("Channel 2 On positive direction \r\n");
        //DBG_PRINTF("Channel 2 duration: %d\r\n", tens_two_dur_ms);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        
        CyDelay(tens_two_dur_ms);
        
        //Off state in between
        //DBG_PRINTF("Channel 2 Off state in between \r\n");
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        CyDelay(tens_two_dur_ms);
        
        //On negative direction
        //DBG_PRINTF("Channel 2 On negative direction \r\n");
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 1);
        
        CyDelay(tens_two_dur_ms);
        
        //Off state in between
        //DBG_PRINTF("Channel 2 Off state in between \r\n");
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
    } 
    if (tens_timeout == 0 && tens_interval_ms > 0 && (tens_one_dur_ms > 0 || tens_two_dur_ms > 0)) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;     
    }
}

void set_tens_freq (double period) {
    
    tens_interval_ms = (int) (period*TENS_INTERVAL_MS);
    
    //DBG_PRINTF("tens_interval_ms: %d \r\n", tens_interval_ms);
    if (tens_interval_ms > 0) {
        tens_timeout = tens_interval_ms;
        tens_full_timeout = tens_timeout;
    }
}


void set_tens_dur(int duration, int32 *tens_dur_ms){
    int tolerance = 30;
    switch (duration) {
        default: *tens_dur_ms = 0; break;   
        case 1: *tens_dur_ms = 100 - tolerance; break;
        case 2: *tens_dur_ms = 200 - tolerance; break;
        case 3: *tens_dur_ms = 300 - tolerance; break;
        case 4: *tens_dur_ms = 400 - tolerance; break;
        case 5: *tens_dur_ms = 500 - tolerance; break;
        case 6: *tens_dur_ms = 600 - tolerance; break;
        case 7: *tens_dur_ms = 700 - tolerance; break;
        case 8: *tens_dur_ms = 800 - tolerance; break;
        case 9: *tens_dur_ms = 900 - tolerance; break;
        case 10: *tens_dur_ms = 1000 - tolerance; break;
    }
}

void set_tens_amp (int amplitude) {

    if (amplitude == 0) {
        DBG_PRINTF("Amplitude is off at 0 %d \r\n");
        PWM_TENS_SetCompare0(0);
        PWM_TENS2_SetCompare0(0);
        //power_flags_update(UI_MENU_TENS_AMP, 0);      
        tens_interval_ms = -1; //Disable timer
        
    } else {           
        // Limiting PWM Value
        int scaled_pwm_value = (amplitude * MAX_TENS_PWM_VALUE) / 100;
        
        // Set TENS1 and wait before setting TENS2
        PWM_TENS_SetCompare0(scaled_pwm_value);
        //DBG_PRINTF("Tens 1 PWM: %d\r\n", PWM_TENS_GetCompare0());
        CyDelayUs((1000000 / tens_interval_ms) / 2); //Delay of 5-50uS
        
        // Set TENS2
        PWM_TENS2_SetCompare0(scaled_pwm_value);
        //DBG_PRINTF("Tens 2 PWM: %d\r\n", PWM_TENS2_GetCompare0());
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
        set_tens_dur(duration_case, &tens_one_dur_ms);
        //DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_ms);
    } else if(channel == 2){  
        set_tens_dur(duration_case, &tens_two_dur_ms); 
        //DBG_PRINTF("Channel 2 duration: %d\r\n", tens_two_dur_ms);
    } else {
        // disable the signal
        //set_tens_amp(0);
        //set_tens_freq(0);
        //set_tens_dur(0, &tens_one_dur_ms);
        //set_tens_dur(0, &tens_two_dur_ms);
    }   
}
/* [] END OF FILE */