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

#include "debug.h"
#include "temp.h"
#include "vibe.h"
#include "my_i2c.h"
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 




// Device address: 0x20
#define MA12070P_I2C_ADDR 0x20
#define PWM_PERIOD 100
//#define MAX_PWM_VALUE 62.5  // 62.5% of 624
#define MAX_PERCENTAGE .625
#define MIN_PERCENTAGE .16
#define FREQUENCY_PERIOD 2
#define PWM_OFF 0
#define VIBE_TIMER_CYCLES 120 // 120 cycles will be the same as 2 seconds

int off_time = 0;
int timer_cycles = 0;
int on_time = 0;
double motor_speed = 0.0; 
int max_PWM = 0;
int min_PWM = 0;
bool is_vibe_on = false;
int vibe_intensity = 0;


void vibe_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    int status;
    int i;
    
    myI2C_I2CMasterClearStatus();
    
    status = myI2C_I2CMasterSendStart(MA12070P_I2C_ADDR, 0);
    //DBG_PRINTF("Status check 1: %d \r\n", status);
    status = myI2C_I2CMasterWriteByte(reg);
    //DBG_PRINTF("Status check 2: %d \r\n", status);
    status = myI2C_I2CMasterSendRestart(MA12070P_I2C_ADDR, 1);
    //DBG_PRINTF("Status check 3: %d \r\n", status);
    
    for (i=0;i<num_regs-1;i++) {
        d[i] = myI2C_I2CMasterReadByte(1);
    }
    d[num_regs-1] = myI2C_I2CMasterReadByte(0);
    
    myI2C_I2CMasterSendStop();   
}

void set_vibe(int intensity, int frequency){
    vibe_intensity = intensity;
    on_time = ((double)intensity / 100.0) * VIBE_TIMER_CYCLES; // Calculates the on_time of the motor
    off_time = VIBE_TIMER_CYCLES - on_time;                  // Calculates the off_time of the motor
    DBG_PRINTF("on time: %d off time: %d\r\n", on_time, off_time);
    
    
    uint32_t pwm_period = PWM_VIBE_GetPeriod0();
    if(pwm_period != PWM_PERIOD){
        PWM_VIBE_SetPeriod0(PWM_PERIOD);
    }
    max_PWM = pwm_period * MAX_PERCENTAGE;
    min_PWM = pwm_period * MIN_PERCENTAGE;
  
    double scaled_motor_value = (frequency / 100.0) * (max_PWM - min_PWM) + min_PWM; // Calculates what the pwm value should be relative to the slider value received
    //DBG_PRINTF("frequency: %d maxPWM: %d minPWM: %d Scaled frequency: %d\r\n", frequency, maxPWM, minPWM, (int)scaled_motor_value);
    
    if(scaled_motor_value >= max_PWM){
        DBG_PRINTF("Over max\r\n");
        motor_speed = max_PWM;
    } else if(scaled_motor_value <= min_PWM){
        motor_speed = 0;
    } else{
        DBG_PRINTF("not above or below\r\n");
        motor_speed = scaled_motor_value;
    }
    
}

void vibe_task( void ){
    if(timer_cycles >= VIBE_TIMER_CYCLES){
        timer_cycles = 0;   
    }
    if(vibe_intensity == 100){
        PWM_VIBE_SetCompare0(motor_speed);
    } else if (vibe_intensity > 0 &&  motor_speed > min_PWM){          
        if(timer_cycles <= on_time ){
            PWM_VIBE_SetCompare0(motor_speed);           
        } else {
            PWM_VIBE_SetCompare0(PWM_OFF);
        }
        timer_cycles++;
    } else{
        PWM_VIBE_SetCompare0(PWM_OFF);
    }
}


