/** 
 * Manages power and charging state machine

 */


#include <project.h>

#define POWER_DIAG_DATA_SIZE 11

#define POWER_FLAG_BLE 31
#define POWER_FLAG_CHG 30

void power_init( void );
void power_task( void );
void power_timer( void );
void power_wakeup( void );
void power_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs);
void power_i2c_write_reg(uint8_t reg, uint8_t d);
void power_get_diag_data(uint8 d[]);
uint32_t get_charging_status();
bool send_data_to_phone(uint8_t* value, uint16_t length);
void power_flags_update(int flag, int state);
void power_5v_on( void );
void power_5v_off( void );


int I2C_SCL_Read( void );
int I2C_SDA_Read( void );
void I2C_SCL_Write(int state);
void I2C_SDA_Write(int state);
