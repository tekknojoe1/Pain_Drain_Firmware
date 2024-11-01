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
#include <math.h>
#include "tens.h"

#define TENS_TIMER_INTERVAL_MS 10
#define TENS_INTERVAL_MS 100
#define MAX_TENS_PWM_VALUE 550

#define TENS_TIMER_20MS_INTERVAL_MS 20
#define TENS_TIMER_10MS_INTERVAL_MS 10

#define TENS_MODE_1_DUR 284
#define TENS_MODE_2_DUR 490

static int32 tens_one_timeout = -1;
static int32 tens_two_timeout = -1;
static int32 tens_one_interval_ms = TENS_TIMER_20MS_INTERVAL_MS;
static int32 tens_two_interval_ms = TENS_TIMER_20MS_INTERVAL_MS;
static int tens_one_timeout_event = 0;
static int tens_two_timeout_event = 0;
static int32 tens_one_counter = 0;
static int32 tens_two_counter = 0;
static int32 tens_amplitude = 0;
static int32 tens_phase_shift = 0;
static int32 tens_one_mode = 1;
static int32 tens_two_mode = 1;
static int32 tens_one_on = 0;
static int32 tens_two_on = 0;


#define TENS_IDLE 0
#define TENS_FRONT_PORCH 1
#define TENS_RAMP_UP 2
#define TENS_HOLD 3
#define TENS_RAMP_DN 4
#define TENS_OFF 5
#define TENS_BACK_PORCH 6
static int tens_one_state = 0;
static int tens_two_state = 0;




static void set_channel_one(void);

void tens_init(void) {
   
    
}

void tens_timer(void){
    if (tens_one_timeout > 0) {
        tens_one_timeout -= tens_one_interval_ms;
        //DBG_PRINTF("tens_timeout: %d \r\n", tens_timeout);
        if (tens_one_timeout < 0) {
            tens_one_timeout = 0;
            tens_one_timeout_event = 1;
        }
    }
    
    if (tens_two_timeout > 0) {
        tens_two_timeout -= tens_two_interval_ms;
        //DBG_PRINTF("tens_timeout: %d \r\n", tens_timeout);
        if (tens_two_timeout < 0) {
            tens_two_timeout = 0;
            tens_two_timeout_event = 1;
        }
    }
    
}

