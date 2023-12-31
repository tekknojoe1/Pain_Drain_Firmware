/*******************************************************************************
* File Name: cyfitter.h
* 
* PSoC Creator  4.4
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2020 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#ifndef INCLUDED_CYFITTER_H
#define INCLUDED_CYFITTER_H
#include "cy_device_headers.h"
#include "cyfitter_gpio.h"
#include "cyfitter_sysint.h"

/* ADC */
#define ADC_intSarClock__DIV_IDX 4
#define ADC_intSarClock__DIV_NUM 4
#define ADC_intSarClock__DIV_TYPE CY_SYSCLK_DIV_8_BIT
#define ADC_SAR__HW SAR
#define ADC_SAR__IDX 0
#define ADC_SAR__TR_SAR_IN TRIG6_OUT_PASS_TR_SAR_IN

/* BLE */
#define BLE_cy_mxs40_ble__HW BLE
#define BLE_cy_mxs40_ble__IDX 0

/* I2S */
#define I2S_cy_mxs40_i2s__HW I2S
#define I2S_cy_mxs40_i2s__IDX 0
#define I2S_cy_mxs40_i2s__TR_I2S_TX_REQ TRIG13_IN_AUDIOSS_TR_I2S_TX_REQ

/* SPI */
#define SPI_SCB__HW SCB2
#define SPI_SCB__IDX 2
#define SPI_SCB__SS0_POSITION 0u
#define SPI_SCB__SS1_POSITION 1u
#define SPI_SCB__SS2_POSITION 2u
#define SPI_SCB__SS3_POSITION 3u
#define SPI_SCBCLK__DIV_IDX 3
#define SPI_SCBCLK__DIV_NUM 3
#define SPI_SCBCLK__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* AMP_PWM */
#define AMP_PWM_TCPWM__CNT_HW TCPWM0_CNT7
#define AMP_PWM_TCPWM__CNT_IDX 7u
#define AMP_PWM_TCPWM__HW TCPWM0
#define AMP_PWM_TCPWM__IDX 0u

/* Clock_1 */
#define Clock_1__DIV_IDX 1
#define Clock_1__DIV_NUM 1
#define Clock_1__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* Clock_2 */
#define Clock_2__DIV_IDX 8
#define Clock_2__DIV_NUM 0
#define Clock_2__DIV_TYPE CY_SYSCLK_DIV_16_BIT

/* Clock_3 */
#define Clock_3__DIV_IDX 2
#define Clock_3__DIV_NUM 2
#define Clock_3__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* Clock_4 */
#define Clock_4__DIV_IDX 0
#define Clock_4__DIV_NUM 0
#define Clock_4__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* Clock_5 */
#define Clock_5__DIV_IDX 7
#define Clock_5__DIV_NUM 7
#define Clock_5__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* Clock_6 */
#define Clock_6__DIV_IDX 5
#define Clock_6__DIV_NUM 5
#define Clock_6__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* Clock_7 */
#define Clock_7__DIV_IDX 6
#define Clock_7__DIV_NUM 6
#define Clock_7__DIV_TYPE CY_SYSCLK_DIV_8_BIT

/* LCD_PWM */
#define LCD_PWM_TCPWM__CNT_HW TCPWM1_CNT1
#define LCD_PWM_TCPWM__CNT_IDX 1u
#define LCD_PWM_TCPWM__HW TCPWM1
#define LCD_PWM_TCPWM__IDX 1u

/* PWM_FAN */
#define PWM_FAN_TCPWM__CNT_HW TCPWM0_CNT0
#define PWM_FAN_TCPWM__CNT_IDX 0u
#define PWM_FAN_TCPWM__HW TCPWM0
#define PWM_FAN_TCPWM__IDX 0u

