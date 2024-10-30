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

void tens_timer(void);
void tens_task(void);
void set_tens_amp(int amplitude);
void set_tens_mode(int channel, int mode); 


void tens_seq_chan_one(int dur, int val, int max);
void tens_seq_chan_two(int dur, int val, int max);

void set_tens_signal(int amplitude, int mode, int play_pause, int channel, int phase);

#endif /* TENS_H */
/* [] END OF FILE */
