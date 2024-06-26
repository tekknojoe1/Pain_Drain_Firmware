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
 * Driver for BQ25883 Charger device
*/


#include <project.h>


#define BQ25883_I2C_ADDR                              0x6B

#define BQ2588X_REG_CHARGE_VOLT                       0x00
    #define BQ2588X_VREG_MASK                         0xFF
    #define BQ2588X_VREG_SHIFT                        0
    #define BQ2588X_VREG_BASE                         6800
    #define BQ2588X_VREG_LSB                          10

#define BQ2588X_REG_CHARGE_CURRENT                    0x01
    #define BQ2588X_EN_HIZ_MASK                       0x80
    #define BQ2588X_EN_HIZ_SHIFT                      7
    #define BQ2588X_HIZ_DISABLE                       0
    #define BQ2588X_HIZ_ENABLE                        1
    #define BQ2588X_EN_ILIM_MASK                      0x40
    #define BQ2588X_EN_ILIM_SHIFT                     6
    #define BQ2588X_ILIM_PIN_DISABLE                  0
    #define BQ2588X_ILIM_PIN_ENABLE                   1
    #define BQ2588X_ICHG_MASK                         0x3F
    #define BQ2588X_ICHG_SHIFT                        0
    #define BQ2588X_ICHG_BASE                         0
    #define BQ2588X_ICHG_LSB                          50

#define BQ2588X_REG_VINDPM                            0x02
    #define BQ2588X_VINDPM_RESET_EN_MASK              0x80
    #define BQ2588X_VINDPM_RESET_EN_SHIFT             7
    #define BQ2588X_VINDPM_RESET_DISABLE              0
    #define BQ2588X_VINDPM_RESET_ENABLE               1
    #define BQ2588X_BAT_DISCHG_LOAD_MASK              0x40
    #define BQ2588X_BAT_DISCHG_LOAD_SHIFT             6
    #define BQ2588X_BAT_DISCHG_LOAD_DISABLE           0
    #define BQ2588X_BAT_DISCHG_LOAD_ENABLE            1
    #define BQ2588X_VINDPM_TH_MASK                    0x1F
    #define BQ2588X_VINDPM_TH_SHIFT                   0
    #define BQ2588X_VINDPM_TH_BASE                    3900
    #define BQ2588X_VINDPM_TH_LSB                     100

#define BQ2588X_REG_IINDPM                            0x03
    #define BQ2588X_FORCE_ICO_MASK                    0x80
    #define BQ2588X_FORCE_ICO_SHIFT                   7
    #define BQ2588X_DO_NOT_FORCE                      0
    #define BQ2588X_DO_FORCE                          1
    #define BQ2588X_DPDM_DETECT_MASK                  0x40
    #define BQ2588X_DPDM_DETECT_SHIFT                 6
    #define BQ2588X_DPDM_DETECT_DISABLE               0
    #define BQ2588X_DPDM_DETECT_ENABLE                1
    #define BQ2588X_EN_ICO_MASK                       0x20
    #define BQ2588X_EN_ICO_SHIFT                      5
    #define BQ2588X_ICO_DISABLE                       0
    #define BQ2588X_ICO_ENABLE                        1
    #define BQ2588X_IINDPM_TH_MASK                    0x1F
    #define BQ2588X_IINDPM_TH_SHIFT                   0
    #define BQ2588X_IINDPM_TH_BASE                    500
    #define BQ2588X_IINDPM_TH_LSB                     100

#define BQ2588X_REG_PRECHG_TERM                       0x04
    #define BQ2588X_IPRECHG_MASK                      0xF0
    #define BQ2588X_IPRECHG_SHIFT                     4
    #define BQ2588X_IPRECHG_BASE                      50
    #define BQ2588X_IPRECHG_LSB                       50
    #define BQ2588X_ITERM_MASK                        0x0F
    #define BQ2588X_ITERM_SHIFT                       0
    #define BQ2588X_ITERM_BASE                        50
    #define BQ2588X_ITERM_LSB                         50

