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
#include "power.h"
#include "bq28Z610.h"
#include <stdio.h>
#include <project.h>
#include <stdlib.h>
#include <math.h>


#define MAX_PEL_PWM_VALUE 255 // limit to 100 percent of 255
#define MAX_FAN_PWM_VALUE 0 // limit to 62.5 percent of 255

// Thermistor parameters
#define THERMISTOR_R0           10000.0f    // Nominal resistance at 25C (10K)
#define THERMISTOR_T0           298.15f     // Nominal temperature in Kelvin (25C)
#define THERMISTOR_B            2073.0f // changing temporarily until we get units with better thermal coupling 3984.0f     // B25/85 coefficient
#define IDAC_SETTING            20      // IDAC setting to get the desired current
#define IDAC_CURRENT_UA         ((float)IDAC_SETTING * 5)      // IDAC current in microamps
#define ADC_VREF_MV             3300.0f     // ADC reference voltage in mV (VDDA = 3.3V)
#define ADC_RESOLUTION          4096.0f     // 12-bit ADC (0-4095)
#define ADC_ZERO_OFFSET         2048        // Count at Vin=0V: Vneg for S/E = Vssa shifts midpoint to 0x800
#define ADC_SPAN                2048.0f     // Usable count span (0x800 to 0xFFF)
#define ADC_SETTLE_MS           20          // Time to allow IDAC and ADC to settle in ms
 
// Temperature acquisition timer
// Timer fires every 10ms (matching tens_timer rate), so 100 counts = 1 second
#define TEMP_SAMPLE_INTERVAL    100         // 100 x 10ms = 1 second
#define BATT_PRINT_INTERVAL      30         // Battery poll/print + BLE Battery Service update cadence (seconds)

typedef struct {
    float  target_c;   // Desired actual surface temperature (deg C)
    int    pwm;        // Feedforward PEL PWM value (0-100)
} TempPwmPoint_t;

// Re-calibrate after adding thermal paste.
static const TempPwmPoint_t temp_pwm_table[] = {
    {  23.2f,  0 },
    {  29.4f,  8 },
    {  35.0f, 15 },
    {  37.8f, 19 },
    {  40.6f, 22 },
    {  43.3f, 26 }
};
#define TEMP_PWM_POINTS  (sizeof(temp_pwm_table) / sizeof(temp_pwm_table[0]))

#define TEMP_MAX_ALLOWED_C         43.3f  // Hard safety limit — never exceed this
#define TEMP_ROOM_TEMP_C           25.2f  // Threshold: targets above this are heating, below are cooling
#define TEMP_ADJUST_RANGE_C         3.0f  // Open trim window when within this many deg C of target
#define TEMP_DEADBAND_C             1.0f  // Hold zone — no adjustment while inside (deg C)
#define TEMP_SETTLED_THRESHOLD_C    0.3f  // Max rate (deg C/sample) to be considered settled
#define TEMP_STALL_SAMPLES             5  // Consecutive settled samples = temperature stalled (~5 sec)
#define TEMP_PWM_TRIM_LIMIT            5  // Max PWM counts per single stall-triggered adjustment

// Cooling duty cycle: alternate PEL+fan and fan-only to prevent hot-side saturation
#define COOL_ON_SAMPLES            300     // Default seconds with PEL + fan running (overridden dynamically)
#define COOL_OFF_SAMPLES            30     // Default seconds fan-only recovery (overridden dynamically)
#define COOL_MAX_PWM               100     // Max magnitude for cooling PWM (prevents hot-side saturation)
#define COOL_IDLE_PWM                1     // Reduced cooling magnitude during hot-side recovery phase

#define HEAT_MAX_PWM                23     // Max magnitude for heating PWM (limits surface to ~110 F)
#define HEAT_LIMIT_SCALE_INTERVAL    5     // Seconds between PWM scale-back steps when at temp limit
#define HEAT_LIMIT_SCALE_UP_INTERVAL 10    // Seconds between PWM scale-up steps during limit recovery
#define HEAT_LIMIT_RECOVER_BELOW_C   1.5f  // Degrees C below max that triggers scale-up


