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
 * Driver for BQ28Z610-R1 Battery gauge
*/


#include <project.h>


#define BQ28Z610_I2C_ADDR                              0x55


void bq28Z610_init( void );
void bq28Z610_write_reg(int reg, int d);
void bq28Z610_read_reg(uint8_t reg, uint8_t* d, int num_regs);

/* [] END OF FILE */
