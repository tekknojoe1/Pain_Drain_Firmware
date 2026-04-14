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

#include "temp.h"
#include "debug.h"
#include <stdio.h>
#include <project.h>
#include <stdlib.h>
#include <math.h>


#define MAX_PEL_PWM_VALUE 255 // limit to 100 percent of 255
#define MAX_FAN_PWM_VALUE 0 // limit to 62.5 percent of 255

// Thermistor parameters
#define THERMISTOR_R0           10000.0f    // Nominal resistance at 25C (10K)
#define THERMISTOR_T0           298.15f     // Nominal temperature in Kelvin (25C)
#define THERMISTOR_B            3984.0f     // B25/85 coefficient
#define IDAC_CURRENT_UA         100.0f      // IDAC current in microamps
#define ADC_VREF_MV             3300.0f     // ADC reference voltage in mV (VDDA = 3.3V)
#define ADC_RESOLUTION          4096.0f     // 12-bit ADC
#define ADC_SETTLE_MS           10          // Time to allow IDAC and ADC to settle in ms
 
// Temperature acquisition timer
// Timer fires every 10ms (matching tens_timer rate), so 100 counts = 1 second
#define TEMP_SAMPLE_INTERVAL    100         // 100 x 10ms = 1 second


static int set_fan_based_on_temp(int temp_value);
static float adc_counts_to_resistance(int16_t counts);
static float resistance_to_celsius(float resistance);
static float celsius_to_fahrenheit(float celsius);

static volatile int temp_acquire_flag = 0;  // Set by temp_timer, cleared by temp_task
static int32_t temp_timer_count = 0;        // Counts timer ticks
static float last_temp_celsius = 0.0f;      // Last measured temperature
static float last_temp_fahrenheit = 0.0f;

void temp_init(void) {
  
    PWM_PEL1_Start();
    PWM_PEL2_Start();
    
    PWM_PEL1_SetCompare0(0);
    PWM_PEL2_SetCompare0(0);
    
    // Initialize ADC and IDAC but leave powered down
    ADC_Start();
    IDAC7_Start();
 
    // Power down both until needed
    ADC_Stop();
    IDAC7_SetValue(0);
    IDAC7_Stop();
 
    DBG_PRINTF("temp_init complete\r\n");
}

void temp_timer(void) {
 
    temp_timer_count++;
 
    if (temp_timer_count >= TEMP_SAMPLE_INTERVAL) {
        temp_timer_count = 0;
        temp_acquire_flag = 1;
    }
}

/*******************************************************************************
* Function Name: temp_task
********************************************************************************
* Summary:
*   Called from the main loop. When the acquire flag is set, powers up the ADC
*   and IDAC, waits for settle, acquires the thermistor reading, converts to
*   temperature, prints result, then powers down ADC and IDAC again.
*******************************************************************************/
void temp_task(void) {
 
    if (!temp_acquire_flag) {
        return;
    }
    temp_acquire_flag = 0;
 
    // Power up IDAC and ADC
    IDAC7_Start();
    IDAC7_SetValue(100);    // Set to 100uA — verify this matches your IDAC7 range setting
    ADC_Start();
 
    // Allow IDAC and ADC to settle
    CyDelay(ADC_SETTLE_MS);
 
    // Trigger conversion and wait for result
    ADC_StartConvert();
    ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT);
    int16_t raw_counts = ADC_GetResult16(0);    // Channel 0
 
    // Power back down
    ADC_Stop();
    IDAC7_SetValue(0);
    IDAC7_Stop();
 
    // Convert counts to resistance
    float resistance = adc_counts_to_resistance(raw_counts);
 
    // Convert resistance to temperature
    last_temp_celsius    = resistance_to_celsius(resistance);
    last_temp_fahrenheit = celsius_to_fahrenheit(last_temp_celsius);
    
    int resistance_int = (int)resistance;
    int resistance_dec = (int)(resistance * 10) % 10;
    int temp_int = (int)last_temp_fahrenheit;
    int temp_dec = (int)(last_temp_fahrenheit * 10) % 10;
 
    DBG_PRINTF("Counts: %d res: %d.%d ohms temp: %d.%d F\r\n", 
           raw_counts, 
           resistance_int, resistance_dec,
           temp_int, temp_dec);
}