#define BQ2588X_REG_CHG_CTRL1                         0x05
    #define BQ2588X_TERM_EN_MASK                      0x80
    #define BQ2588X_TERM_EN_SHIFT                     7
    #define BQ2588X_TERM_DISABLE                      0
    #define BQ2588X_TERM_ENABLE                       1
    #define BQ2588X_WDT_TIMER_MASK                    0x30
    #define BQ2588X_WDT_TIMER_SHIFT                   4
    #define BQ2588X_WDT_TIMER_DISABLE                 0
    #define BQ2588X_WDT_TIMER_40S                     1
    #define BQ2588X_WDT_TIMER_80S                     2
    #define BQ2588X_WDT_TIMER_160S                    3
    #define BQ2588X_SAFETY_TIMER_EN_MASK              0x08
    #define BQ2588X_SAFETY_TIMER_EN_SHIFT             3
    #define BQ2588X_SAFETY_TIMER_DISABLE              0
    #define BQ2588X_SAFETY_TIMER_ENABLE               1
    #define BQ2588X_SAFETY_TIMER_MASK                 0x06
    #define BQ2588X_SAFETY_TIMER_SHIFT                1
    #define BQ2588X_SAFETY_TIMER_5H                   0
    #define BQ2588X_SAFETY_TIMER_8H                   1
    #define BQ2588X_SAFETY_TIMER_12H                  2
    #define BQ2588X_SAFETY_TIMER_20H                  3

#define BQ2588X_REG_CHG_CTRL2                         0x06
    #define BQ2588X_OTG_EN_MASK                       0x80
    #define BQ2588X_OTG_EN_SHIFT                      7
    #define BQ2588X_OTG_DISABLE                       0
    #define BQ2588X_OTG_ENABLE                        1
    #define BQ2588X_AUTO_DPDM_MASK                    0x40
    #define BQ2588X_AUTO_DPDM_SHIFT                   6
    #define BQ2588X_AUTO_DPDM_DISABLE                 0
    #define BQ2588X_AUTO_DPDM_ENABLE                  1
    #define BQ2588X_CHARGE_EN_MASK                    0x08
    #define BQ2588X_CHARGE_EN_SHIFT                   3
    #define BQ2588X_CHARGE_DISABLE                    0
    #define BQ2588X_CHARGE_ENABLE                     1
    #define BQ2588X_BATLOW_VOLT_MASK                  0x04
    #define BQ2588X_BATLOW_VOLT_SHIFT                 2
    #define BQ2588X_BATLOW_VOLT_5P6V                  0
    #define BQ2588X_BATLOW_VOLT_6V                    1
    #define BQ2588X_RECHG_VOLT_MASK                   0x03
    #define BQ2588X_RECHG_VOLT_SHIFT                  0
    #define BQ2588X_RECHG_VOLT_BASE                   100
    #define BQ2588X_RECHG_VOLT_LSB                    100

#define BQ2588X_REG_CHG_CTRL3                         0x07
    #define BQ2588X_WDT_RESET_MASK                    0x40
    #define BQ2588X_WDT_RESET_SHIFT                   6
    #define BQ2588X_WDT_RESET_DISABLE                 0
    #define BQ2588X_WDT_RESET                         1
    #define BQ2588X_TOPOFF_TIMER_MASK                 0x30
    #define BQ2588X_TOPOFF_TIMER_SHIFT                4
    #define BQ2588X_TOPOFF_TIMER_DISABLE              0
    #define BQ2588X_TOPOFF_TIMER_15M                  1
    #define BQ2588X_TOPOFF_TIMER_30M                  2
    #define BQ2588X_TOPOFF_TIMER_45M                  3
    #define BQ2588X_SYS_MIN_VOLT_MASK                 0x0F
    #define BQ2588X_SYS_MIN_VOLT_SHIFT                0
    #define BQ2588X_SYS_MIN_VOLT_BASE                 6000
    #define BQ2588X_SYS_MIN_VOLT_LSB                  100
    
#define BQ2588X_REG_CHG_CTRL4                         0x08

#define BQ2588X_REG_OTG_CTRL                          0x09
    #define BQ2588X_OTG_ILIM_MASK                     0xF0
    #define BQ2588X_OTG_ILIM_SHIFT                    4
    #define BQ2588X_OTG_ILIM_BASE                     500
    #define BQ2588X_OTG_ILIM_LSB                      100
    #define BQ2588X_OTG_VLIM_MASK                     0x0F
    #define BQ2588X_OTG_VLIM_SHIFT                    0
    #define BQ2588X_OTG_VLIM_BASE                     4500
    #define BQ2588X_OTG_VLIM_LSB                      100

