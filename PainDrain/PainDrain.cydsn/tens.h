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

void set_tens_task(void);
void set_tens_freq(int period);
void set_tens_dur(int duration);
void set_tens_amp(int amplitude);
void set_phase_shift(int phase_degrees);
void set_tens_signal(int channel, int tens_amplitude, int tens_frequency, int tens_duration, int phase);

#endif /* TENS_H */
/* [] END OF FILE */