/*
#include "cy_dma.h"
#include "DMA_Play.h"

// Size of the recorded buffer 
//#define BUFFER_SIZE     32768u
#define BUFFER_SIZE     2000u
// Number of channels (Stereo) 
#define NUM_CHANNELS    2u

// Array containing the recorded data (stereo) 
int16_t recorded_data[NUM_CHANNELS][BUFFER_SIZE];

// Number of elements recorded by the PDM/PCM 
uint32_t pdm_count = 0;

bool isVibeOn = false;


// Device address: 0x20
#define MA12070P_I2C_ADDR 0x20

void vibe_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    int status;
    int i;
    
    myI2C_I2CMasterClearStatus();
    
    status = myI2C_I2CMasterSendStart(MA12070P_I2C_ADDR, 0);
    DBG_PRINTF("Status check 1: %d \r\n", status);
    status = myI2C_I2CMasterWriteByte(reg);
    DBG_PRINTF("Status check 2: %d \r\n", status);
    status = myI2C_I2CMasterSendRestart(MA12070P_I2C_ADDR, 1);
    DBG_PRINTF("Status check 3: %d \r\n", status);
    
    for (i=0;i<num_regs-1;i++) {
        d[i] = myI2C_I2CMasterReadByte(1);
    }
    d[num_regs-1] = myI2C_I2CMasterReadByte(0);
    
    myI2C_I2CMasterSendStop();   
}

// Function to play recorded data
void PlayRecordedData(void)
{
    if(isVibeOn){
        // Start playing the recorded data by enabling the DMAs 
        //Cy_DMA_Channel_Enable(DMA_Play_HW, DMA_Play_DW_CHANNEL);
        //DBG_PRINTF("LOOPING\r\n");
    }
    else{
        //Cy_DMA_Channel_Disable(DMA_Play_HW, DMA_Play_DW_CHANNEL);   
    }

}

// Generate a sine wave
/*
void generateSineWave(){
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        // Assuming linear vibration response to sine wave
        float time_in_seconds = (float)i / (float)BUFFER_SIZE;
        float angle = 2.0 * M_PI * frequency * time_in_seconds;
        recorded_data[channel][i] = (int16_t)(amplitude * sin(angle));
    }
}
*/

/*
void vibe_init(void){
    // Initialize the DMA for playback (DMA_PLAY) and its descriptor addresses 
    //DMA_Play_Init();
    //Cy_DMA_Descriptor_SetYloopDataCount(&DMA_Play_SRAM_to_I2S, BUFFER_SIZE * NUM_CHANNELS / 256);
    //Cy_DMA_Descriptor_SetSrcAddress(&DMA_Play_SRAM_to_I2S, (void *)&recorded_data[0]);
    //Cy_DMA_Descriptor_SetDstAddress(&DMA_Play_SRAM_to_I2S, (void *)&I2S_HW->TX_FIFO_WR);
}


void set_vibe(const char* waveType, int amplitude, int frequency, int waveform){
    
    int channel = 0;
    if(amplitude > 0){
       isVibeOn = true;
    }
    else {
       isVibeOn = false;   
    }
    if(strcmp(waveType, "sine") == 0){
        // Calculate the number of samples per cycle
        int samples_per_cycle = BUFFER_SIZE / frequency;
        // Generate sine waveform data and add it to the array
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            // Assuming linear vibration response to sine wave
            float time_in_seconds = (float)i / (float)BUFFER_SIZE;
            float angle = 2.0 * M_PI * frequency * time_in_seconds;
            recorded_data[channel][i] = (int16_t)(amplitude * sin(angle));
        }
     DBG_PRINTF("Sine true \r\n");   
    }
    else if(strcmp(waveType, "square") == 0){
     DBG_PRINTF("square true \r\n");   
    }
    else if(strcmp(waveType, "triangle") == 0){
     DBG_PRINTF("triangle true \r\n");   
    }
    else if(strcmp(waveType, "sawtooth") == 0){
     DBG_PRINTF("sawtooth true \r\n");   
    }
    else{
     DBG_PRINTF("ERROR\r\n");   
    }
}
*/

/* [] END OF FILE */