#define BQ2588X_REG_ICO_LIMIT                         0x0A
    #define BQ2588X_ICO_ILIM_MASK                     0x1F
    #define BQ2588X_ICO_ILIM_SHIFT                    0
    #define BQ2588X_ICO_ILIM_BASE                     500
    #define BQ2588X_ICO_ILIM_LSB                      100

#define BQ2588X_REG_CHG_STATUS1                       0x0B
    #define BQ2588X_ADC_DONE_STAT_MASK                0x80
    #define BQ2588X_IINDPM_STAT_MASK                  0x40
    #define BQ2588X_IINDPM_STAT_SHIFT                 6
    #define BQ2588X_IINDPM_STAT_INACTIVE              0
    #define BQ2588X_IINDPM_STAT_ACTIVE                1
    #define BQ2588X_VINDPM_STAT_MASK                  0x20
    #define BQ2588X_VINDPM_STAT_SHIFT                 5
    #define BQ2588X_VINDPM_STAT_INACTIVE              0
    #define BQ2588X_VINDPM_STAT_ACTIVE                1
    #define BQ2588X_TREG_STAT_MASK                    0x10
    #define BQ2588X_TREG_STAT_SHIFT                   4
    #define BQ2588X_TREG_STAT_INACTIVE                0
    #define BQ2588X_TREG_STAT_ACTIVE                  1
    #define BQ2588X_WDT_STAT_MASK                     0x08
    #define BQ2588X_WDT_STAT_SHIFT                    3
    #define BQ2588X_WDT_STAT_INACTIVE                 0
    #define BQ2588X_WDT_STAT_EXPIRED                  1
    #define BQ2588X_CHRG_STAT_MASK                    0x07
    #define BQ2588X_CHRG_STAT_SHIFT                   0
    #define BQ2588X_CHRG_STAT_IDLE                    0
    #define BQ2588X_CHRG_STAT_TRICKLE                  1
    #define BQ2588X_CHRG_STAT_PRECHG                  2
    #define BQ2588X_CHRG_STAT_FAST                    3
    #define BQ2588X_CHRG_STAT_TAPER                   4
    #define BQ2588X_CHRG_STAT_TOPOFF                  5
    #define BQ2588X_CHRG_STAT_DONE                    6
    #define BQ2588X_CHRG_STAT_RSVED                   7

#define BQ2588X_REG_CHG_STATUS2                       0x0c
    #define BQ2588X_PG_STAT_MASK                      0x80
    #define BQ2588X_PG_STAT_SHIFT                     7
    #define BQ2588X_PG_STAT_POOR                      0
    #define BQ2588X_PG_STAT_GOOD                      1
    #define BQ2588X_VBUS_STAT_MASK                    0x70
    #define BQ2588X_VBUS_STAT_SHIFT                   4
    #define BQ2588X_VBUS_NO_INPUT                     0
    #define BQ2588X_VBUS_USB_SDP                      1
    #define BQ2588X_VBUS_USB_CDP                      2
    #define BQ2588X_VBUS_USB_DCP                      3
    #define BQ2588X_VBUS_POOR_SRC                     4
    #define BQ2588X_VBUS_USB_UNKNOWN                  5
    #define BQ2588X_VBUS_USB_NON_STANDARD             6
    #define BQ2588X_VBUS_OTG_MODE                     7
    #define BQ2588X_ICO_STAT_MASK                     0x06
    #define BQ2588X_ICO_STAT_SHIFT                    1
    #define BQ2588X_ICO_DISABLED                      0
    #define BQ2588X_ICO_IN_PROGRESS                   1
    #define BQ2588X_ICO_DONE                          2
    #define BQ2588X_RESERVED                          3

#define BQ2588X_REG_NTC_STATUS                        0x0D
    #define BQ2588X_TS_STAT_MASK                      0x07
    #define BQ2588X_TS_STAT_SHIFT                     0
    #define BQ2588X_TS_NORMAL                         0
    #define BQ2588X_TS_WARM                           1
    #define BQ2588X_TS_COOL                           2
    #define BQ2588X_TS_COLD                           3
    #define BQ2588X_TS_HOT                            4
    #define BQ2588X_TS_RSVED0                         5
    #define BQ2588X_TS_RSVED1                         6
    #define BQ2588X_TS_RSVED2                         7

