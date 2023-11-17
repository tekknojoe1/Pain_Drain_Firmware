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
void set_tens_task(void);
void set_tens_freq(double period);
void set_tens_dur(int duration, int tens_dur_us);
void set_tens_amp(int amplitude);
void set_tens_signal(int amplitude, double duration, double period, int channel, int phase);

#endif /* TENS_H */
/* [] END OF FILE */