void tens_task( void ) {
    
    if (tens_amplitude == 0) {
        
        tens_one_state = TENS_IDLE;
        tens_two_state = TENS_IDLE;
        
        return;
    }
    

    //Channel 1
    if (tens_one_mode == 1) {
        //Channel 1 Mode 1 (20ms boundaries)
        tens_one_interval_ms = TENS_TIMER_20MS_INTERVAL_MS;
        //if (tens_one_timeout_event > 0) {
        //    tens_one_timeout_event = 0;
        
        switch (tens_one_state) {
            default: //idle
                tens_one_counter = 0;
                tens_one_state = TENS_RAMP_UP;
                tens_two_counter = 0;
                tens_two_state = TENS_IDLE;    //Make sure channel 2 is always in sync with channel 1
            break;
            
            case TENS_RAMP_UP:
                tens_one_counter += 10;    //Count up 10 ms
                tens_seq_chan_one(TENS_MODE_1_DUR, tens_one_counter, 1200);       //do a sequence
                if (tens_one_counter >= 1200) {
                    tens_one_counter = 0;
                    tens_one_state = TENS_HOLD;
                }
            break;
            case TENS_HOLD:
                tens_one_counter += 10;    //Count up 10 ms
                tens_seq_chan_one(TENS_MODE_1_DUR, 1200, 1200);       //do a sequence
                if (tens_one_counter >= 2800) {
                    tens_one_counter = 0;
                    tens_one_state = TENS_OFF;
                }
            break;
            case TENS_OFF:
                tens_one_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_one_counter >= 10000) {  //Off for 10 seconds
                    tens_one_counter = 0;
                    tens_one_state = TENS_IDLE;
                }
            break;
        }
        
    } else {
        //Channel 1 Mode 2 (10ms boundaries)
        tens_one_interval_ms = TENS_TIMER_10MS_INTERVAL_MS;
        //if (tens_one_timeout_event > 0) {
        //    tens_one_timeout_event = 0;
            
        switch (tens_one_state) {
            default: //idle
                tens_one_counter = 0;
                tens_one_state = TENS_RAMP_UP;
                tens_two_counter = 0;
                tens_two_state = TENS_IDLE;    //Make sure channel 2 is always in sync with channel 1
            break;
            case TENS_RAMP_UP:
                tens_one_counter += 10;    //Count up 10 ms
                tens_seq_chan_one(TENS_MODE_2_DUR, tens_one_counter, 1000);       //do a sequence
                if (tens_one_counter >= 1000) {
                    tens_one_counter = 0;
                    tens_one_state = TENS_HOLD;
                }
            break;
            case TENS_HOLD:
                tens_one_counter += 10;    //Count up 10 ms
                tens_seq_chan_one(TENS_MODE_2_DUR, 1000, 1000);       //do a sequence
                if (tens_one_counter >= 2200) {
                    tens_one_counter = 0;
                    tens_one_state = TENS_RAMP_DN;
                }
            break;
            case TENS_RAMP_DN:
                tens_one_counter += 10;    //Count up 10 ms
                tens_seq_chan_one(TENS_MODE_2_DUR, 800-tens_one_counter, 800);       //do a sequence
                if (tens_one_counter >= 800) {
                    tens_one_counter = 0;
                    tens_one_state = TENS_OFF;
                }
            break;
            case TENS_OFF:
                tens_one_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_one_counter >= 10000) {  //Off for 10 seconds
                    tens_one_counter = 0;
                    tens_one_state = TENS_IDLE;
                }
            break;
        }
    }
    
        
    //Channel 2 - behaves like channel 1, but can be 180 degrees out of phase
    if (tens_two_mode == 1) {
        //Channel 2 Mode 1 (20ms boundaries)
        tens_two_interval_ms = TENS_TIMER_20MS_INTERVAL_MS;
        //if (tens_two_timeout_event > 0) {
        //    tens_two_timeout_event = 0;
        
        switch (tens_two_state) {
            default: //idle
                tens_two_counter = 0;
                if (tens_phase_shift > 0) {
                    tens_two_state = TENS_FRONT_PORCH;
                } else {
                    tens_two_state = TENS_RAMP_UP;
                }
            break;
            case TENS_FRONT_PORCH:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 7000) {  //Off for 5 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_RAMP_UP;
                }
            break;
            case TENS_RAMP_UP:
                tens_two_counter += 10;    //Count up 10 ms
                tens_seq_chan_two(TENS_MODE_1_DUR, tens_two_counter, 1200);       //do a sequence
                if (tens_two_counter >= 1200) {
                    tens_two_counter = 0;
                    tens_two_state = TENS_HOLD;
                }
            break;
            case TENS_HOLD:
                tens_two_counter += 10;    //Count up 10 ms
                tens_seq_chan_two(TENS_MODE_1_DUR, 1200, 1200);       //do a sequence
                if (tens_two_counter >= 2800) {
                    tens_two_counter = 0;
                    if (tens_phase_shift > 0) {
                        tens_two_state = TENS_BACK_PORCH;
                    } else {
                        tens_two_state = TENS_OFF;
                    }
                }
            break;
            case TENS_OFF:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 10000) {  //Off for 10 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_IDLE;
                }
            break;
            case TENS_BACK_PORCH:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 3000) {  //Off for 5 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_IDLE;
                }
            break;
        }
        
    } else {
        //Channel 2 Mode 2 (10ms boundaries)
        tens_two_interval_ms = TENS_TIMER_10MS_INTERVAL_MS;
        //if (tens_two_timeout_event > 0) {
        //    tens_two_timeout_event = 0;
            
        switch (tens_two_state) {
            default: //idle
                tens_two_counter = 0;
                if (tens_phase_shift > 0) {
                    tens_two_state = TENS_FRONT_PORCH;
                } else {
                    tens_two_state = TENS_RAMP_UP;
                }
            break;
            case TENS_FRONT_PORCH:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 5000) {  //Off for 5 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_RAMP_UP;
                }
            break;
            case TENS_RAMP_UP:
                tens_two_counter += 10;    //Count up 10 ms
                tens_seq_chan_two(TENS_MODE_2_DUR, tens_two_counter, 1000);       //do a sequence
                if (tens_two_counter >= 1000) {
                    tens_two_counter = 0;
                    tens_two_state = TENS_HOLD;
                }
            break;
            case TENS_HOLD:
                tens_two_counter += 10;    //Count up 10 ms
                tens_seq_chan_two(TENS_MODE_2_DUR, 1000, 1000);       //do a sequence
                if (tens_two_counter >= 2200) {
                    tens_two_counter = 0;
                    tens_two_state = TENS_RAMP_DN;
                }
            break;
            case TENS_RAMP_DN:
                tens_two_counter += 10;    //Count up 10 ms
                tens_seq_chan_two(TENS_MODE_2_DUR, 800-tens_two_counter, 800);       //do a sequence
                if (tens_two_counter >= 800) {
                    tens_two_counter = 0;
                    if (tens_phase_shift > 0) {
                        tens_two_state = TENS_BACK_PORCH;
                    } else {
                        tens_two_state = TENS_OFF;
                    }
                }
            break;
            case TENS_OFF:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 10000) {  //Off for 10 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_IDLE;
                }
            break;
            case TENS_BACK_PORCH:
                tens_two_counter += 10;    //Count up 10 ms
                //No sequence
                if (tens_two_counter >= 5000) {  //Off for 5 seconds
                    tens_two_counter = 0;
                    tens_two_state = TENS_IDLE;
                }
            break;
        }
    }
   
}


