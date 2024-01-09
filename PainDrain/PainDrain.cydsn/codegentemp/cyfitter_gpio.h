/*******************************************************************************
* File Name: cyfitter_gpio.h
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

#ifndef INCLUDED_CYFITTER_GPIO_H
#define INCLUDED_CYFITTER_GPIO_H
#include "cy_device_headers.h"

/* PWR */
#define PWR_0_DRIVEMODE CY_GPIO_DM_PULLUP
#define PWR_0_INBUF_ENABLED 1u
#define PWR_0_INIT_DRIVESTATE 1u
#define PWR_0_INIT_MUXSEL 0u
#define PWR_0_INPUT_SYNC 2u
#define PWR_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PWR_0_NUM 4u
#define PWR_0_PORT GPIO_PRT0
#define PWR_0_SLEWRATE CY_GPIO_SLEW_FAST
#define PWR_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define PWR_BTN_0_DRIVEMODE CY_GPIO_DM_PULLUP
#define PWR_BTN_0_INBUF_ENABLED 1u
#define PWR_BTN_0_INIT_DRIVESTATE 1u
#define PWR_BTN_0_INIT_MUXSEL 0u
#define PWR_BTN_0_INPUT_SYNC 2u
#define PWR_BTN_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PWR_BTN_0_NUM 7u
#define PWR_BTN_0_PORT GPIO_PRT7
#define PWR_BTN_0_SLEWRATE CY_GPIO_SLEW_FAST
#define PWR_BTN_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define PWR_BTN_DRIVEMODE CY_GPIO_DM_PULLUP
#define PWR_BTN_INBUF_ENABLED 1u
#define PWR_BTN_INIT_DRIVESTATE 1u
#define PWR_BTN_INIT_MUXSEL 0u
#define PWR_BTN_INPUT_SYNC 2u
#define PWR_BTN_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PWR_BTN_NUM 7u
#define PWR_BTN_PORT GPIO_PRT7
#define PWR_BTN_SLEWRATE CY_GPIO_SLEW_FAST
#define PWR_BTN_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define PWR_DRIVEMODE CY_GPIO_DM_PULLUP
#define PWR_INBUF_ENABLED 1u
#define PWR_INIT_DRIVESTATE 1u
#define PWR_INIT_MUXSEL 0u
#define PWR_INPUT_SYNC 2u
#define PWR_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PWR_NUM 4u
#define PWR_PORT GPIO_PRT0
#define PWR_SLEWRATE CY_GPIO_SLEW_FAST
#define PWR_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* PEL_1 */
#define PEL_1_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define PEL_1_0_INBUF_ENABLED 0u
#define PEL_1_0_INIT_DRIVESTATE 1u
#define PEL_1_0_INIT_MUXSEL 3u
#define PEL_1_0_INPUT_SYNC 2u
#define PEL_1_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PEL_1_0_NUM 5u
#define PEL_1_0_PORT GPIO_PRT9
#define PEL_1_0_SLEWRATE CY_GPIO_SLEW_FAST
#define PEL_1_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define PEL_1_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define PEL_1_INBUF_ENABLED 0u
#define PEL_1_INIT_DRIVESTATE 1u
#define PEL_1_INIT_MUXSEL 3u
#define PEL_1_INPUT_SYNC 2u
#define PEL_1_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PEL_1_NUM 5u
#define PEL_1_PORT GPIO_PRT9
#define PEL_1_SLEWRATE CY_GPIO_SLEW_FAST
#define PEL_1_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* PEL_2 */
#define PEL_2_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define PEL_2_0_INBUF_ENABLED 0u
#define PEL_2_0_INIT_DRIVESTATE 1u
#define PEL_2_0_INIT_MUXSEL 3u
#define PEL_2_0_INPUT_SYNC 2u
#define PEL_2_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PEL_2_0_NUM 1u
#define PEL_2_0_PORT GPIO_PRT9
#define PEL_2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define PEL_2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define PEL_2_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define PEL_2_INBUF_ENABLED 0u
#define PEL_2_INIT_DRIVESTATE 1u
#define PEL_2_INIT_MUXSEL 3u
#define PEL_2_INPUT_SYNC 2u
#define PEL_2_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define PEL_2_NUM 1u
#define PEL_2_PORT GPIO_PRT9
#define PEL_2_SLEWRATE CY_GPIO_SLEW_FAST
#define PEL_2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* AMP_WS */
#define AMP_WS_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_WS_0_INBUF_ENABLED 0u
#define AMP_WS_0_INIT_DRIVESTATE 1u
#define AMP_WS_0_INIT_MUXSEL 22u
#define AMP_WS_0_INPUT_SYNC 2u
#define AMP_WS_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_WS_0_NUM 2u
#define AMP_WS_0_PORT GPIO_PRT5
#define AMP_WS_0_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_WS_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define AMP_WS_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_WS_INBUF_ENABLED 0u
#define AMP_WS_INIT_DRIVESTATE 1u
#define AMP_WS_INIT_MUXSEL 22u
#define AMP_WS_INPUT_SYNC 2u
#define AMP_WS_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_WS_NUM 2u
#define AMP_WS_PORT GPIO_PRT5
#define AMP_WS_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_WS_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* CHG_CE */
#define CHG_CE_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CHG_CE_0_INBUF_ENABLED 0u
#define CHG_CE_0_INIT_DRIVESTATE 0u
#define CHG_CE_0_INIT_MUXSEL 0u
#define CHG_CE_0_INPUT_SYNC 2u
#define CHG_CE_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_CE_0_NUM 6u
#define CHG_CE_0_PORT GPIO_PRT10
#define CHG_CE_0_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_CE_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define CHG_CE_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CHG_CE_INBUF_ENABLED 0u
#define CHG_CE_INIT_DRIVESTATE 0u
#define CHG_CE_INIT_MUXSEL 0u
#define CHG_CE_INPUT_SYNC 2u
#define CHG_CE_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_CE_NUM 6u
#define CHG_CE_PORT GPIO_PRT10
#define CHG_CE_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_CE_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* CHG_PG */
#define CHG_PG_0_DRIVEMODE CY_GPIO_DM_PULLUP
#define CHG_PG_0_INBUF_ENABLED 1u
#define CHG_PG_0_INIT_DRIVESTATE 1u
#define CHG_PG_0_INIT_MUXSEL 0u
#define CHG_PG_0_INPUT_SYNC 2u
#define CHG_PG_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_PG_0_NUM 1u
#define CHG_PG_0_PORT GPIO_PRT10
#define CHG_PG_0_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_PG_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define CHG_PG_DRIVEMODE CY_GPIO_DM_PULLUP
#define CHG_PG_INBUF_ENABLED 1u
#define CHG_PG_INIT_DRIVESTATE 1u
#define CHG_PG_INIT_MUXSEL 0u
#define CHG_PG_INPUT_SYNC 2u
#define CHG_PG_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_PG_NUM 1u
#define CHG_PG_PORT GPIO_PRT10
#define CHG_PG_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_PG_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* CHG_TS */
#define CHG_TS_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CHG_TS_0_INBUF_ENABLED 0u
#define CHG_TS_0_INIT_DRIVESTATE 0u
#define CHG_TS_0_INIT_MUXSEL 0u
#define CHG_TS_0_INPUT_SYNC 2u
#define CHG_TS_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_TS_0_NUM 3u
#define CHG_TS_0_PORT GPIO_PRT9
#define CHG_TS_0_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_TS_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define CHG_TS_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CHG_TS_INBUF_ENABLED 0u
#define CHG_TS_INIT_DRIVESTATE 0u
#define CHG_TS_INIT_MUXSEL 0u
#define CHG_TS_INPUT_SYNC 2u
#define CHG_TS_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_TS_NUM 3u
#define CHG_TS_PORT GPIO_PRT9
#define CHG_TS_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_TS_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* AMP_MCK */
#define AMP_MCK_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_MCK_0_INBUF_ENABLED 0u
#define AMP_MCK_0_INIT_DRIVESTATE 1u
#define AMP_MCK_0_INIT_MUXSEL 9u
#define AMP_MCK_0_INPUT_SYNC 2u
#define AMP_MCK_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_MCK_0_NUM 4u
#define AMP_MCK_0_PORT GPIO_PRT9
#define AMP_MCK_0_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_MCK_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define AMP_MCK_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_MCK_INBUF_ENABLED 0u
#define AMP_MCK_INIT_DRIVESTATE 1u
#define AMP_MCK_INIT_MUXSEL 9u
#define AMP_MCK_INPUT_SYNC 2u
#define AMP_MCK_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_MCK_NUM 4u
#define AMP_MCK_PORT GPIO_PRT9
#define AMP_MCK_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_MCK_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* AMP_SCK */
#define AMP_SCK_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_SCK_0_INBUF_ENABLED 0u
#define AMP_SCK_0_INIT_DRIVESTATE 1u
#define AMP_SCK_0_INIT_MUXSEL 22u
#define AMP_SCK_0_INPUT_SYNC 2u
#define AMP_SCK_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_SCK_0_NUM 1u
#define AMP_SCK_0_PORT GPIO_PRT5
#define AMP_SCK_0_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_SCK_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define AMP_SCK_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_SCK_INBUF_ENABLED 0u
#define AMP_SCK_INIT_DRIVESTATE 1u
#define AMP_SCK_INIT_MUXSEL 22u
#define AMP_SCK_INPUT_SYNC 2u
#define AMP_SCK_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_SCK_NUM 1u
#define AMP_SCK_PORT GPIO_PRT5
#define AMP_SCK_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_SCK_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* AMP_SDO */
#define AMP_SDO_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_SDO_0_INBUF_ENABLED 0u
#define AMP_SDO_0_INIT_DRIVESTATE 1u
#define AMP_SDO_0_INIT_MUXSEL 22u
#define AMP_SDO_0_INPUT_SYNC 2u
#define AMP_SDO_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_SDO_0_NUM 3u
#define AMP_SDO_0_PORT GPIO_PRT5
#define AMP_SDO_0_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_SDO_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define AMP_SDO_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define AMP_SDO_INBUF_ENABLED 0u
#define AMP_SDO_INIT_DRIVESTATE 1u
#define AMP_SDO_INIT_MUXSEL 22u
#define AMP_SDO_INPUT_SYNC 2u
#define AMP_SDO_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define AMP_SDO_NUM 3u
#define AMP_SDO_PORT GPIO_PRT5
#define AMP_SDO_SLEWRATE CY_GPIO_SLEW_FAST
#define AMP_SDO_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* DISP_CS */
#define DISP_CS_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_CS_0_INBUF_ENABLED 0u
#define DISP_CS_0_INIT_DRIVESTATE 1u
#define DISP_CS_0_INIT_MUXSEL 0u
#define DISP_CS_0_INPUT_SYNC 2u
#define DISP_CS_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_CS_0_NUM 2u
#define DISP_CS_0_PORT GPIO_PRT6
#define DISP_CS_0_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_CS_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define DISP_CS_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_CS_INBUF_ENABLED 0u
#define DISP_CS_INIT_DRIVESTATE 1u
#define DISP_CS_INIT_MUXSEL 0u
#define DISP_CS_INPUT_SYNC 2u
#define DISP_CS_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_CS_NUM 2u
#define DISP_CS_PORT GPIO_PRT6
#define DISP_CS_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_CS_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* FAN_PWM */
#define FAN_PWM_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define FAN_PWM_0_INBUF_ENABLED 0u
#define FAN_PWM_0_INIT_DRIVESTATE 1u
#define FAN_PWM_0_INIT_MUXSEL 3u
#define FAN_PWM_0_INPUT_SYNC 2u
#define FAN_PWM_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define FAN_PWM_0_NUM 3u
#define FAN_PWM_0_PORT GPIO_PRT10
#define FAN_PWM_0_SLEWRATE CY_GPIO_SLEW_FAST
#define FAN_PWM_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define FAN_PWM_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define FAN_PWM_INBUF_ENABLED 0u
#define FAN_PWM_INIT_DRIVESTATE 1u
#define FAN_PWM_INIT_MUXSEL 3u
#define FAN_PWM_INPUT_SYNC 2u
#define FAN_PWM_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define FAN_PWM_NUM 3u
#define FAN_PWM_PORT GPIO_PRT10
#define FAN_PWM_SLEWRATE CY_GPIO_SLEW_FAST
#define FAN_PWM_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* I2C_SCL */
#define I2C_SCL_0_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define I2C_SCL_0_INBUF_ENABLED 1u
#define I2C_SCL_0_INIT_DRIVESTATE 1u
#define I2C_SCL_0_INIT_MUXSEL 0u
#define I2C_SCL_0_INPUT_SYNC 2u
#define I2C_SCL_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define I2C_SCL_0_NUM 4u
#define I2C_SCL_0_PORT GPIO_PRT6
#define I2C_SCL_0_SLEWRATE CY_GPIO_SLEW_FAST
#define I2C_SCL_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define I2C_SCL_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define I2C_SCL_INBUF_ENABLED 1u
#define I2C_SCL_INIT_DRIVESTATE 1u
#define I2C_SCL_INIT_MUXSEL 0u
#define I2C_SCL_INPUT_SYNC 2u
#define I2C_SCL_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define I2C_SCL_NUM 4u
#define I2C_SCL_PORT GPIO_PRT6
#define I2C_SCL_SLEWRATE CY_GPIO_SLEW_FAST
#define I2C_SCL_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* I2C_SDA */
#define I2C_SDA_0_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define I2C_SDA_0_INBUF_ENABLED 1u
#define I2C_SDA_0_INIT_DRIVESTATE 1u
#define I2C_SDA_0_INIT_MUXSEL 0u
#define I2C_SDA_0_INPUT_SYNC 2u
#define I2C_SDA_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define I2C_SDA_0_NUM 5u
#define I2C_SDA_0_PORT GPIO_PRT6
#define I2C_SDA_0_SLEWRATE CY_GPIO_SLEW_FAST
#define I2C_SDA_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define I2C_SDA_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define I2C_SDA_INBUF_ENABLED 1u
#define I2C_SDA_INIT_DRIVESTATE 1u
#define I2C_SDA_INIT_MUXSEL 0u
#define I2C_SDA_INPUT_SYNC 2u
#define I2C_SDA_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define I2C_SDA_NUM 5u
#define I2C_SDA_PORT GPIO_PRT6
#define I2C_SDA_SLEWRATE CY_GPIO_SLEW_FAST
#define I2C_SDA_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* CHG_STAT */
#define CHG_STAT_0_DRIVEMODE CY_GPIO_DM_PULLUP
#define CHG_STAT_0_INBUF_ENABLED 1u
#define CHG_STAT_0_INIT_DRIVESTATE 1u
#define CHG_STAT_0_INIT_MUXSEL 0u
#define CHG_STAT_0_INPUT_SYNC 2u
#define CHG_STAT_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_STAT_0_NUM 0u
#define CHG_STAT_0_PORT GPIO_PRT10
#define CHG_STAT_0_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_STAT_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define CHG_STAT_DRIVEMODE CY_GPIO_DM_PULLUP
#define CHG_STAT_INBUF_ENABLED 1u
#define CHG_STAT_INIT_DRIVESTATE 1u
#define CHG_STAT_INIT_MUXSEL 0u
#define CHG_STAT_INPUT_SYNC 2u
#define CHG_STAT_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define CHG_STAT_NUM 0u
#define CHG_STAT_PORT GPIO_PRT10
#define CHG_STAT_SLEWRATE CY_GPIO_SLEW_FAST
#define CHG_STAT_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* DISP_INT */
#define DISP_INT_0_DRIVEMODE CY_GPIO_DM_HIGHZ
#define DISP_INT_0_INBUF_ENABLED 1u
#define DISP_INT_0_INIT_DRIVESTATE 0u
#define DISP_INT_0_INIT_MUXSEL 0u
#define DISP_INT_0_INPUT_SYNC 2u
#define DISP_INT_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_INT_0_NUM 1u
#define DISP_INT_0_PORT GPIO_PRT7
#define DISP_INT_0_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_INT_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define DISP_INT_DRIVEMODE CY_GPIO_DM_HIGHZ
#define DISP_INT_INBUF_ENABLED 1u
#define DISP_INT_INIT_DRIVESTATE 0u
#define DISP_INT_INIT_MUXSEL 0u
#define DISP_INT_INPUT_SYNC 2u
#define DISP_INT_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_INT_NUM 1u
#define DISP_INT_PORT GPIO_PRT7
#define DISP_INT_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_INT_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* DISP_RST */
#define DISP_RST_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_RST_0_INBUF_ENABLED 0u
#define DISP_RST_0_INIT_DRIVESTATE 1u
#define DISP_RST_0_INIT_MUXSEL 0u
#define DISP_RST_0_INPUT_SYNC 2u
#define DISP_RST_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_RST_0_NUM 4u
#define DISP_RST_0_PORT GPIO_PRT5
#define DISP_RST_0_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_RST_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define DISP_RST_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_RST_INBUF_ENABLED 0u
#define DISP_RST_INIT_DRIVESTATE 1u
#define DISP_RST_INIT_MUXSEL 0u
#define DISP_RST_INPUT_SYNC 2u
#define DISP_RST_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_RST_NUM 4u
#define DISP_RST_PORT GPIO_PRT5
#define DISP_RST_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_RST_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* DISP_SCK */
#define DISP_SCK_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_SCK_0_INBUF_ENABLED 0u
#define DISP_SCK_0_INIT_DRIVESTATE 1u
#define DISP_SCK_0_INIT_MUXSEL 20u
#define DISP_SCK_0_INPUT_SYNC 2u
#define DISP_SCK_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_SCK_0_NUM 2u
#define DISP_SCK_0_PORT GPIO_PRT9
#define DISP_SCK_0_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_SCK_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define DISP_SCK_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_SCK_INBUF_ENABLED 0u
#define DISP_SCK_INIT_DRIVESTATE 1u
#define DISP_SCK_INIT_MUXSEL 20u
#define DISP_SCK_INPUT_SYNC 2u
#define DISP_SCK_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_SCK_NUM 2u
#define DISP_SCK_PORT GPIO_PRT9
#define DISP_SCK_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_SCK_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_PWM */
#define TENS_PWM_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_PWM_0_INBUF_ENABLED 0u
#define TENS_PWM_0_INIT_DRIVESTATE 1u
#define TENS_PWM_0_INIT_MUXSEL 8u
#define TENS_PWM_0_INPUT_SYNC 2u
#define TENS_PWM_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_PWM_0_NUM 6u
#define TENS_PWM_0_PORT GPIO_PRT5
#define TENS_PWM_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_PWM_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_PWM_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_PWM_INBUF_ENABLED 0u
#define TENS_PWM_INIT_DRIVESTATE 1u
#define TENS_PWM_INIT_MUXSEL 8u
#define TENS_PWM_INPUT_SYNC 2u
#define TENS_PWM_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_PWM_NUM 6u
#define TENS_PWM_PORT GPIO_PRT5
#define TENS_PWM_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_PWM_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* BKLGT_PWM */
#define BKLGT_PWM_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define BKLGT_PWM_0_INBUF_ENABLED 0u
#define BKLGT_PWM_0_INIT_DRIVESTATE 1u
#define BKLGT_PWM_0_INIT_MUXSEL 9u
#define BKLGT_PWM_0_INPUT_SYNC 2u
#define BKLGT_PWM_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define BKLGT_PWM_0_NUM 6u
#define BKLGT_PWM_0_PORT GPIO_PRT9
#define BKLGT_PWM_0_SLEWRATE CY_GPIO_SLEW_FAST
#define BKLGT_PWM_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define BKLGT_PWM_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define BKLGT_PWM_INBUF_ENABLED 0u
#define BKLGT_PWM_INIT_DRIVESTATE 1u
#define BKLGT_PWM_INIT_MUXSEL 9u
#define BKLGT_PWM_INPUT_SYNC 2u
#define BKLGT_PWM_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define BKLGT_PWM_NUM 6u
#define BKLGT_PWM_PORT GPIO_PRT9
#define BKLGT_PWM_SLEWRATE CY_GPIO_SLEW_FAST
#define BKLGT_PWM_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* DISP_MOSI */
#define DISP_MOSI_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_MOSI_0_INBUF_ENABLED 0u
#define DISP_MOSI_0_INIT_DRIVESTATE 1u
#define DISP_MOSI_0_INIT_MUXSEL 20u
#define DISP_MOSI_0_INPUT_SYNC 2u
#define DISP_MOSI_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_MOSI_0_NUM 0u
#define DISP_MOSI_0_PORT GPIO_PRT9
#define DISP_MOSI_0_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_MOSI_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define DISP_MOSI_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define DISP_MOSI_INBUF_ENABLED 0u
#define DISP_MOSI_INIT_DRIVESTATE 1u
#define DISP_MOSI_INIT_MUXSEL 20u
#define DISP_MOSI_INPUT_SYNC 2u
#define DISP_MOSI_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define DISP_MOSI_NUM 0u
#define DISP_MOSI_PORT GPIO_PRT9
#define DISP_MOSI_SLEWRATE CY_GPIO_SLEW_FAST
#define DISP_MOSI_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_NEG1 */
#define TENS_NEG1_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_NEG1_0_INBUF_ENABLED 0u
#define TENS_NEG1_0_INIT_DRIVESTATE 0u
#define TENS_NEG1_0_INIT_MUXSEL 0u
#define TENS_NEG1_0_INPUT_SYNC 2u
#define TENS_NEG1_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_NEG1_0_NUM 6u
#define TENS_NEG1_0_PORT GPIO_PRT12
#define TENS_NEG1_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_NEG1_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_NEG1_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_NEG1_INBUF_ENABLED 0u
#define TENS_NEG1_INIT_DRIVESTATE 0u
#define TENS_NEG1_INIT_MUXSEL 0u
#define TENS_NEG1_INPUT_SYNC 2u
#define TENS_NEG1_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_NEG1_NUM 6u
#define TENS_NEG1_PORT GPIO_PRT12
#define TENS_NEG1_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_NEG1_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_NEG2 */
#define TENS_NEG2_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_NEG2_0_INBUF_ENABLED 0u
#define TENS_NEG2_0_INIT_DRIVESTATE 0u
#define TENS_NEG2_0_INIT_MUXSEL 0u
#define TENS_NEG2_0_INPUT_SYNC 2u
#define TENS_NEG2_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_NEG2_0_NUM 5u
#define TENS_NEG2_0_PORT GPIO_PRT5
#define TENS_NEG2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_NEG2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_NEG2_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_NEG2_INBUF_ENABLED 0u
#define TENS_NEG2_INIT_DRIVESTATE 0u
#define TENS_NEG2_INIT_MUXSEL 0u
#define TENS_NEG2_INPUT_SYNC 2u
#define TENS_NEG2_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_NEG2_NUM 5u
#define TENS_NEG2_PORT GPIO_PRT5
#define TENS_NEG2_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_NEG2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_POS1 */
#define TENS_POS1_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_POS1_0_INBUF_ENABLED 0u
#define TENS_POS1_0_INIT_DRIVESTATE 0u
#define TENS_POS1_0_INIT_MUXSEL 0u
#define TENS_POS1_0_INPUT_SYNC 2u
#define TENS_POS1_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_POS1_0_NUM 2u
#define TENS_POS1_0_PORT GPIO_PRT10
#define TENS_POS1_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_POS1_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_POS1_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_POS1_INBUF_ENABLED 0u
#define TENS_POS1_INIT_DRIVESTATE 0u
#define TENS_POS1_INIT_MUXSEL 0u
#define TENS_POS1_INPUT_SYNC 2u
#define TENS_POS1_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_POS1_NUM 2u
#define TENS_POS1_PORT GPIO_PRT10
#define TENS_POS1_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_POS1_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_POS2 */
#define TENS_POS2_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_POS2_0_INBUF_ENABLED 0u
#define TENS_POS2_0_INIT_DRIVESTATE 0u
#define TENS_POS2_0_INIT_MUXSEL 0u
#define TENS_POS2_0_INPUT_SYNC 2u
#define TENS_POS2_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_POS2_0_NUM 7u
#define TENS_POS2_0_PORT GPIO_PRT12
#define TENS_POS2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_POS2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_POS2_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_POS2_INBUF_ENABLED 0u
#define TENS_POS2_INIT_DRIVESTATE 0u
#define TENS_POS2_INIT_MUXSEL 0u
#define TENS_POS2_INPUT_SYNC 2u
#define TENS_POS2_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_POS2_NUM 7u
#define TENS_POS2_PORT GPIO_PRT12
#define TENS_POS2_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_POS2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_PWM2 */
#define TENS_PWM2_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_PWM2_0_INBUF_ENABLED 0u
#define TENS_PWM2_0_INIT_DRIVESTATE 1u
#define TENS_PWM2_0_INIT_MUXSEL 8u
#define TENS_PWM2_0_INPUT_SYNC 2u
#define TENS_PWM2_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_PWM2_0_NUM 0u
#define TENS_PWM2_0_PORT GPIO_PRT5
#define TENS_PWM2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_PWM2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_PWM2_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_PWM2_INBUF_ENABLED 0u
#define TENS_PWM2_INIT_DRIVESTATE 1u
#define TENS_PWM2_INIT_MUXSEL 8u
#define TENS_PWM2_INPUT_SYNC 2u
#define TENS_PWM2_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_PWM2_NUM 0u
#define TENS_PWM2_PORT GPIO_PRT5
#define TENS_PWM2_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_PWM2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_SENSE */
#define TENS_SENSE_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define TENS_SENSE_0_INBUF_ENABLED 0u
#define TENS_SENSE_0_INIT_DRIVESTATE 1u
#define TENS_SENSE_0_INIT_MUXSEL 0u
#define TENS_SENSE_0_INPUT_SYNC 2u
#define TENS_SENSE_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_SENSE_0_NUM 5u
#define TENS_SENSE_0_PORT GPIO_PRT10
#define TENS_SENSE_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_SENSE_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_SENSE_DRIVEMODE CY_GPIO_DM_ANALOG
#define TENS_SENSE_INBUF_ENABLED 0u
#define TENS_SENSE_INIT_DRIVESTATE 1u
#define TENS_SENSE_INIT_MUXSEL 0u
#define TENS_SENSE_INPUT_SYNC 2u
#define TENS_SENSE_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_SENSE_NUM 5u
#define TENS_SENSE_PORT GPIO_PRT10
#define TENS_SENSE_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_SENSE_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* MY_UART_PIN */
#define MY_UART_PIN_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define MY_UART_PIN_0_INBUF_ENABLED 0u
#define MY_UART_PIN_0_INIT_DRIVESTATE 1u
#define MY_UART_PIN_0_INIT_MUXSEL 0u
#define MY_UART_PIN_0_INPUT_SYNC 2u
#define MY_UART_PIN_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define MY_UART_PIN_0_NUM 6u
#define MY_UART_PIN_0_PORT GPIO_PRT6
#define MY_UART_PIN_0_SLEWRATE CY_GPIO_SLEW_FAST
#define MY_UART_PIN_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define MY_UART_PIN_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define MY_UART_PIN_INBUF_ENABLED 0u
#define MY_UART_PIN_INIT_DRIVESTATE 1u
#define MY_UART_PIN_INIT_MUXSEL 0u
#define MY_UART_PIN_INPUT_SYNC 2u
#define MY_UART_PIN_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define MY_UART_PIN_NUM 6u
#define MY_UART_PIN_PORT GPIO_PRT6
#define MY_UART_PIN_SLEWRATE CY_GPIO_SLEW_FAST
#define MY_UART_PIN_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TEMP_USER_EN */
#define TEMP_USER_EN_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TEMP_USER_EN_0_INBUF_ENABLED 0u
#define TEMP_USER_EN_0_INIT_DRIVESTATE 0u
#define TEMP_USER_EN_0_INIT_MUXSEL 0u
#define TEMP_USER_EN_0_INPUT_SYNC 2u
#define TEMP_USER_EN_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TEMP_USER_EN_0_NUM 2u
#define TEMP_USER_EN_0_PORT GPIO_PRT7
#define TEMP_USER_EN_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TEMP_USER_EN_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TEMP_USER_EN_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TEMP_USER_EN_INBUF_ENABLED 0u
#define TEMP_USER_EN_INIT_DRIVESTATE 0u
#define TEMP_USER_EN_INIT_MUXSEL 0u
#define TEMP_USER_EN_INPUT_SYNC 2u
#define TEMP_USER_EN_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TEMP_USER_EN_NUM 2u
#define TEMP_USER_EN_PORT GPIO_PRT7
#define TEMP_USER_EN_SLEWRATE CY_GPIO_SLEW_FAST
#define TEMP_USER_EN_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_USER_EN1 */
#define TENS_USER_EN1_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_USER_EN1_0_INBUF_ENABLED 0u
#define TENS_USER_EN1_0_INIT_DRIVESTATE 0u
#define TENS_USER_EN1_0_INIT_MUXSEL 0u
#define TENS_USER_EN1_0_INPUT_SYNC 2u
#define TENS_USER_EN1_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_USER_EN1_0_NUM 3u
#define TENS_USER_EN1_0_PORT GPIO_PRT6
#define TENS_USER_EN1_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_USER_EN1_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_USER_EN1_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_USER_EN1_INBUF_ENABLED 0u
#define TENS_USER_EN1_INIT_DRIVESTATE 0u
#define TENS_USER_EN1_INIT_MUXSEL 0u
#define TENS_USER_EN1_INPUT_SYNC 2u
#define TENS_USER_EN1_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_USER_EN1_NUM 3u
#define TENS_USER_EN1_PORT GPIO_PRT6
#define TENS_USER_EN1_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_USER_EN1_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* TENS_USER_EN2 */
#define TENS_USER_EN2_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_USER_EN2_0_INBUF_ENABLED 0u
#define TENS_USER_EN2_0_INIT_DRIVESTATE 0u
#define TENS_USER_EN2_0_INIT_MUXSEL 0u
#define TENS_USER_EN2_0_INPUT_SYNC 2u
#define TENS_USER_EN2_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_USER_EN2_0_NUM 0u
#define TENS_USER_EN2_0_PORT GPIO_PRT0
#define TENS_USER_EN2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_USER_EN2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define TENS_USER_EN2_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define TENS_USER_EN2_INBUF_ENABLED 0u
#define TENS_USER_EN2_INIT_DRIVESTATE 0u
#define TENS_USER_EN2_INIT_MUXSEL 0u
#define TENS_USER_EN2_INPUT_SYNC 2u
#define TENS_USER_EN2_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define TENS_USER_EN2_NUM 0u
#define TENS_USER_EN2_PORT GPIO_PRT0
#define TENS_USER_EN2_SLEWRATE CY_GPIO_SLEW_FAST
#define TENS_USER_EN2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

#endif /* INCLUDED_CYFITTER_GPIO_H */
