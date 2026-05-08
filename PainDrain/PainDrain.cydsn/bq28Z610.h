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


#define BQ28Z610_I2C_ADDR           0x55

// Standard command registers (16-bit, little-endian)
#define BQ28Z610_REG_TEMPERATURE    0x06  // 0.1 K units
#define BQ28Z610_REG_VOLTAGE        0x08  // mV
#define BQ28Z610_REG_FLAGS          0x0A
#define BQ28Z610_REG_REMAINING_CAP  0x10  // mAh
#define BQ28Z610_REG_FULL_CAP       0x12  // mAh
#define BQ28Z610_REG_AVG_CURRENT    0x14  // mA (signed)
#define BQ28Z610_REG_SOC            0x2C  // State of charge, 0-100 %
#define BQ28Z610_REG_SOH            0x2E  // State of health, 0-100 %

void bq28Z610_init( void );
void bq28Z610_write_reg(int reg, int d);
void bq28Z610_read_reg(uint8_t reg, uint8_t* d, int num_regs);
void bq28Z610_read_all_reg();
uint8_t  bq28Z610_get_soc(void);
uint16_t bq28Z610_get_voltage_mv(void);

/* [] END OF FILE */