void tens_seq_chan_one(int dur, int val, int max) {
    
    
    //On positive direction
    //DBG_PRINTF("Channel 1 On positive direction \r\n");
    //DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_ms);
    if (tens_one_on == 1) {
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    }
    //DBG_PRINTF("Channel 1 duration: %d\r\n", tens_one_dur_ms);
    CyDelayUs(dur);
    
    //Off state in between
    //DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
    
    CyDelayUs(dur);
    
    //On negative direction
    //DBG_PRINTF("Channel 1 On negative direction \r\n");
    if (tens_one_on == 1) {
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);
    }
    
    CyDelayUs(dur);
    
    //Off state in between
    //DBG_PRINTF("Channel 1 Off state in between \r\n");
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0); 
    
   
    
}

void tens_seq_chan_two(int dur, int val, int max) {
        
    //On positive direction
    //DBG_PRINTF("Channel 2 On positive direction \r\n");
    //DBG_PRINTF("Channel 2 duration: %d\r\n", tens_two_dur_ms);
    if (tens_two_on == 1) {
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
    }
    
    CyDelayUs(dur);
    
    //Off state in between
    //DBG_PRINTF("Channel 2 Off state in between \r\n");
    Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
    
    CyDelayUs(dur);
    
    //On negative direction
    //DBG_PRINTF("Channel 2 On negative direction \r\n");
    if (tens_two_on == 1) {
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 1);
    }
    
    CyDelayUs(dur);
    
    //Off state in between
    //DBG_PRINTF("Channel 2 Off state in between \r\n");
    Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 0);
    Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
    
    
}





void set_tens_mode(int channel, int mode) {
    if (channel == 1) {
        tens_one_mode = mode;
    } else if (channel == 2) {
        tens_two_mode = mode;
    }
}

void set_tens_amp (int amplitude) {
    DBG_PRINTF("setting amp\r\n");
    double comp;
    
    if (amplitude == 0) {
        DBG_PRINTF("Amp is 0\r\n");
        PWM_TENS_Disable();
        PWM_TENS2_Disable();
        
    } else if (tens_amplitude == 0 && amplitude > 0) {
        DBG_PRINTF("ten_amp local is 0 and amp is greater than 0\r\n");
        PWM_TENS_Start();
        CyDelayUs(25);
        PWM_TENS2_Start();
        
    }
    
    tens_amplitude = amplitude;  
    
    if (tens_amplitude > 0) {  
        DBG_PRINTF("amp is greater than 0\r\n");
        // Set TENS1 and wait before setting TENS2
        comp = 0.0089 * pow(tens_amplitude, 2.0287);
        if (comp < 0.0)
            comp = 0.0;
        else if (comp > 100.0)
            comp = 100.0;
        
        int scaled_pwm_value = (int)((comp * MAX_TENS_PWM_VALUE) / 100);
        DBG_PRINTF("pwm value: %d\r\n", scaled_pwm_value);
        
        PWM_TENS_SetCompare0(scaled_pwm_value);
        PWM_TENS2_SetCompare0(scaled_pwm_value);
        
    }
    
    
}

// Helper function to make setting a signal easier to channel 1 and 2
void set_tens_signal(int amplitude, int mode, int play_pause, int channel, int phase) {
    
    set_tens_amp(amplitude);
    
    if (channel == 1) {
        tens_one_mode = mode;
        tens_one_on = play_pause;        
    } else {
        tens_two_mode = mode;
        tens_two_on = play_pause;
    }
    
    tens_phase_shift = phase;
    
}





/* [] END OF FILE */