static float adc_counts_to_resistance(int16_t counts);
static float resistance_to_celsius(float resistance);
static float celsius_to_fahrenheit(float celsius);
static float adc_counts_to_mvoltage(int16_t counts);
static float feedforward_slope(float temp_c);
static void  temp_control_update(void);
static void  cool_cycle_update(void);
static int32_t run_seconds_for_power(float power_level);
static int32_t estimated_recovery_seconds(float power_level);

static volatile int temp_acquire_flag = 0;  // Set by temp_timer, cleared by temp_task
static int32_t temp_timer_count = 0;        // Counts timer ticks
static float last_temp_celsius = 0.0f;      // Last measured temperature
static float last_temp_fahrenheit = 0.0f;

static float target_actual_c    = 0.0f;
static float prev_temp_c        = 0.0f;
static int   pel_pwm            = 0;
static int   pel_active         = 0;
static int   pel_mode           = 0;  // 1 = heating session, -1 = cooling session
static int   direct_pwm_mode    = 0;  // 1 = direct PWM set by user, trim bypassed
static int   trim_active        = 0;  // 1 = trim window open, 0 = holding feedforward PWM
static int   trim_base_pwm      = 0;  // PWM reference for bounding the current trim window
static int   stall_count        = 0;  // Consecutive settled samples (for stall detection)
static int32_t cool_sample_count = 0; // Samples elapsed in current cooling phase
static int     cool_pel_on       = 1; // 1 = PEL active, 0 = fan-only phase
static int32_t cool_on_limit     = COOL_ON_SAMPLES;  // Dynamic run time (seconds)
static int32_t cool_off_limit    = COOL_OFF_SAMPLES; // Dynamic recovery time (seconds)
static int32_t batt_print_count  = BATT_PRINT_INTERVAL; // Seconds elapsed since last battery level print
static int32_t heat_limit_count    = 0;  // Seconds elapsed since last over-limit PWM scale-back
static int32_t heat_scale_up_count = 0;  // Seconds elapsed since last under-limit PWM scale-up
static int     heat_limit_active   = 0;  // 1 = PWM was reduced by the temp limit
static int     heat_limit_user_pwm = 0;  // User's original requested PWM (ceiling for scale-up)

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
*   and IDAC, waits for settle, acquires the thermistor reading, powers down 
*   ADC and IDAC again, converts to temperature, then prints result.
*******************************************************************************/
void temp_task(void) {
 
    if (!temp_acquire_flag) {
        return;
    }
    temp_acquire_flag = 0;
 

	 //if ( temp_cnt == 0)
	 {
   // Power up IDAC and ADC
    IDAC7_Start();
    IDAC7_SetValue(IDAC_SETTING);    // DAC code 20 × 5uA/bit (635uA range) = 100uA = IDAC_CURRENT_UA
    ADC_Start();
	 }
 
    // Allow IDAC and ADC to settle
    CyDelay(ADC_SETTLE_MS);
 
    // Trigger conversion and wait for result
    ADC_StartConvert();
    ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT);
    int16_t raw_counts = ADC_GetResult16(0);    // Channel 0
 
	 //if( (temp_cnt++ > 1))
	 {
    // Power back down
    ADC_Stop();
    IDAC7_SetValue(0);
    IDAC7_Stop();
	 //temp_cnt = 0;
	 }

	 // Convert counts to resistance
    float resistance = adc_counts_to_resistance(raw_counts);
 
    // Convert resistance to temperature
    last_temp_celsius    = resistance_to_celsius(resistance);
    last_temp_fahrenheit = celsius_to_fahrenheit(last_temp_celsius);
    
    /* 
	 // Convert counts to millivolts
    float mvoltage = adc_counts_to_mvoltage(raw_counts);

	 int resistance_int = (int)resistance;
    int resistance_dec = (int)(resistance * 10) % 10;
    int temp_int_f = (int)last_temp_fahrenheit;
	 int temp_dec_f = (int)(last_temp_fahrenheit * 10) % 10;
	 int temp_int_c = (int)last_temp_celsius;
	 int temp_dec_c = (int)(last_temp_celsius * 10) % 10;
	 int mvoltage_int = (int)mvoltage;
	 int mvoltage_dec = (int)(mvoltage * 10) % 10;

	 DBG_PRINTF("Counts: %d res: %d.%d ohms mv: %d.%d mV temp: %d.%d F %d.%d C\r\n",
           raw_counts,
           resistance_int, resistance_dec,
           mvoltage_int, mvoltage_dec,
           temp_int_f, temp_dec_f,
			  temp_int_c, temp_dec_c ); 
	 /**/

    if (pel_mode == -1 && pel_active) {
        int32_t phase_limit = cool_pel_on ? cool_on_limit : cool_off_limit;
        DBG_PRINTF("Cool cycle: %s %d/%d sec\r\n",
                   cool_pel_on ? "PEL+fan" : "fan-only",
                   (int)cool_sample_count, (int)phase_limit);
    }

    //if (isDeviceCharging()) {
        batt_print_count++;
        if (batt_print_count >= BATT_PRINT_INTERVAL) {
            batt_print_count = 0;
            uint8_t  soc = bq28Z610_get_soc();
            uint16_t mv  = bq28Z610_get_voltage_mv();
            const char* status_str;
            switch(device_status) {
                case 0: status_str = "Not Charging"; break;
                case 1: status_str = "Charging"; break;
                case 2: status_str = "Full"; break;
                case 3: status_str = "Low"; break;
                case 4: status_str = "Medium"; break;
                case 5: status_str = "Normal"; break;
                default: status_str = "Unknown"; break;
            }
            DBG_PRINTF("Battery: %d%c %dmV %s\r\n", (int)soc, '%', (int)mv, status_str);

            /* Publish the real charge level on the standard BLE Battery Service
             * (0x180F / Battery Level 0x2A19). SetCharacteristicValue keeps the GATT
             * DB value current so any read returns the actual %.
             * NOTIFY-ON-CHANGE: only push a notification when the level actually
             * changes (and a client is connected), so the mobile app is woken only
             * on real changes, not on every poll. A change that happens while
             * disconnected is notified on the next poll after reconnect (last-sent
             * value only advances when a notification is actually sent). */
            (void)Cy_BLE_BASS_SetCharacteristicValue(0u, CY_BLE_BAS_BATTERY_LEVEL,
                                                     CY_BLE_BAS_BATTERY_LEVEL_LEN, &soc);
            static uint8_t last_notified_soc = 0xFFu;   /* 0xFF = nothing sent yet */
            if (soc != last_notified_soc)
            {
                if (Cy_BLE_GetConnectionState(cy_ble_connHandle[0]) == CY_BLE_CONN_STATE_CONNECTED)
                {
                    /* Only advance last_notified_soc when the notification is actually
                     * delivered; if the send fails (e.g. CCCD not enabled yet, queue
                     * full) we retry on the next poll so the client isn't left stale. */
                    cy_en_ble_api_result_t nres =
                        Cy_BLE_BASS_SendNotification(cy_ble_connHandle[0], 0u, CY_BLE_BAS_BATTERY_LEVEL,
                                                     CY_BLE_BAS_BATTERY_LEVEL_LEN, &soc);
                    if (nres == CY_BLE_SUCCESS)
                    {
                        DBG_PRINTF("BLE NOTIFY: Battery Level -> %d%c (was %d%c)\r\n",
                                   (int)soc, '%', (int)last_notified_soc, '%');
                        last_notified_soc = soc;
                    }
                    else if (nres == CY_BLE_ERROR_NTF_DISABLED)
                    {
                        /* Client is connected but has NOT enabled notifications on
                         * the Battery Level CCCD (0x2A19). Nothing to retry until it
                         * subscribes -- the value is still readable via GATT read. */
                        DBG_PRINTF("BLE NOTIFY: Battery Level not delivered - client hasn't subscribed (0x2A19 CCCD disabled)\r\n");
                    }
                    else
                    {
                        DBG_PRINTF("BLE NOTIFY: Battery Level send failed (0x%x), will retry\r\n",
                                   (int)nres);
                    }
                }
                else
                {
                    DBG_PRINTF("BLE NOTIFY: Battery Level changed to %d%c but not connected; deferring\r\n",
                               (int)soc, '%');
                }
            }
        }
    //} else {
    //    batt_print_count = BATT_PRINT_INTERVAL - 2;
    //}

    temp_control_update();
    cool_cycle_update();
}