/* DMA_Play */
#define DMA_Play_DW__BLOCK_HW DW0
#define DMA_Play_DW__BLOCK_NUMBER 0u
#define DMA_Play_DW__CHANNEL_HW DW0_CH_STRUCT0
#define DMA_Play_DW__CHANNEL_NUMBER 0u
#define DMA_Play_DW__TR_IN TRIG0_OUT_CPUSS_DW0_TR_IN0

/* PWM_PEL1 */
#define PWM_PEL1_TCPWM__CNT_HW TCPWM0_CNT1
#define PWM_PEL1_TCPWM__CNT_IDX 1u
#define PWM_PEL1_TCPWM__HW TCPWM0
#define PWM_PEL1_TCPWM__IDX 0u

/* PWM_PEL2 */
#define PWM_PEL2_TCPWM__CNT_HW TCPWM0_CNT2
#define PWM_PEL2_TCPWM__CNT_IDX 2u
#define PWM_PEL2_TCPWM__HW TCPWM0
#define PWM_PEL2_TCPWM__IDX 0u

/* PWM_TENS */
#define PWM_TENS_TCPWM__CNT_HW TCPWM1_CNT7
#define PWM_TENS_TCPWM__CNT_IDX 7u
#define PWM_TENS_TCPWM__HW TCPWM1
#define PWM_TENS_TCPWM__IDX 1u

/* PWM_TENS2 */
#define PWM_TENS2_TCPWM__CNT_HW TCPWM0_CNT4
#define PWM_TENS2_TCPWM__CNT_IDX 4u
#define PWM_TENS2_TCPWM__HW TCPWM0
#define PWM_TENS2_TCPWM__IDX 0u

