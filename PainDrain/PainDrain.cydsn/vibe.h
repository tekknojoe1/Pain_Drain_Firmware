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
#include <project.h>
    
void vibe_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs);

#include <project.h>
    
void set_vibe_task(void);
void vibe_init(void);
void set_vibe(int intensity, int frequency);
void vibe_task( void );

void vibe_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs);

typedef struct {
    uint8_t intensity;
} VibrationSetting;

#endif
/* [] END OF FILE */