/*******************************************************************************
* Function Name: adc_counts_to_resistance
********************************************************************************
* Summary:
*   Converts raw ADC counts to thermistor resistance in ohms.
*   V = counts * (Vref / ADC_resolution)
*   R = V / I  where I = 100uA = 0.0001A
*******************************************************************************/
static float adc_counts_to_mvoltage(int16_t counts) {

    float voltage_mv = ((float)(counts - ADC_ZERO_OFFSET) / ADC_SPAN) * ADC_VREF_MV;

    return voltage_mv;
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

    float voltage_mv = ((float)(counts - ADC_ZERO_OFFSET) / ADC_SPAN) * ADC_VREF_MV;
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


/*******************************************************************************
* Function Name: lerp_f
* Summary: Linear interpolation helper.
*******************************************************************************/
static float lerp_f(float x, float x0, float x1, float y0, float y1) {
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}


/*******************************************************************************
* Function Name: target_to_feedforward_pwm
* Summary:
*   Returns the initial PEL PWM value (0-100) for the given target actual
*   temperature, interpolated from the feedforward table.
*******************************************************************************/
static int target_to_feedforward_pwm(float target_c) {
    uint32_t n = TEMP_PWM_POINTS;

    if (target_c <= temp_pwm_table[0].target_c)
        return temp_pwm_table[0].pwm;
    if (target_c >= temp_pwm_table[n - 1u].target_c)
        return temp_pwm_table[n - 1u].pwm;

    uint32_t i;
    for (i = 0u; i < n - 1u; i++) {
        if (target_c <= temp_pwm_table[i + 1u].target_c) {
            float h = lerp_f(target_c,
                             temp_pwm_table[i].target_c, temp_pwm_table[i + 1u].target_c,
                             (float)temp_pwm_table[i].pwm, (float)temp_pwm_table[i + 1u].pwm);
            return (int)h;
        }
    }
    return temp_pwm_table[n - 1u].pwm;
}


/*******************************************************************************
* Function Name: temp_disable_heating
* Summary: Immediately stops heating and clears control state.
*******************************************************************************/
void temp_disable_heating(void) {
    pel_active        = 0;
    pel_mode          = 0;
    direct_pwm_mode   = 0;
    target_actual_c   = 0.0f;
    pel_pwm           = 0;
    trim_active       = 0;
    trim_base_pwm     = 0;
    stall_count       = 0;
    cool_sample_count = 0;
    cool_pel_on       = 1;
    cool_on_limit      = COOL_ON_SAMPLES;
    cool_off_limit     = COOL_OFF_SAMPLES;
    heat_limit_active  = 0;
    heat_limit_user_pwm = 0;
    heat_limit_count   = 0;
    heat_scale_up_count = 0;
    set_pel_pwm(0);
}


/*******************************************************************************
* Function Name: temp_set_direct_pwm
* Summary:
*   Sets the PEL PWM directly (negative = cooling, positive = heating) without
*   engaging the trim or temperature control loop.  The cooling duty cycle still
*   runs so the hot side can dissipate heat normally.  Call temp_disable_heating()
*   or set_target_temperature_c() to exit this mode.
*******************************************************************************/
void temp_set_direct_pwm(int pwm) {
    DBG_PRINTF("Direct PWM: %d\r\n", pwm);
    pel_active        = 1;
    pel_mode          = (pwm < 0) ? -1 : 1;
    direct_pwm_mode   = 1;
    pel_pwm           = pwm;
    if (pwm > 0) {
        heat_limit_user_pwm = pwm;
        heat_limit_active   = 0;
        heat_limit_count    = 0;
        heat_scale_up_count = 0;
    }
    trim_active       = 0;
    trim_base_pwm     = pwm;
    stall_count       = 0;
    cool_sample_count = 0;
    cool_pel_on       = 1;
    set_pel_pwm(pwm);
}


/*******************************************************************************
* Function Name: feedforward_slope
* Summary:
*   Returns the local slope of the feedforward table (PWM counts per deg C)
*   for the table segment containing temp_c.  Used to size trim adjustments
*   proportionally to the temperature error.
*******************************************************************************/
static float feedforward_slope(float temp_c) {
    uint32_t n = TEMP_PWM_POINTS;
    if (n < 2u) return 1.0f;
    if (temp_c <= temp_pwm_table[0].target_c)
        return (float)(temp_pwm_table[1].pwm - temp_pwm_table[0].pwm) /
               (temp_pwm_table[1].target_c - temp_pwm_table[0].target_c);
    if (temp_c >= temp_pwm_table[n - 1u].target_c)
        return (float)(temp_pwm_table[n - 1u].pwm - temp_pwm_table[n - 2u].pwm) /
               (temp_pwm_table[n - 1u].target_c - temp_pwm_table[n - 2u].target_c);
    uint32_t i;
    for (i = 0u; i < n - 1u; i++) {
        if (temp_c <= temp_pwm_table[i + 1u].target_c)
            return (float)(temp_pwm_table[i + 1u].pwm - temp_pwm_table[i].pwm) /
                   (temp_pwm_table[i + 1u].target_c - temp_pwm_table[i].target_c);
    }
    return 1.0f;
}


/*******************************************************************************
* Function Name: temp_control_update
* Summary:
*   Called each time a new thermistor sample is ready.
*
*   Phase 1 — RAMP: hold the feedforward PWM while temp is more than
*   TEMP_ADJUST_RANGE_C from target AND has not stalled.
*
*   Phase 2 — TRIM: once within range or stalled, make ONE proportional
*   adjustment (error × table slope, capped at ±TEMP_PWM_TRIM_LIMIT counts)
*   each time the temperature stalls.  Between stall events the PWM is held
*   constant so the device can react before the next correction.
*
*   Phase 3 — HOLD: inside the deadband the PWM is frozen.  If temp drifts
*   out, the trim cycle re-activates on the next stall.
*
*   trim_base_pwm is the feedforward PWM for the target temperature and is
*   NEVER changed after session start.  All trim adjustments are bounded to
*   [trim_base - TRIM_LIMIT, trim_base + TRIM_LIMIT], so the PWM can never
*   drift far from the calibrated table value regardless of how many stalls occur.
*******************************************************************************/
static void temp_control_update(void) {
    if (!pel_active) return;

    // Safety limit applies regardless of direct_pwm_mode: scale back 1 PWM count every HEAT_LIMIT_SCALE_INTERVAL seconds.
    if (last_temp_celsius >= TEMP_MAX_ALLOWED_C) {
        heat_scale_up_count = 0;
        heat_limit_count++;
        if (pel_pwm > 0 && heat_limit_count >= HEAT_LIMIT_SCALE_INTERVAL) {
            heat_limit_count = 0;
            heat_limit_active = 1;
            pel_pwm--;
            set_pel_pwm(pel_pwm);
            int act_int = (int)last_temp_celsius;
            int act_dec = (int)(last_temp_celsius * 10.0f) % 10;
            DBG_PRINTF("TEMP LIMIT: %d.%d C, pwm scaled to %d\r\n", act_int, act_dec, pel_pwm);
        }
        return;
    }
    heat_limit_count = 0;

    // If the temp limit previously reduced the PWM and the temp has now dropped too far
    // below the max, scale back up 1 count every HEAT_LIMIT_SCALE_UP_INTERVAL seconds.
    if (heat_limit_active && pel_pwm < heat_limit_user_pwm &&
        last_temp_celsius < (TEMP_MAX_ALLOWED_C - HEAT_LIMIT_RECOVER_BELOW_C)) {
        heat_scale_up_count++;
        if (heat_scale_up_count >= HEAT_LIMIT_SCALE_UP_INTERVAL) {
            heat_scale_up_count = 0;
            pel_pwm++;
            set_pel_pwm(pel_pwm);
            int act_int = (int)last_temp_celsius;
            int act_dec = (int)(last_temp_celsius * 10.0f) % 10;
            DBG_PRINTF("TEMP RECOVER: %d.%d C, pwm scaled to %d\r\n", act_int, act_dec, pel_pwm);
        }
    } else {
        heat_scale_up_count = 0;
    }

    if (direct_pwm_mode) return;

    float rate  = last_temp_celsius - prev_temp_c;  // positive = rising, negative = falling
    prev_temp_c = last_temp_celsius;

    float error   = target_actual_c - last_temp_celsius;
    float abs_err = (error < 0.0f) ? -error : error;

    // Count consecutive settled samples to detect a temperature stall.
    int settled = (rate > -TEMP_SETTLED_THRESHOLD_C && rate < TEMP_SETTLED_THRESHOLD_C);
    if (settled) {
        if (stall_count < TEMP_STALL_SAMPLES) stall_count++;
    } else {
        stall_count = 0;
    }
    int stalled = (stall_count >= TEMP_STALL_SAMPLES);

    // Phase 1 — RAMP: hold feedforward PWM until within range or stalled.
    if (!trim_active) {
        if (abs_err <= TEMP_ADJUST_RANGE_C || stalled) {
            trim_active = 1;
            stall_count = 0;
            DBG_PRINTF("Trim open: ff_base=%d\r\n", trim_base_pwm);
        } else {
            return;
        }
    }

    // Phase 3 — HOLD: inside deadband, freeze PWM; reset stall so re-entry
    // into trim only fires after a genuine new stall outside the deadband.
    if (abs_err <= TEMP_DEADBAND_C) {
        stall_count = 0;
        return;
    }

    // Phase 2 — TRIM: only act on a stall event; hold between stalls.
    if (!stalled) return;

    // Proportional adjustment: error × local table slope, capped at ±TRIM_LIMIT.
    float slope = feedforward_slope(last_temp_celsius);
    float adj_f = error * slope;
    int adjustment = (adj_f >= 0.0f) ? (int)(adj_f + 0.5f) : (int)(adj_f - 0.5f);
    if (adjustment >  TEMP_PWM_TRIM_LIMIT) adjustment =  TEMP_PWM_TRIM_LIMIT;
    if (adjustment < -TEMP_PWM_TRIM_LIMIT) adjustment = -TEMP_PWM_TRIM_LIMIT;

    int new_pwm = pel_pwm + adjustment;

    // Hard bound: PWM must stay within ±TRIM_LIMIT of the feedforward table value.
    // trim_base_pwm == ff_pwm and is never updated, so this window is permanent.
    int pwm_floor   = trim_base_pwm - TEMP_PWM_TRIM_LIMIT;
    int pwm_ceiling = trim_base_pwm + TEMP_PWM_TRIM_LIMIT;
    int mode_floor   = (pel_mode == 1) ? 0    : -COOL_MAX_PWM;
    int mode_ceiling = (pel_mode == 1) ? target_to_feedforward_pwm(TEMP_MAX_ALLOWED_C) : 0;
    if (pwm_floor   < mode_floor)   pwm_floor   = mode_floor;
    if (pwm_ceiling > mode_ceiling) pwm_ceiling = mode_ceiling;
    if (new_pwm < pwm_floor)   new_pwm = pwm_floor;
    if (new_pwm > pwm_ceiling) new_pwm = pwm_ceiling;

    stall_count = 0;

    if (new_pwm == pel_pwm) return;  // already at the limit — nothing to apply

    pel_pwm = new_pwm;
    set_pel_pwm(pel_pwm);
    DBG_PRINTF("Trim adj=%d pwm=%d (ff_base=%d)\r\n", adjustment, pel_pwm, trim_base_pwm);
}


/*******************************************************************************
* Function Name: cool_cycle_update
* Summary:
*   Called once per temperature sample (~1 sec) when in cooling mode.
*   Alternates between PEL+fan ON (COOL_ON_SAMPLES seconds) and fan-only
*   (COOL_OFF_SAMPLES seconds) to let the hot side dissipate heat and prevent
*   the Peltier from saturating.  During the fan-only phase the PEL PWM
*   registers are forced to zero each sample so any trim write cannot
*   accidentally re-enable the Peltier mid-cycle.
*******************************************************************************/
static void cool_cycle_update(void) {
    if (!pel_active || pel_mode != -1) {
        cool_sample_count = 0;
        cool_pel_on       = 1;
        return;
    }

    cool_sample_count++;

    if (cool_pel_on) {
        if (cool_sample_count >= cool_on_limit) {
            cool_pel_on       = 0;
            cool_sample_count = 0;
            set_pel_pwm(0);
            set_fan(100);     // full speed during recovery phase
            power_led_green(); CyDelay(100); power_led_off();  // green = PEL off, recovery
            DBG_PRINTF("Cool cycle: PEL off (recovery), fan on\r\n");
        }
    } else {
        // Re-assert PEL off every sample so a trim event cannot accidentally re-enable it.
        set_pel_pwm(0);
        set_fan(100);   // full speed

        if (cool_sample_count >= cool_off_limit) {
            cool_pel_on       = 1;
            cool_sample_count = 0;
            set_pel_pwm(pel_pwm);   // restore cooling PWM
            power_led_blue(); CyDelay(100); power_led_off();   // blue = PEL on
            DBG_PRINTF("Cool cycle: PEL on\r\n");
        }
    }
}


/*******************************************************************************
* Function Name: set_target_temperature_c
* Summary:
*   Sets the desired actual surface temperature (deg C). Immediately applies a
*   feedforward heater value to reduce lag, then temp_control_update() trims
*   it each thermistor sample. Target is clamped to TEMP_MAX_ALLOWED_C (43.3 C).
*******************************************************************************/
void set_target_temperature_c(float desired_c) {
    if (desired_c > TEMP_MAX_ALLOWED_C) {
        desired_c = TEMP_MAX_ALLOWED_C;
        DBG_PRINTF("Temp target clamped to %d C\r\n", (int)TEMP_MAX_ALLOWED_C);
    }

    target_actual_c   = desired_c;
    pel_active        = 1;
    direct_pwm_mode   = 0;
    prev_temp_c       = last_temp_celsius;
    trim_active       = 0;
    stall_count       = 0;
    cool_sample_count = 0;
    cool_pel_on       = 1;

    if (desired_c > TEMP_ROOM_TEMP_C) {
        pel_mode = 1;   // heating session
        pel_pwm  = target_to_feedforward_pwm(desired_c);
    } else {
        pel_mode = -1;  // cooling session
        pel_pwm  = 0;
    }
    trim_base_pwm = pel_pwm;
    set_pel_pwm(pel_pwm);

    DBG_PRINTF("Target: %d C  FF pwm: %d\r\n", (int)desired_c, pel_pwm);
}


void set_target_temperature_f(float desired_f) {
	 float target_c = (desired_f - 32.0f) * 5.0f / 9.0f;
	 DBG_PRINTF("Target: %d C  ", (int)desired_f);
    set_target_temperature_c(target_c);
}


void set_pel_pwm(int value){

	static int prev_value = 0;
	// Limit the value to [-COOL_MAX_PWM, HEAT_MAX_PWM]
	value = (value < -COOL_MAX_PWM) ? -COOL_MAX_PWM : (value > HEAT_MAX_PWM) ? HEAT_MAX_PWM : value;

	// Scale the value to the PWM range
	int scaled_pel_pwm = (abs(value) * MAX_PEL_PWM_VALUE) / 100;
	DBG_PRINTF("pel value: %d scaled: %d\r\n", value, scaled_pel_pwm);

	if (value > 0)
	{
		PWM_PEL1_SetCompare0(0);				  // PEL1 off
		PWM_PEL2_SetCompare0(scaled_pel_pwm); // PEL2 on
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
		  if ( prev_value < 0 ) {
        	set_fan(1);
		  }
		  else {
			  set_fan(0);
		  }
        //PWM_PEL1_Disable();
        //PWM_PEL2_Disable();
        //Cy_GPIO_Write(TEMP_USER_EN1_PORT, TEMP_USER_EN1_NUM, 0);  //Enable is low
        //DBG_PRINTF("Disabled PWM1 GetCompare: %d\r\n", PWM_PEL1_GetCompare0());
        //DBG_PRINTF("Disabled PWM2 GetCompare: %d\r\n", PWM_PEL2_GetCompare0());
        //set_fan(0);
    }
	 prev_value = value;
}

void set_fan(int value){
    
    // limit to 100
    value = ( abs(value) > 100) ? 100 : value;
    
    // scale the value to the PWM range
    //int scaled_fan_pwm = ( abs(value) * MAX_FAN_PWM_VALUE) / 100;
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
// run_seconds = (-8.87 * power_level) + 56.62, clamped to >= 1
static int32_t run_seconds_for_power(float power_level) {
    float run = (-8.87f * power_level) + 56.62f;
    if (run < 1.0f) run = 1.0f;
    return (int32_t)(run + 0.5f);
}

// Piecewise-linear interpolation of measured recovery times vs. normalised power level.
// Mirrors the Python estimated_recovery() function exactly.
static int32_t estimated_recovery_seconds(float power_level) {
    static const float levels[] = {0.25f, 0.50f, 0.75f, 1.00f};
    static const int   times[]  = {87,    160,   175,   149};
    int i;
    if (power_level <= levels[0]) return (int32_t)times[0];
    if (power_level >= levels[3]) return (int32_t)times[3];
    for (i = 0; i < 3; i++) {
        if (power_level <= levels[i + 1]) {
            float t      = (power_level - levels[i]) / (levels[i + 1] - levels[i]);
            float result = (float)times[i] + t * (float)(times[i + 1] - times[i]);
            return (int32_t)(result + 0.5f);
        }
    }
    return (int32_t)times[3];
}

/*******************************************************************************
* Function Name: temp_set_power_level
* Summary:
*   Main user-facing control entry point. Accepts a power level from -100 to 100
*   where 0 = off, positive = heat, negative = cold.
*
*   User level is scaled to the device PWM range:
*     heat: user [1,100] → device [1, HEAT_MAX_PWM]
*     cold: user [-1,-100] → device [-1, -COOL_MAX_PWM]
*
*   For cooling sessions the on/off cycle times are computed from the power level
*   using empirical formulas before the direct-PWM session starts.
*******************************************************************************/
void temp_set_power_level(int user_level) {
    if (user_level == 0) {
        temp_disable_heating();
        return;
    }

    int device_pwm;
    if (user_level > 0) {
        device_pwm = (user_level * HEAT_MAX_PWM + 50) / 100;
        if (device_pwm < 1) device_pwm = 1;
    } else {
        int abs_user = -user_level;
        int abs_dev  = (abs_user * COOL_MAX_PWM + 50) / 100;
        device_pwm   = -abs_dev;
        if (device_pwm > -1) device_pwm = -1;

        float power_level = (float)(-device_pwm) / (float)COOL_MAX_PWM;
        cool_on_limit  = run_seconds_for_power(power_level);
        cool_off_limit = estimated_recovery_seconds(power_level);
        DBG_PRINTF("Cool limits: on=%d off=%d sec\r\n", (int)cool_on_limit, (int)cool_off_limit);
    }

    DBG_PRINTF("Power level: user=%d device=%d\r\n", user_level, device_pwm);
    temp_set_direct_pwm(device_pwm);
}

/* [] END OF FILE */