#define BQ2588X_REG_FAULT_STATUS                      0x0E
    #define BQ2588X_VBUS_OVP_STAT_MASK                0x80
    #define BQ2588X_VBUS_OVP_STAT_SHIFT               7
    #define BQ2588X_VBUS_OVP_STAT_INACTIVE            0
    #define BQ2588X_VBUS_OVP_STAT_ACTIVE              1
    #define BQ2588X_TSHUT_STAT_MASK                   0x40
    #define BQ2588X_TSHUT_STAT_SHIFT                  6
    #define BQ2588X_TSHUT_STAT_INACTIVE               0
    #define BQ2588X_TSHUT_STAT_ACTIVE                 1
    #define BQ2588X_BAT_OVP_STAT_MASK                 0x20
    #define BQ2588X_BAT_OVP_STAT_SHIFT                5
    #define BQ2588X_BAT_OVP_STAT_INACTIVE             0
    #define BQ2588X_BAT_OVP_STAT_ACTIVE               1
    #define BQ2588X_SAFETY_TIMER_STAT_MASK            0x10
    #define BQ2588X_SAFETY_TIMER_STAT_SHIFT           4
    #define BQ2588X_SAFETY_TIMER_STAT_NORMAL          0
    #define BQ2588X_SAFETY_TIMER_STAT_EXPIRED         1
    #define BQ2588X_SYS_SHORT_STAT_MASK               0x08
    #define BQ2588X_SYS_SHORT_STAT_SHIFT              3
    #define BQ2588X_SYS_SHORT_STAT_INACTIVE           0
    #define BQ2588X_SYS_SHORT_STAT_ACTIVE             1
    #define BQ2588X_OTG_STAT_MASK                     0x01
    #define BQ2588X_OTG_STAT_SHIFT                    0
    #define BQ2588X_OTG_STAT_NORMAL                   0
    #define BQ2588X_OTG_STAT_BUS_OVERLOAD             1

#define BQ2588X_REG_CHG_FLAG1                         0x0F
    #define BQ2588X_IINDPM_FLAG_MASK                  0x40
    #define BQ2588X_IINDPM_FLAG_SHIFT                 6
    #define BQ2588X_IINDPM_FLAG_INACTIVE              0
    #define BQ2588X_IINDPM_FLAG_ACTIVE                1
    #define BQ2588X_VINDPM_FLAG_MASK                  0x20
    #define BQ2588X_VINDPM_FLAG_SHIFT                 5
    #define BQ2588X_VINDPM_FLAG_INACTIVE              0
    #define BQ2588X_VINDPM_FLAG_ACTIVE                1
    #define BQ2588X_TREG_FLAG_MASK                    0x10
    #define BQ2588X_TREG_FLAG_SHIFT                   4
    #define BQ2588X_TREG_FLAG_INACTIVE                0
    #define BQ2588X_TREG_FLAG_ACTIVE                  1
    #define BQ2588X_WDT_FLAG_MASK                     0x08
    #define BQ2588X_WDT_FLAG_SHIFT                    3
    #define BQ2588X_WDT_FLAG_NORMAL                   0
    #define BQ2588X_WDT_FLAG_EXPIRED                  1
    #define BQ2588X_CHRG_FLAG_MASK                    0x01
    #define BQ2588X_CHRG_FLAG_SHIFT                   0
    #define BQ2588X_CHRG_FLAG_NORMAL                  0
    #define BQ2588X_CHRG_FLAG_CHANGED                 1

#define BQ2588X_REG_CHG_FLAG2                         0x10
    #define BQ2588X_PG_FLAG_MASK                      0x80
    #define BQ2588X_PG_FLAG_SHIFT                     7
    #define BQ2588X_PG_FLAG_POOR                      0
    #define BQ2588X_PG_FLAG_GOOD                      1
    #define BQ2588X_VBUS_FLAG_MASK                    0x10
    #define BQ2588X_VBUS_FLAG_SHIFT                   4
    #define BQ2588X_VBUS_FLAG_NORMAL                  0
    #define BQ2588X_VBUS_FLAG_CHANGED                 1
    #define BQ2588X_TS_FLAG_MASK                      0x04
    #define BQ2588X_TS_FLAG_SHIFT                     2
    #define BQ2588X_TS_FLAG_NORMAL                    0
    #define BQ2588X_TS_FLAG_CHANGED                   1
    #define BQ2588X_ICO_FLAG_MASK                     0x02
    #define BQ2588X_ICO_FLAG_SHIFT                    1
    #define BQ2588X_ICO_FLAG_NORMAL                   0
    #define BQ2588X_ICO_FLAG_CHANGED                  1
    #define BQ2588X_VSYS_FLAG_MASK                    0x01
    #define BQ2588X_VSYS_FLAG_SHIFT                   0
    #define BQ2588X_VSYS_FLAG_NORMAL                  0
    #define BQ2588X_VSYS_FLAG_CHANGED                 1

