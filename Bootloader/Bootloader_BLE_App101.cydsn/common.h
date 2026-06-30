/*******************************************************************************
* File Name: common.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#ifndef COMMON_H
#define COMMON_H

#include <project.h>
#include <stdio.h>

/***************************************
* Conditional Compilation Parameters
***************************************/
#define ENABLED                         (1u)
#define DISABLED                        (0u)
#define DEBUG_UART_ENABLED              ENABLED

  
/***************************************
*           API Constants
***************************************/
#define ADV_TIMER_TIMEOUT               (1u)                /* Сounts in seconds */
#define SW2_PRESS_TIME_DEL_BOND_LIST    (0x04u)

/* BAS service defines */
#define BATTERY_TIMEOUT                 (30u)               /* Battery simulation timeout */

/* Dual-app bootloader memory map (1 MB flash, 0x10000000-0x10100000)
 *
 *   Bootloader : 0x10000000 - 0x1000FFFF  (64 KB)  this project, CM0+ only, no BLE
 *   App0 slot  : 0x10010000 - 0x10087FFF  (480 KB) PainDrain built for slot 0
 *   App1 slot  : 0x10088000 - 0x100FFFFF  (480 KB) PainDrain built for slot 1
 *
 * Each 480 KB slot holds a full dual-core app (vector table at slot start):
 *   App0  CM0+ : 0x10010000 (128 KB)   App0 CM4 : 0x10030000 (~352 KB)
 *   App1  CM0+ : 0x10088000 (128 KB)   App1 CM4 : 0x100A8000 (~352 KB)
 *   Metadata is the reserved last 256 B of each slot.
 */
#define APP_SLOT_SIZE                   (0x78000u)          /* 480 KB per slot          */

#define APP0_START_ADDR                 (0x10010000u)       /* App0 CM0+ vector table   */
#define APP0_CM4_ADDR                   (0x10030000u)       /* App0 CM4 vector table    */
#define APP0_META_ADDR                  (0x10087F00u)       /* App0 metadata (last row) */

#define APP1_START_ADDR                 (0x10088000u)       /* App1 CM0+ vector table   */
#define APP1_CM4_ADDR                   (0x100A8000u)       /* App1 CM4 vector table    */
#define APP1_META_ADDR                  (0x100FFF00u)       /* App1 metadata (last row) */

/***************************************
*        External Function Prototypes
***************************************/
int HostMain(void);

/* Function prototypes from debug.c */
void ShowValue(cy_stc_ble_gatt_value_t *value);
char HexToAscii(uint8_t value, uint8_t nibble);
void Set32ByPtr(uint8_t ptr[], uint32_t value);
void PrintStackVersion(void);
void PrintApiResult(cy_en_ble_api_result_t apiResult);
void ShowError(void);

/* Function prototypes from bond.c */
void App_DisplayBondList(void);
void App_RemoveDevicesFromBondListBySW2Press(uint32_t seconds);
void App_RemoveDevicesFromBondList(void);
void App_SetRemoveBondListFlag(void);
bool App_IsRemoveBondListFlag(void);
bool App_IsDeviceInBondList(uint32_t bdHandle);
uint32_t App_GetCountOfBondedDevices(void);


/***************************************
*   UART_DEB Macros / prototypes
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)                 (printf(__VA_ARGS__))
    #define UART_DEB_PUT_CHAR(...)           while(1UL != UART_DEB_Put(__VA_ARGS__))
    #define UART_DEB_GET_CHAR(...)          (UART_DEB_Get())
    #define UART_DEB_IS_TX_COMPLETE(...)    (UART_DEB_IsTxComplete())
    #define UART_DEB_WAIT_TX_COMPLETE(...)   while(UART_DEB_IS_TX_COMPLETE() == 0) ;    
    #define UART_DEB_SCB_CLEAR_RX_FIFO(...) (Cy_SCB_ClearRxFifo(UART_DEB_SCB__HW))
    #define UART_START(...)                 (UART_DEB_Start(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
    #define UART_DEB_PUT_CHAR(...)
    #define UART_DEB_GET_CHAR(...)          (0u)
    #define UART_DEB_IS_TX_COMPLETE(...)    (1u)
    #define UART_DEB_WAIT_TX_COMPLETE(...)  (0u)
    #define UART_DEB_SCB_CLEAR_RX_FIFO(...) (0u)
    #define UART_START(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */

#define UART_DEB_NO_DATA                (char8) CY_SCB_UART_RX_NO_DATA


/***************************************
* External data references
***************************************/
extern cy_stc_ble_conn_handle_t appConnHandle;


#endif /* COMMON_H */

/* [] END OF FILE */
