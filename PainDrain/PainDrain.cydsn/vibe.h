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
#ifndef VIBE_H
#define VIBE_H

void set_vibe_task(void);
void vibe_init(void);
void set_vibe(const char* waveType, int amplitude, int frequency, int waveform);

#endif
/* [] END OF FILE */