/*******************************************************************************
* Function Name: adc_counts_to_resistance
********************************************************************************
* Summary:
*   Converts raw ADC counts to thermistor resistance in ohms.
*   V = counts * (Vref / ADC_resolution)
*   R = V / I  where I = 100uA = 0.0001A
*******************************************************************************/
static float adc_counts_to_resistance(int16_t counts) {
 
    float voltage_mv = (counts / ADC_RESOLUTION) * ADC_VREF_MV;
    float voltage_v  = voltage_mv / 1000.0f;
    float current_a  = IDAC_CURRENT_UA / 1000000.0f;   // 100uA to amps
    float resistance = voltage_v / current_a;
 
    return resistance;
}
 
 
/*******************************************************************************
* Function Name: resistance_to_celsius
********************************************************************************
* Summary:
*   Converts thermistor resistance to temperature in Celsius using the
*   simplified B parameter Steinhart-Hart equation:
*   1/T = 1/T0 + (1/B) * ln(R/R0)
*******************************************************************************/
static float resistance_to_celsius(float resistance) {
 
    float ln_ratio   = logf(resistance / THERMISTOR_R0);
    float inv_temp   = (1.0f / THERMISTOR_T0) + (1.0f / THERMISTOR_B) * ln_ratio;
    float temp_k     = 1.0f / inv_temp;
    float temp_c     = temp_k - 273.15f;
 
    return temp_c;
}
 
 
/*******************************************************************************
* Function Name: celsius_to_fahrenheit
*******************************************************************************/
static float celsius_to_fahrenheit(float celsius) {
    return (celsius * 9.0f / 5.0f) + 32.0f;
}



void set_temp(int value){

    // Limit the value to [-100, 100]
    value = (value < -100) ? -100 : (value > 100) ? 100 : value;
    
    // Scale the value to the PWM range
    int scaled_pel_pwm = ( abs(value) * MAX_PEL_PWM_VALUE) / 100;
    DBG_PRINTF("scaled temp value %d\r\n", scaled_pel_pwm);
    DBG_PRINTF("temp value %d\r\n", value);
    
    if(value > 0){
        PWM_PEL1_SetCompare0(0);                //PEL1 off
        PWM_PEL2_SetCompare0(scaled_pel_pwm);   //PEL2 on
        set_fan(value);
    } else if (value < 0) {
        PWM_PEL2_SetCompare0(0);                //PEL2 off
        PWM_PEL1_SetCompare0(scaled_pel_pwm);   //PEL1 on
        set_fan(value);
    } else { 
        //DBG_PRINTF("Disable Temp\r\n");
        // Turn off both PELs to save power
        PWM_PEL1_SetCompare0(0);
        PWM_PEL2_SetCompare0(0);
        set_fan(0);
        //PWM_PEL1_Disable();
        //PWM_PEL2_Disable();
        //Cy_GPIO_Write(TEMP_USER_EN1_PORT, TEMP_USER_EN1_NUM, 0);  //Enable is low
        //DBG_PRINTF("Disabled PWM1 GetCompare: %d\r\n", PWM_PEL1_GetCompare0());
        //DBG_PRINTF("Disabled PWM2 GetCompare: %d\r\n", PWM_PEL2_GetCompare0());
        set_fan(0);
    } 
}

void set_fan(int value){
    
    // limit to 100
    value = ( abs(value) > 100) ? 100 : value;
    
    // scale the value to the PWM range
    int scaled_fan_pwm = ( abs(value) * MAX_FAN_PWM_VALUE) / 100;
    //DBG_PRINTF("Value: %d\r\n", value);
    //Update Fan
    if (value == 0) {
        //Turn fan off
        //DBG_PRINTF("PIN Low\r\n");
        Cy_GPIO_Write(FAN_EN_PORT, FAN_EN_NUM, 0);
    } else {
        //Adjust fan
        Cy_GPIO_Write(FAN_EN_PORT, FAN_EN_NUM, 1);
        //PWM_FAN_SetCompare0(scaled_fan_pwm);
    }
    //DBG_PRINTF("Fan value: %d\r\n", scaled_fan_pwm);
}

int set_fan_based_on_temp(int temp_value) {
    int adjusted_value;

    if (temp_value == 100) {
        adjusted_value = 100;
    } else if (temp_value > 80) {
        adjusted_value = 90;
    } else if (temp_value > 60) {
        adjusted_value = 70;
    } else if (temp_value > 40) {
        adjusted_value = 50;
    } else if (temp_value > 20) {
        adjusted_value = 30;
    } else{
        adjusted_value = 20;
    }
    
    return adjusted_value;
}
/* [] END OF FILE */