#define BQ2588X_REG_FAULT_FLAG                        0x11
    #define BQ2588X_VBUS_OVP_FLAG_MASK                0x80
    #define BQ2588X_VBUS_OVP_FLAG_SHIFT               7
    #define BQ2588X_VBUS_OVP_FLAG_INACTIVE            0
    #define BQ2588X_VBUS_OVP_FLAG_ACTIVE              1
    #define BQ2588X_TSHUT_FLAG_MASK                   0x40
    #define BQ2588X_TSHUT_FLAG_SHIFT                  6
    #define BQ2588X_TSHUT_FLAG_INACTIVE               0
    #define BQ2588X_TSHUT_FLAG_ACTIVE                 1
    #define BQ2588X_BAT_OVP_FLAG_MASK                 0x20
    #define BQ2588X_BAT_OVP_FLAG_SHIFT                5
    #define BQ2588X_BAT_OVP_FLAG_INACTIVE             0
    #define BQ2588X_BAT_OVP_FLAG_ACTIVE               1
    #define BQ2588X_SAFETY_TIMER_FLAG_MASK            0x10
    #define BQ2588X_SAFETY_TIMER_FLAG_SHIFT           4
    #define BQ2588X_SAFETY_TIMER_FLAG_NORMAL          0
    #define BQ2588X_SAFETY_TIMER_FLAG_EXPIRED         1
    #define BQ2588X_SYS_SHORT_FLAG_MASK               0x08
    #define BQ2588X_SYS_SHORT_FLAG_SHIFT              3
    #define BQ2588X_SYS_SHORT_FLAG_INACTIVE           0
    #define BQ2588X_SYS_SHORT_FLAG_ACTIVE             1
    #define BQ2588X_OTG_FLAG_MASK                     0x01
    #define BQ2588X_OTG_FLAG_SHIFT                    0
    #define BQ2588X_OTG_FLAG_NORMAL                   0
    #define BQ2588X_OTG_FLAG_BUS_OVERLOAD             1

#define BQ2588X_REG_CHG_INT_MASK1                     0x12
    #define BQ2588X_IINDPM_INT_MASK_MASK              0x40
    #define BQ2588X_IINDPM_INT_MASK_SHIFT             6
    #define BQ2588X_IINDPM_INT_MASK_DISABLE           0
    #define BQ2588X_IINDPM_INT_MASK_ENABLE            1
    #define BQ2588X_VINDPM_INT_MASK_MASK              0x20
    #define BQ2588X_VINDPM_INT_MASK_SHIFT             5
    #define BQ2588X_VINDPM_INT_MASK_DISABLE           0
    #define BQ2588X_VINDPM_INT_MASK_ENABLE            1
    #define BQ2588X_TREG_INT_MASK_MASK                0x10
    #define BQ2588X_TREG_INT_MASK_SHIFT               4
    #define BQ2588X_TREG_INT_MASK_DISABLE             0
    #define BQ2588X_TREG_INT_MASK_ENABLE              1
    #define BQ2588X_WDT_INT_MASK_MASK                 0x08
    #define BQ2588X_WDT_INT_MASK_SHIFT                3
    #define BQ2588X_WDT_INT_MASK_DISABLE              0
    #define BQ2588X_WDT_INT_MASK_ENABLE               1
    #define BQ2588X_CHRG_INT_MASK_MASK                0x01
    #define BQ2588X_CHRG_INT_MASK_SHIFT               0
    #define BQ2588X_CHRG_INT_MASK_DISABLE             0
    #define BQ2588X_CHRG_INT_MASK_ENABLE              1

