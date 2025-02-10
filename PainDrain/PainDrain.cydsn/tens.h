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
#ifndef TENS_H
#define TENS_H
#include <project.h>

void tens_timer(void);
void tens_task(void);
void set_tens_amp(int amplitude);
void set_tens_mode(int channel, int mode); 


void tens_seq_chan_one(int dur, int val, int max);
void tens_seq_chan_two(int dur, int val, int max);

void set_tens_signal(int amplitude, int mode, int play_pause, int channel, int phase);

// Structs for stimuli

/*
Packet information contains
1: char T - TENS
2: int Amplitude
3: int Mode
4: int Play/Pause
5: int Channel
6: int Phase
*/
typedef struct {
    uint8_t intensity;
    uint8_t mode;
    uint8_t play;
    uint8_t channel;
    uint8_t phase;
} TensSetting;

#endif /* TENS_H */
/* [] END OF FILE */