/* Miscellaneous */
#define CY_PROJECT_NAME "PainDrain"
#define CY_VERSION "PSoC Creator  4.4"
#define CYDEV_CHIP_DIE_LEOPARD 1u
#define CYDEV_CHIP_DIE_PSOC4A 26u
#define CYDEV_CHIP_DIE_PSOC5LP 2u
#define CYDEV_CHIP_DIE_PSOC5TM 3u
#define CYDEV_CHIP_DIE_TMA4 4u
#define CYDEV_CHIP_DIE_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_FM0P 5u
#define CYDEV_CHIP_FAMILY_FM3 6u
#define CYDEV_CHIP_FAMILY_FM4 7u
#define CYDEV_CHIP_FAMILY_PSOC3 1u
#define CYDEV_CHIP_FAMILY_PSOC4 2u
#define CYDEV_CHIP_FAMILY_PSOC5 3u
#define CYDEV_CHIP_FAMILY_PSOC6 4u
#define CYDEV_CHIP_FAMILY_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_USED CYDEV_CHIP_FAMILY_PSOC6
#define CYDEV_CHIP_JTAG_ID 0xE2F02100u
#define CYDEV_CHIP_MEMBER_3A 1u
#define CYDEV_CHIP_MEMBER_4A 26u
#define CYDEV_CHIP_MEMBER_4AA 25u
#define CYDEV_CHIP_MEMBER_4AB 30u
#define CYDEV_CHIP_MEMBER_4AC 14u
#define CYDEV_CHIP_MEMBER_4AD 15u
#define CYDEV_CHIP_MEMBER_4AE 16u
#define CYDEV_CHIP_MEMBER_4D 20u
#define CYDEV_CHIP_MEMBER_4E 6u
#define CYDEV_CHIP_MEMBER_4F 27u
#define CYDEV_CHIP_MEMBER_4G 4u
#define CYDEV_CHIP_MEMBER_4H 24u
#define CYDEV_CHIP_MEMBER_4I 32u
#define CYDEV_CHIP_MEMBER_4J 21u
#define CYDEV_CHIP_MEMBER_4K 22u
#define CYDEV_CHIP_MEMBER_4L 31u
#define CYDEV_CHIP_MEMBER_4M 29u
#define CYDEV_CHIP_MEMBER_4N 11u
#define CYDEV_CHIP_MEMBER_4O 8u
#define CYDEV_CHIP_MEMBER_4P 28u
#define CYDEV_CHIP_MEMBER_4Q 17u
#define CYDEV_CHIP_MEMBER_4R 9u
#define CYDEV_CHIP_MEMBER_4S 12u
#define CYDEV_CHIP_MEMBER_4T 10u
#define CYDEV_CHIP_MEMBER_4U 5u
#define CYDEV_CHIP_MEMBER_4V 23u
#define CYDEV_CHIP_MEMBER_4W 13u
#define CYDEV_CHIP_MEMBER_4X 7u
#define CYDEV_CHIP_MEMBER_4Y 18u
#define CYDEV_CHIP_MEMBER_4Z 19u
#define CYDEV_CHIP_MEMBER_5A 3u
#define CYDEV_CHIP_MEMBER_5B 2u
#define CYDEV_CHIP_MEMBER_6A 33u
#define CYDEV_CHIP_MEMBER_FM3 37u
#define CYDEV_CHIP_MEMBER_FM4 38u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE1 34u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE2 35u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE3 36u
#define CYDEV_CHIP_MEMBER_UNKNOWN 0u
#define CYDEV_CHIP_MEMBER_USED CYDEV_CHIP_MEMBER_6A
#define CYDEV_CHIP_DIE_EXPECT CYDEV_CHIP_MEMBER_USED
#define CYDEV_CHIP_DIE_ACTUAL CYDEV_CHIP_DIE_EXPECT
#define CYDEV_CHIP_REV_LEOPARD_ES1 0u
#define CYDEV_CHIP_REV_LEOPARD_ES2 1u
#define CYDEV_CHIP_REV_LEOPARD_ES3 3u
#define CYDEV_CHIP_REV_LEOPARD_PRODUCTION 3u
#define CYDEV_CHIP_REV_PSOC4A_ES0 17u
#define CYDEV_CHIP_REV_PSOC4A_PRODUCTION 17u
#define CYDEV_CHIP_REV_PSOC5LP_ES0 0u
#define CYDEV_CHIP_REV_PSOC5LP_PRODUCTION 0u
#define CYDEV_CHIP_REV_PSOC5TM_ES0 0u
#define CYDEV_CHIP_REV_PSOC5TM_ES1 1u
#define CYDEV_CHIP_REV_PSOC5TM_PRODUCTION 1u
#define CYDEV_CHIP_REV_TMA4_ES 17u
#define CYDEV_CHIP_REV_TMA4_ES2 33u
#define CYDEV_CHIP_REV_TMA4_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_3A_ES1 0u
#define CYDEV_CHIP_REVISION_3A_ES2 1u
#define CYDEV_CHIP_REVISION_3A_ES3 3u
#define CYDEV_CHIP_REVISION_3A_PRODUCTION 3u
#define CYDEV_CHIP_REVISION_4A_ES0 17u
#define CYDEV_CHIP_REVISION_4A_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_4AA_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AB_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AC_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AD_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AE_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4D_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4E_CCG2_NO_USBPD 0u
#define CYDEV_CHIP_REVISION_4E_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION_256K 0u
#define CYDEV_CHIP_REVISION_4G_ES 17u
#define CYDEV_CHIP_REVISION_4G_ES2 33u
#define CYDEV_CHIP_REVISION_4G_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_4H_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4I_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4J_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4K_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4L_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4M_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4N_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4O_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4P_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Q_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4R_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4S_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4T_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4U_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4V_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4W_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4X_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Y_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Z_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_5A_ES0 0u
#define CYDEV_CHIP_REVISION_5A_ES1 1u
#define CYDEV_CHIP_REVISION_5A_PRODUCTION 1u
#define CYDEV_CHIP_REVISION_5B_ES0 0u
#define CYDEV_CHIP_REVISION_5B_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_6A_ES 17u
#define CYDEV_CHIP_REVISION_6A_NO_UDB 33u
#define CYDEV_CHIP_REVISION_6A_PRODUCTION 33u
#define CYDEV_CHIP_REVISION_FM3_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_FM4_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE1_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE2_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE3_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_USED CYDEV_CHIP_REVISION_6A_PRODUCTION
#define CYDEV_CHIP_REV_EXPECT CYDEV_CHIP_REVISION_USED
#define CYDEV_CLK_ALTSYSTICKCLK__HZ 32768U
#define CYDEV_CLK_ALTSYSTICKCLK__KHZ 32U
#define CYDEV_CLK_ALTSYSTICKCLK__MHZ 0U
#define CYDEV_CLK_BAKCLK__HZ 32768U
#define CYDEV_CLK_BAKCLK__KHZ 32U
#define CYDEV_CLK_BAKCLK__MHZ 0U
#define CYDEV_CLK_FASTCLK__HZ 50000000U
#define CYDEV_CLK_FASTCLK__KHZ 50000U
#define CYDEV_CLK_FASTCLK__MHZ 50U
#define CYDEV_CLK_FLL__HZ 50000000U
#define CYDEV_CLK_FLL__KHZ 50000U
#define CYDEV_CLK_FLL__MHZ 50U
#define CYDEV_CLK_HFCLK0__HZ 50000000U
#define CYDEV_CLK_HFCLK0__KHZ 50000U
#define CYDEV_CLK_HFCLK0__MHZ 50U
#define CYDEV_CLK_HFCLK1__HZ 16307692U
#define CYDEV_CLK_HFCLK1__KHZ 16307U
#define CYDEV_CLK_HFCLK1__MHZ 16U
#define CYDEV_CLK_HFCLK4__HZ 2038461U
#define CYDEV_CLK_HFCLK4__KHZ 2038U
#define CYDEV_CLK_HFCLK4__MHZ 2U
#define CYDEV_CLK_ILO__HZ 32000U
#define CYDEV_CLK_ILO__KHZ 32U
#define CYDEV_CLK_ILO__MHZ 0U
#define CYDEV_CLK_IMO__HZ 8000000U
#define CYDEV_CLK_IMO__KHZ 8000U
#define CYDEV_CLK_IMO__MHZ 8U
#define CYDEV_CLK_LFCLK__HZ 32768U
#define CYDEV_CLK_LFCLK__KHZ 32U
#define CYDEV_CLK_LFCLK__MHZ 0U
#define CYDEV_CLK_PATHMUX0__HZ 8000000U
#define CYDEV_CLK_PATHMUX0__KHZ 8000U
#define CYDEV_CLK_PATHMUX0__MHZ 8U
#define CYDEV_CLK_PATHMUX1__HZ 8000000U
#define CYDEV_CLK_PATHMUX1__KHZ 8000U
#define CYDEV_CLK_PATHMUX1__MHZ 8U
#define CYDEV_CLK_PATHMUX2__HZ 8000000U
#define CYDEV_CLK_PATHMUX2__KHZ 8000U
#define CYDEV_CLK_PATHMUX2__MHZ 8U
#define CYDEV_CLK_PATHMUX3__HZ 8000000U
#define CYDEV_CLK_PATHMUX3__KHZ 8000U
#define CYDEV_CLK_PATHMUX3__MHZ 8U
#define CYDEV_CLK_PATHMUX4__HZ 8000000U
#define CYDEV_CLK_PATHMUX4__KHZ 8000U
#define CYDEV_CLK_PATHMUX4__MHZ 8U
#define CYDEV_CLK_PERICLK__HZ 25000000U
#define CYDEV_CLK_PERICLK__KHZ 25000U
#define CYDEV_CLK_PERICLK__MHZ 25U
#define CYDEV_CLK_PILO__HZ 32768U
#define CYDEV_CLK_PILO__KHZ 32U
#define CYDEV_CLK_PILO__MHZ 0U
#define CYDEV_CLK_PLL0__HZ 16307692U
#define CYDEV_CLK_PLL0__KHZ 16307U
#define CYDEV_CLK_PLL0__MHZ 16U
#define CYDEV_CLK_PUMPCLK__HZ 25000000U
#define CYDEV_CLK_PUMPCLK__KHZ 25000U
#define CYDEV_CLK_PUMPCLK__MHZ 25U
#define CYDEV_CLK_SLOWCLK__HZ 25000000U
#define CYDEV_CLK_SLOWCLK__KHZ 25000U
#define CYDEV_CLK_SLOWCLK__MHZ 25U
#define CYDEV_CLK_TIMERCLK__HZ 8000000U
#define CYDEV_CLK_TIMERCLK__KHZ 8000U
#define CYDEV_CLK_TIMERCLK__MHZ 8U
#define CYDEV_CONFIG_READ_ACCELERATOR 1
#define CYDEV_CONFIG_UNUSED_IO_AllowButWarn 0
#define CYDEV_CONFIG_UNUSED_IO CYDEV_CONFIG_UNUSED_IO_AllowButWarn
#define CYDEV_CONFIG_UNUSED_IO_AllowWithInfo 1
#define CYDEV_CONFIG_UNUSED_IO_Disallowed 2
#define CYDEV_CONFIGURATION_COMPRESSED 1
#define CYDEV_CONFIGURATION_MODE_COMPRESSED 0
#define CYDEV_CONFIGURATION_MODE CYDEV_CONFIGURATION_MODE_COMPRESSED
#define CYDEV_CONFIGURATION_MODE_DMA 2
#define CYDEV_CONFIGURATION_MODE_UNCOMPRESSED 1
#define CYDEV_CORE_VOLTAGE CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V
#define CYDEV_DEBUGGING_DPS_Disable 3
#define CYDEV_DEBUGGING_DPS CYDEV_DEBUGGING_DPS_Disable
#define CYDEV_DEBUGGING_DPS_JTAG_4 1
#define CYDEV_DEBUGGING_DPS_JTAG_5 0
#define CYDEV_DEBUGGING_DPS_SWD 2
#define CYDEV_DEBUGGING_DPS_SWD_SWV 6
#define CYDEV_DEBUGGING_ENABLE 0
#define CYDEV_ENABLE_SIMO_BUCK true
#define CYDEV_IS_EXPORTING_CODE 0
#define CYDEV_IS_IMPORTING_CODE 0
#define CYDEV_PMIC_ENABLED 0
#define CYDEV_PMIC_MODE CYDEV_PMIC_MODE_DISABLED
#define CYDEV_POWER_MODE CYDEV_POWER_MODE_SIMO_BUCK_0_9V
#define CYDEV_PROJ_TYPE 0
#define CYDEV_PROJ_TYPE_BOOTLOADER 1
#define CYDEV_PROJ_TYPE_LAUNCHER 5
#define CYDEV_PROJ_TYPE_LOADABLE 2
#define CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER 4
#define CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER 3
#define CYDEV_PROJ_TYPE_STANDARD 0
#define CYDEV_SYSTEM_AREF_CURRENT_HIGH 1
#define CYDEV_SYSTEM_AREF_CURRENT CYDEV_SYSTEM_AREF_CURRENT_HIGH
#define CYDEV_SYSTEM_AREF_CURRENT_LOW 0
#define CYDEV_SYSTEM_AREF_DEEPSLEEP 0
#define CYDEV_SYSTEM_AREF_SOURCE CY_SYSANALOG_VREF_SOURCE_LOCAL_1_2V
#define CYDEV_VARIABLE_VDDA 0
#define CYDEV_VBAC_SUPPLY CYDEV_VBAC_SUPPLY_VDDD
#define CYDEV_VBACKUP 3.3
#define CYDEV_VBACKUP_MV 3300
#define CYDEV_VDD 3.3
#define CYDEV_VDD_MV 3300
#define SRSS__HW SRSS0
#define SRSS__IDX 0
#define CYDEV_BOOTLOADER_ENABLE 0

#endif /* INCLUDED_CYFITTER_H */
