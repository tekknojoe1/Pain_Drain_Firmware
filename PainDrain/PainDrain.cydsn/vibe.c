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
#include <math.h> 

#include "vibe.h"
/*
#include "cy_dma.h"
#include "DMA_Play.h"
#include <stdio.h>
#include <project.h>
#include <stdlib.h>


 Size of the recorded buffer 
#define BUFFER_SIZE     32768u

 Number of channels (Stereo) 
#define NUM_CHANNELS    2u

 Array containing the recorded data (stereo) 
int16_t recorded_data[NUM_CHANNELS][BUFFER_SIZE];

 Number of elements recorded by the PDM/PCM 
uint32_t pdm_count = 0;


// Function to play recorded data
void PlayRecordedData(void)
{
     Initialize the DMA for playback (DMA_PLAY) and its descriptor addresses 
    DMA_Play_Init();
    Cy_DMA_Descriptor_SetYloopDataCount(&DMA_Play_SRAM_to_I2S, BUFFER_SIZE * NUM_CHANNELS / 256);
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_Play_SRAM_to_I2S, (void *)&recorded_data[0]);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_Play_SRAM_to_I2S, (void *)&I2S_HW->TX_FIFO_WR);

     Start playing the recorded data by enabling the DMAs 
    Cy_DMA_Channel_Enable(DMA_Play_HW, DMA_Play_DW_CHANNEL);
}

void set_vibe(const char* waveform, int freqeuncy, int amplitude){
    
    int channel = 0;

    // Using a sine waveform for vibrations
    if (strcmp(waveform, "sine") == 0) {
        // Calculate the number of samples per cycle
        int samples_per_cycle = BUFFER_SIZE / frequency;

        // Generate sine waveform data and add it to the array
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            // Assuming linear vibration response to sine wave
            float time_in_seconds = (float)i / (float)BUFFER_SIZE;
            float angle = 2.0 * M_PI * frequency * time_in_seconds;
            recorded_data[channel][i] = (int16_t)(amplitude * sin(angle));
        }
    }
}
*/

/* [] END OF FILE */
