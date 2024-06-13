/** 
 * Manages power and charging state machine

 */


#include <project.h>

#define POWER_DIAG_DATA_SIZE 11

#define POWER_FLAG_BLE 31
#define POWER_FLAG_CHG 30
#define LED_ON                          (0u)
#define LED_OFF                         (1u)
//#define BATTERY_CHARGING                (0u)
//#define BATTERY_NOT_CHARGING            (1u)
//#define BATTERY_FULLY_CHARGED           (2u)
//#define BATTERY_LOW                     (3u)
//#define BATTERY_NORMAL_OPERATION        (4u)

// Define the enum for battery status
typedef enum {
    NOT_CHARGING,
    CHARGING,
    FULLY_CHARGED,
    LOW_BATTERY,
    MEDIUM_BATTERY,
    NORMAL_OPERATION,
    WARNING
} DeviceStatus;

// Declare an external variable for battery status
extern DeviceStatus device_status;

void power_init( void );
void power_task( void );
void power_timer( void );
void power_wakeup( void );
void power_i2c_read_reg(uint8_t deviceAddr, uint8_t reg, uint8_t* d, int num_regs);
void power_i2c_write_reg(uint8_t deviceAddr, uint8_t reg, uint8_t d);
void power_get_diag_data(uint8 d[]);
bool send_data_to_phone(uint8_t* value, uint16_t length, uint8_t characteristic);
void check_charger();
void shut_off_all_stimuli();
void power_flags_update(int flag, int state);
void power_5v_on( void );
void power_5v_off( void );


int I2C_SCL_Read( void );
int I2C_SDA_Read( void );
void I2C_SCL_Write(int state);
void I2C_SDA_Write(int state);
void UpdateLedState(void);
void power_led_off(void);
void power_led_charging(void);
void power_led_lowbatt(void);
void power_led_green(void);
void power_led_ble(void);
void power_led_advertising(void);
void power_led_connected(void);
void reset_timer_cycles(void);

uint16_t reg_array[38];