/*******************************************************************************
* File Name: user_interface.h
*
* Version: 1.10
*
* Description:
*  Contains the function prototypes and constants for user interface related source.
*
* Hardware Dependency:
*  CY8CKIT-062 PSoC6 BLE Pioneer Kit
*  CY8CPROTO-063-BLE PSoC 6 BLE Prototyping Kit
*
********************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "common.h"

/***************************************
*           API Constants
***************************************/
#define LED_ON                          (0u)
#define LED_OFF                         (1u)
#define RGB_LED_MIN_VOLTAGE_MV          (2700u)
#define SW2_PRESS_TIME_DEL_BOND_LIST    (4u)

#define UI_SCREEN_WIDTH                 160
#define UI_SCREEN_HEIGHT                79
#define UI_CHAR_HEIGHT                  12
#define UI_NUM_BUTTONS                  3
#define UI_TITLE_BLUETOOTH_ICON_WIDTH   16
#define UI_TITLE_BATTERY_ICON_WIDTH     32
#define UI_TITLE_WIDTH                  (UI_SCREEN_WIDTH - 1 - UI_TITLE_BLUETOOTH_ICON_WIDTH - UI_TITLE_BATTERY_ICON_WIDTH)
#define UI_TITLE_HEIGHT                 (UI_CHAR_HEIGHT+2)
#define UI_BUTTON_WIDTH                 49
#define UI_LEFT_BUTTON_X                2
#define UI_CENTER_BUTTON_X              55
#define UI_RIGHT_BUTTON_X               108
#define UI_BUTTON_HEIGHT                (UI_CHAR_HEIGHT+2)
#define UI_BUTTON_Y                     (UI_SCREEN_HEIGHT - UI_BUTTON_HEIGHT)

#define UI_SELECT_WINDOW_WIDTH          44
#define UI_ITEM_LEFT_X                  ( (UI_SCREEN_WIDTH-(UI_SELECT_WINDOW_WIDTH*3) )/4)
#define UI_ITEM_CENTER_X                ( UI_ITEM_LEFT_X*2 + UI_SELECT_WINDOW_WIDTH )
#define UI_ITEM_RIGHT_X                 ( UI_ITEM_LEFT_X*3 + UI_SELECT_WINDOW_WIDTH*2 )


#define UI_COLOR_BACKGROUND             WHITE
#define UI_COLOR_TITLE_TEXT             BLUE
#define UI_COLOR_BUTTON                 RED
#define UI_COLOR_BUTTON_TEXT            YELLOW
#define UI_COLOR_SELECT_WINDOW          DARK_GREEN
#define UI_COLOR_ICON_TEXT              MAGENTA

#define UI_BUTTON_DEBOUNCE_MS           50

/***************************************
*    Function Prototypes
***************************************/
void InitUserInterface(void);
void UpdateLedState(void);

/***************************************
*   System Power Definitions 
***************************************/
#ifdef CYDEV_VDDD_MV
   #define SYS_VOLTAGE CYDEV_VDDD_MV    /* PSoC6 device system voltage. */
#else
   #define SYS_VOLTAGE CYDEV_VDD_MV     /* PSoC6 Module system voltage */    
#endif

/***************************************
*        Macros
***************************************/
// Set the disconnect LED  
//#define Disconnect_LED_Write(value)                                                    
//        
// Set the advertising LED  
#define Advertising_LED_Write(value)                                                  

#define Advertising_LED_INV() Cy_GPIO_Inv(Advertising_LED_0_PORT, Advertising_LED_0_NUM) 

// Set the alert LED   
//#define Alert_LED_Write(value)                                                      

//#define Alert_LED_INV() Cy_GPIO_Inv(Alert_LED_0_PORT, Alert_LED_0_NUM)        


// Read the SW2 pin 
//#define SW2_Read()  Cy_GPIO_Read(SW2_0_PORT, SW2_0_NUM)

// Set LED5 
//#define LED5_Write(value)                                                               

//#define LED5_INV()  Cy_GPIO_Inv(Disconnect_LED_0_PORT, Disconnect_LED_0_NUM)        

// LEDs operations 
//#define EnableAllLeds()                             

//#define DisableAllLeds()                            


void ui_timer( void );
void ui_task( void );






    
/* [] END OF FILE */