#define BQ2588X_REG_CHG_INT_MASK2                     0x13
    #define BQ2588X_PG_INT_MASK_MASK                  0x80
    #define BQ2588X_PG_INT_MASK_SHIFT                 7
    #define BQ2588X_PG_INT_MASK_DISABLE               0
    #define BQ2588X_PG_INT_MASK_ENABLE                1
    #define BQ2588X_VBUS_INT_MASK_MASK                0x10
    #define BQ2588X_VBUS_INT_MASK_SHIFT               4
    #define BQ2588X_VBUS_INT_MASK_DISABLE             0
    #define BQ2588X_VBUS_INT_MASK_ENABLE              1
    #define BQ2588X_TS_INT_MASK_MASK                  0x04
    #define BQ2588X_TS_INT_MASK_SHIFT                 2
    #define BQ2588X_TS_INT_MASK_DISABLE               0
    #define BQ2588X_TS_INT_MASK_ENABLE                1
    #define BQ2588X_ICO_INT_MASK_MASK                 0x02
    #define BQ2588X_ICO_INT_MASK_SHIFT                1
    #define BQ2588X_ICO_INT_MASK_DISABLE              0
    #define BQ2588X_ICO_INT_MASK_ENABLE               1
    #define BQ2588X_VSYS_INT_MASK_MASK                0x01
    #define BQ2588X_VSYS_INT_MASK_SHIFT               0
    #define BQ2588X_VSYS_INT_MASK_DISABLE             0
    #define BQ2588X_VSYS_INT_MASK_ENABLE              1

#define BQ2588X_REG_FAULT_INT_MASK                    0x14
    #define BQ2588X_VBUS_OVP_INT_MASK_MASK            0x80
    #define BQ2588X_VBUS_OVP_INT_MASK_SHIFT           7
    #define BQ2588X_VBUS_OVP_INT_MASK_DISABLE         0
    #define BQ2588X_VBUS_OVP_INT_MASK_ENABLE          1
    #define BQ2588X_TSHUT_INT_MASK_MASK               0x40
    #define BQ2588X_TSHUT_INT_MASK_SHIFT              6
    #define BQ2588X_TSHUT_INT_MASK_DISABLE            0
    #define BQ2588X_TSHUT_INT_MASK_ENABLE             1
    #define BQ2588X_BAT_OVP_INT_MASK_MASK             0x20
    #define BQ2588X_BAT_OVP_INT_MASK_SHIFT            5
    #define BQ2588X_BAT_OVP_INT_MASK_DISABLE          0
    #define BQ2588X_BAT_OVP_INT_MASK_ENABLE           1
    #define BQ2588X_SAFETY_TIMER_INT_MASK_MASK        0x10
    #define BQ2588X_SAFETY_TIMER_INT_MASK_SHIFT       4
    #define BQ2588X_SAFETY_TIMER_INT_MASK_DISABLE     0
    #define BQ2588X_SAFETY_TIMER_INT_MASK_ENABLE      1
    #define BQ2588X_SYS_SHORT_INT_MASK_MASK           0x08
    #define BQ2588X_SYS_SHORT_INT_MASK_SHIFT          3
    #define BQ2588X_SYS_SHORT_INT_MASK_DISABLE        0
    #define BQ2588X_SYS_SHORT_INT_MASK_ENABLE         1
    #define BQ2588X_OTG_INT_MASK_MASK                 0x01
    #define BQ2588X_OTG_INT_MASK_SHIFT                0
    #define BQ2588X_OTG_INT_MASK_DISABLE              0
    #define BQ2588X_OTG_INT_MASK_ENABLE               1

#define BQ2588X_REG_ADC_CTRL                          0x15
    #define BQ2588X_ADC_SCAN_EN_MASK                  0x80
    #define BQ2588X_ADC_SCAN_EN_SHIFT                 7
    #define BQ2588X_ADC_SCAN_DISABLE                  0
    #define BQ2588X_ADC_SCAN_ENABLE                   1
    #define BQ2588X_ADC_SCAN_RATE_MASK                0x40
    #define BQ2588X_ADC_SCAN_RATE_SHIFT               6
    #define BQ2588X_ADC_SCAN_CONTINUOUS               0
    #define BQ2588X_ADC_SCAN_ONESHOT                  1
    #define BQ2588X_ADC_SCAN_BITS_MASK                0x30
    #define BQ2588X_ADC_SCAN_BITS_SHIFT               4
    #define BQ2588X_ADC_SCAN_15BITS                   0
    #define BQ2588X_ADC_SCAN_14BITS                   1
    #define BQ2588X_ADC_SCAN_13BITS                   2
    #define BQ2588X_ADC_SCAN_12BITS                   3

