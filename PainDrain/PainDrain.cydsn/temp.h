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
void temp_timer( void );
void temp_task( void );
void set_pel_pwm(int value);
void set_fan(int value);
void set_target_temperature_c(float desired_c);
void set_target_temperature_f(float desired_f);
void temp_disable_heating(void);
void temp_set_direct_pwm(int pwm);
void temp_set_power_level(int user_level);

typedef struct {
    int8_t temp; // Signed to handle negative temperatures
} TemperatureSetting;

#endif

/* [] END OF FILE */

