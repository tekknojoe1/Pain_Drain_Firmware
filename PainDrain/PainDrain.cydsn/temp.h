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
#ifndef TEMP_H
#define TEMP_H
#include <project.h>


void temp_init( void );
void set_temp(int value);
void set_fan(int value);

typedef struct {
    int8_t temp; // Signed to handle negative temperatures
} TemperatureSetting;

#endif

/* [] END OF FILE */