#define BQ2588X_REG_ADC_FUNC_DIS                      0x16
    #define BQ2588X_IBUS_ADC_DIS_MASK                 0x80
    #define BQ2588X_IBUS_ADC_DIS_SHIFT                7
    #define BQ2588X_IBUS_ADC_ENABLE                   0
    #define BQ2588X_IBUS_ADC_DISABLE                  1
    #define BQ2588X_ICHG_ADC_DIS_MASK                 0x40
    #define BQ2588X_ICHG_ADC_DIS_SHIFT                6
    #define BQ2588X_ICHG_ADC_ENABLE                   0
    #define BQ2588X_ICHG_ADC_DISABLE                  1
    #define BQ2588X_VBUS_ADC_DIS_MASK                 0x20
    #define BQ2588X_VBUS_ADC_DIS_SHIFT                5
    #define BQ2588X_VBUS_ADC_ENABLE                   0
    #define BQ2588X_VBUS_ADC_DISABLE                  1
    #define BQ2588X_VBAT_ADC_DIS_MASK                 0x10
    #define BQ2588X_VBAT_ADC_DIS_SHIFT                4
    #define BQ2588X_VBAT_ADC_ENABLE                   0
    #define BQ2588X_VBAT_ADC_DISABLE                  1
    #define BQ2588X_VSYS_ADC_DIS_MASK                 0x08
    #define BQ2588X_VSYS_ADC_DIS_SHIFT                3
    #define BQ2588X_VSYS_ADC_ENABLE                   0
    #define BQ2588X_VSYS_ADC_DISABLE                  1
    #define BQ2588X_TS_ADC_DIS_MASK                   0x04
    #define BQ2588X_TS_ADC_DIS_SHIFT                  2
    #define BQ2588X_TS_ADC_ENABLE                     0
    #define BQ2588X_TS_ADC_DISABLE                    1
    #define BQ2588X_TDIE_ADC_DIS_MASK                 0x01
    #define BQ2588X_TDIE_ADC_DIS_SHIFT                0
    #define BQ2588X_TDIE_ADC_ENABLE                   0
    #define BQ2588X_TDIE_ADC_DISABLE                  1

#define BQ2588X_REG_IBUS_ADC_1                        0x17
    #define BQ2588X_IBUS_ADC_HB_MASK                  0xFF
    #define BQ2588X_IBUS_ADC_HB_SHIFT                 0
    #define BQ2588X_IBUS_ADC_HB_BASE                  0
    #define BQ2588X_IBUS_ADC_HB_LSB                   256

#define BQ2588X_REG_IBUS_ADC_0                        0x18
    #define BQ2588X_IBUS_ADC_LB_MASK                  0xFF
    #define BQ2588X_IBUS_ADC_LB_SHIFT                 0
    #define BQ2588X_IBUS_ADC_LB_BASE                  0
    #define BQ2588X_IBUS_ADC_LB_LSB                   1

#define BQ2588X_REG_ICHG_ADC_1                        0x19
    #define BQ2588X_ICHG_ADC_HB_MASK                  0xFF
    #define BQ2588X_ICHG_ADC_HB_SHIFT                 0
    #define BQ2588X_ICHG_ADC_HB_BASE                  0
    #define BQ2588X_ICHG_ADC_HB_LSB                   256

#define BQ2588X_REG_ICHG_ADC_0                        0x1A
    #define BQ2588X_ICHG_ADC_LB_MASK                  0xFF
    #define BQ2588X_ICHG_ADC_LB_SHIFT                 0
    #define BQ2588X_ICHG_ADC_LB_BASE                  0
    #define BQ2588X_ICHG_ADC_LB_LSB                   1

#define BQ2588X_REG_VBUS_ADC_1                        0x1B
    #define BQ2588X_VBUS_ADC_HB_MASK                  0xFF
    #define BQ2588X_VBUS_ADC_HB_SHIFT                 0
    #define BQ2588X_VBUS_ADC_HB_BASE                  0
    #define BQ2588X_VBUS_ADC_HB_LSB                   256

#define BQ2588X_REG_VBUS_ADC_0                        0x1C
    #define BQ2588X_VBUS_ADC_LB_MASK                  0xFF
    #define BQ2588X_VBUS_ADC_LB_SHIFT                 0
    #define BQ2588X_VBUS_ADC_LB_BASE                  0
    #define BQ2588X_VBUS_ADC_LB_LSB                   1

#define BQ2588X_REG_VBAT_ADC_1                        0x1D
    #define BQ2588X_VBAT_ADC_HB_MASK                  0xFF
    #define BQ2588X_VBAT_ADC_HB_SHIFT                 0
    #define BQ2588X_VBAT_ADC_HB_BASE                  0
    #define BQ2588X_VBAT_ADC_HB_LSB                   256

#define BQ2588X_REG_VBAT_ADC_0                        0x1E
    #define BQ2588X_VBAT_ADC_LB_MASK                  0xFF
    #define BQ2588X_VBAT_ADC_LB_SHIFT                 0
    #define BQ2588X_VBAT_ADC_LB_BASE                  0
    #define BQ2588X_VBAT_ADC_LB_LSB                   1

#define BQ2588X_REG_VSYS_ADC_1                        0x1F
    #define BQ2588X_VSYS_ADC_HB_MASK                  0xFF
    #define BQ2588X_VSYS_ADC_HB_SHIFT                 0
    #define BQ2588X_VSYS_ADC_HB_BASE                  0
    #define BQ2588X_VSYS_ADC_HB_LSB                   256

#define BQ2588X_REG_VSYS_ADC_0                        0x20
    #define BQ2588X_VSYS_ADC_LB_MASK                  0xFF
    #define BQ2588X_VSYS_ADC_LB_SHIFT                 0
    #define BQ2588X_VSYS_ADC_LB_BASE                  0
    #define BQ2588X_VSYS_ADC_LB_LSB                   1

#define BQ2588X_REG_TS_ADC_1                          0x21
    #define BQ2588X_TS_ADC_HB_MASK                    0xFF
    #define BQ2588X_TS_ADC_HB_SHIFT                   0
    #define BQ2588X_TS_ADC_HB_BASE                    0
    #define BQ2588X_TS_ADC_HB_LSB                     25

#define BQ2588X_REG_TS_ADC_0                          0x22
    #define BQ2588X_TS_ADC_LB_MASK                    0xFF
    #define BQ2588X_TS_ADC_LB_SHIFT                   0
    #define BQ2588X_TS_ADC_LB_BASE                    0
    #define BQ2588X_TS_ADC_LB_LSB                     (25/256)

#define BQ2588X_REG_TDIE_ADC_1                        0x23
    #define BQ2588X_TDIE_ADC_HB_MASK                  0xFF
    #define BQ2588X_TDIE_ADC_HB_SHIFT                 0
    #define BQ2588X_TDIE_ADC_HB_BASE                  0
    #define BQ2588X_TDIE_ADC_HB_LSB                   128

#define BQ2588X_REG_TDIE_ADC_0                        0x24
    #define BQ2588X_TDIE_ADC_LB_MASK                  0xFF
    #define BQ2588X_TDIE_ADC_LB_SHIFT                 0
    #define BQ2588X_TDIE_ADC_LB_BASE                  0
    #define BQ2588X_TDIE_ADC_LB_LSB                   (1/2)

#define BQ2588X_REG_PART_NUM                          0x25
    #define BQ2588X_REG_RESET_MASK                    0x80
    #define BQ2588X_REG_RESET_SHIFT                   7
    #define BQ2588X_REG_RESET_IDLE                    0
    #define BQ2588X_REG_RESET_KICK                    1
    #define BQ2588X_PART_NO_MASK                      0x38
    #define BQ2588X_PART_NO_SHIFT                     3
    #define BQ2588X_PART_NO_BQ25880                   1
    #define BQ2588X_PART_NO_BQ25882                   2
    #define BQ2588X_REVISION_MASK                     0x07
    #define BQ2588X_REVISION_SHIFT                    0
    #define BQ2588X_REV_1P0                           0
    #define BQ2588X_REV_1P1                           1
        
    

void bq25883_init( void );
void bq25883_write_reg(int reg, int d);
void bq25883_read_reg(uint8_t reg, uint8_t* d, int num_regs);
void bq25883_read_all_reg();

/* [] END OF FILE */
