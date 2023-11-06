/*******************************************************************************
* File Name: user_interface.c
*
* Version: 1.10
*
* Description:
*  This file contains user interface related source.
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
#include "user_interface.h"
#include "st7735.h"
#include "power.h"
#include <stdio.h>
#include "bq24298.h"
#include <stdlib.h>

static int32_t ui_timeout = 0;
#define UI_TIMER_INTERVAL_MS 10

static int32 tens_timeout = -1;
static int32 tens_interval_ms = 0;
static int32 tens_dur_us = 0;

#define UI_STATE_INIT 0
#define UI_STATE_DRAW_SPLASH_SCREEN 1
#define UI_STATE_SPLASH_SCREEN_DELAY 2
#define UI_STATE_IDLE 10
static int ui_state = 0;

#define UI_MENU_MAX 10
#define UI_MENU_MAIN 0
#define UI_MENU_PRESET 1
#define UI_MENU_TEMP 2
#define UI_MENU_VIBE_AMP 3
#define UI_MENU_VIBE_FREQ 4
#define UI_MENU_TENS_FREQ 5
#define UI_MENU_TENS_AMP 6
#define UI_MENU_TENS_DUR 7
#define UI_MENU_BLUETOOTH 8
#define UI_MENU_DIAG 9

static int ui_menu = 0;
static int ui_menu_state = 0;
static int ui_menu_item[UI_MENU_MAX] = {0};

const char* const UI_TITLES[] = {"Main Menu", "Presets Menu", "Temperature Menu", "Vibration Amplitude", "Vibration Frequency", "TENS Frequency", "TENS Amplitude", "TENS Duration", "Bluetooth Settings", "Diagnostics"}; 

const char* const UI_MAIN_MENU_ITEMS[] = {"Pre-sets", "Temper-ature", "Vibe-amp", "Vibe-freq", "TENS-freq", "TENS-Amp", "TENS-Dur", "Blue-tooth", "Diagno-stics" };
const char* const UI_PRESETS_MENU_ITEMS[] = {"Preset-1", "Preset-2", "Preset-3", "Preset-4", "Preset-5", "Preset-6", "Preset-7", "Preset-8", "Preset-9", "Preset-10"};
const char* const UI_TEMP_MENU_ITEMS[] = {"Cold-7", "Cold-6", "Cold-5", "Cold-4", "Cold-3", "Cold-2", "Cold-1", "Off", "Hot-1", "Hot-2", "Hot-3", "Hot-4", "Hot-5", "Hot-6", "Hot-7"};
const char* const UI_VIBE_AMP_MENU_ITEMS[] = {"Off", "Amp-1", "Amp-2", "Amp-3", "Amp-4", "Amp-5", "Amp-6", "Amp-7", "Amp-8", "Amp-9", "Amp-10"};
const char* const UI_VIBE_FREQ_MENU_ITEMS[] = {"Freq-1", "Freq-2", "Freq-3", "Freq-4", "Freq-5", "Freq-6", "Freq-7", "Freq-8", "Freq-9", "Freq-10"};
const char* const UI_TENS_FREQ_MENU_ITEMS[] = {"1 Hz", "2 Hz", "3 Hz", "4 Hz", "5 Hz", "6 Hz", "7 Hz", "8 Hz", "9 Hz", "10 Hz"};
const char* const UI_TENS_AMP_MENU_ITEMS[] = {"Off", "Amp-1", "Amp-2", "Amp-3", "Amp-4", "Amp-5", "Amp-6", "Amp-7", "Amp-8", "Amp-9", "Amp-10"};
const char* const UI_TENS_DUR_MENU_ITEMS[] = {"300", "400", "500", "600", "700", "800", "900", "1000", "1100", "1200"};
const char* const UI_BLUETOOTH_MENU_ITEMS[] = {"Pair"};
const char* const UI_DIAG_MENU_ITEMS[] = {""};



const int UI_MENU_ITEMS_INIT[UI_MENU_MAX] = {1,             1,  0,  0,  1,  1,  0,  1,  0, 0};
const int UI_MENU_ITEMS_MAX[UI_MENU_MAX] =  {UI_MENU_MAX-1, 10, 7,  10, 10, 10, 10, 10, 0, 0}; 
const int UI_MENU_ITEMS_MIN[UI_MENU_MAX] =  {1,             1,  -7, 0,  1,  1,  0,  1,  0, 0};

static int btn_state[UI_NUM_BUTTONS] = {0};
static int btn_events[UI_NUM_BUTTONS] = {0};
static int btn_timeout[UI_NUM_BUTTONS] = {0};

static int vibe_freq = 0;
static int vibe_amp = 0;
static int vibe_timeout = 0;
//static int vibe_on_time = 0;
static int vibe_state = 0;
#define VIBE_PERIOD_MS 100 




/*******************************************************************************
* Function Name: void ui_timer()
********************************************************************************
*
* Summary:
*   user interface timer called every 100ms right now
*
*******************************************************************************/
void ui_timer( void ) {
    int i;
    
    if (tens_timeout > 0) {
        tens_timeout -= UI_TIMER_INTERVAL_MS;
        
        if (tens_timeout < 0) {
            tens_timeout = 0;
        }
    }
    
    if (ui_timeout > 0) {
        ui_timeout -= UI_TIMER_INTERVAL_MS;
        
        if (ui_timeout < 0) {
            ui_timeout = 0;
        }
    }
    
    for (i=0;i<UI_NUM_BUTTONS;i++) {
        if (btn_timeout[i] > 0) {
            btn_timeout[i] -= UI_TIMER_INTERVAL_MS;
            
            if (btn_timeout[i] < 0) {
                btn_timeout[i] = 0;
            }
        }
    }
    
    
    if (vibe_timeout > 0) {
        vibe_timeout -= UI_TIMER_INTERVAL_MS;
        
        if (vibe_timeout < 0) {
            vibe_timeout = 0;
        }
    }
    
}


/*******************************************************************************
* Function Name: void ui_draw_splash_screen()
********************************************************************************
*
* Summary:
*   Draws splash screen
*
*******************************************************************************/
void ui_draw_splash_screen( void ) {
    //ST7735_SetPosition (53, 23);  
    //ST7735_DrawString ("PAIN DRAIN", BLUE, X2);
    //ST7735_SetPosition (47, 41);  
    //ST7735_DrawString ("SPLASH SCREEN", BLUE, X2);
    //ST7735_RAM_Content_Show();
}

/*******************************************************************************
* Function Name: void ui_draw_title()
********************************************************************************
*
* Summary:
*   Draws title
*
*******************************************************************************/
void ui_draw_title(char text[]) {
    //ST7735_DrawRectangle (1, 1+UI_TITLE_WIDTH, 1, 1+UI_TITLE_HEIGHT, UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (1, 1);  
    //ST7735_DrawString (text, UI_COLOR_TITLE_TEXT, X2);
}

/*******************************************************************************
* Function Name: void ui_draw_buttons()
********************************************************************************
*
* Summary:
*   Draws buttons
*
*******************************************************************************/
void ui_draw_buttons(char left[], char center[], char right[] ) {
    
    //Draw Left Button
    //ST7735_DrawRectangle (UI_LEFT_BUTTON_X, UI_LEFT_BUTTON_X+UI_BUTTON_WIDTH, UI_BUTTON_Y, UI_BUTTON_Y+UI_BUTTON_HEIGHT, UI_COLOR_BUTTON);
    //ST7735_SetPosition (UI_LEFT_BUTTON_X+(UI_BUTTON_WIDTH-(CHARS_COLS_LEN+1)*strlen(left))/2, UI_BUTTON_Y+1);  
    //ST7735_DrawString (left, UI_COLOR_BUTTON_TEXT, X2);
    
    //Draw Center Button
    //ST7735_DrawRectangle (UI_CENTER_BUTTON_X, UI_CENTER_BUTTON_X+UI_BUTTON_WIDTH, UI_BUTTON_Y, UI_BUTTON_Y+UI_BUTTON_HEIGHT, UI_COLOR_BUTTON);
    //ST7735_SetPosition (UI_CENTER_BUTTON_X+(UI_BUTTON_WIDTH-(CHARS_COLS_LEN+1)*strlen(center))/2, UI_BUTTON_Y+1);   
    //ST7735_DrawString (center, UI_COLOR_BUTTON_TEXT, X2);
    
    //Draw Right Button
    //ST7735_DrawRectangle (UI_RIGHT_BUTTON_X, UI_RIGHT_BUTTON_X+UI_BUTTON_WIDTH, UI_BUTTON_Y, UI_BUTTON_Y+UI_BUTTON_HEIGHT, UI_COLOR_BUTTON);
    //ST7735_SetPosition (UI_RIGHT_BUTTON_X+(UI_BUTTON_WIDTH-(CHARS_COLS_LEN+1)*strlen(right))/2, UI_BUTTON_Y+1);  
    //ST7735_DrawString (right, UI_COLOR_BUTTON_TEXT, X2);
}

/*******************************************************************************
* Function Name: void ui_draw_select_window()
********************************************************************************
*
* Summary:
*   Select window
*
*******************************************************************************/
void ui_draw_select_window( void ) {
    
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2,     (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,    (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2,                          (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2,                            UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+1,   (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,  (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1,                        (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1,                          UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2,     (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,    (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,   (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,     UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+1,   (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,  (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2-1+UI_SELECT_WINDOW_WIDTH, (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2-1+UI_SELECT_WINDOW_WIDTH,   UI_COLOR_SELECT_WINDOW);

    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2,     (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2,                           (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1,                        (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,   UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+1,   (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+1,                         (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1+1,                      (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-2,   UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,     (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH,                           (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1,                        (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,   UI_COLOR_SELECT_WINDOW);
    //ST7735_DrawLine((UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,   (UI_SCREEN_WIDTH-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-1,                         (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+1+1,                      (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-2,   UI_COLOR_SELECT_WINDOW);

 
}

/*******************************************************************************
* Function Name: void ui_clear_icon()
********************************************************************************
*
* Summary:
*   Clears icon area
*
*******************************************************************************/
void ui_clear_icon(int loc) {
   //ST7735_DrawRectangle (loc+2, loc+UI_SELECT_WINDOW_WIDTH-4, (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+2, (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-3, UI_COLOR_BACKGROUND);   
}

/*******************************************************************************
* Function Name: void ui_draw_icon_text()
********************************************************************************
*
* Summary:
*   Draws icon text
*
*******************************************************************************/
void ui_draw_icon_text(int loc, char text[]) {
    char s[10];
    strcpy(s, text);
    char* top = strtok(s, "-");
    char* bot = strtok(NULL, "-");
    
    //ST7735_SetPosition (loc+(UI_SELECT_WINDOW_WIDTH-(CHARS_COLS_LEN+1)*strlen(top) )/2, (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-4-UI_CHAR_HEIGHT*2-10 ); 
    //ST7735_DrawString (top, UI_COLOR_ICON_TEXT, X2);
    //ST7735_SetPosition (loc+(UI_SELECT_WINDOW_WIDTH-(CHARS_COLS_LEN+1)*strlen(bot) )/2, (UI_SCREEN_HEIGHT-UI_SELECT_WINDOW_WIDTH)/2+UI_SELECT_WINDOW_WIDTH-4-UI_CHAR_HEIGHT );  
    //ST7735_DrawString (bot, UI_COLOR_ICON_TEXT, X2); 
}



/*******************************************************************************
* Function Name: void ui_draw_icons()
********************************************************************************
*
* Summary:
*   Draws an icon
*
*******************************************************************************/
void ui_draw_icons (int menu, int loc, int item) {
    ui_clear_icon(loc);  
    if (item < UI_MENU_ITEMS_MIN[menu]) return;
    if (item > UI_MENU_ITEMS_MAX[menu]) return;
    
    switch (menu) {
        default: break;
        case UI_MENU_MAIN:          ui_draw_icon_text(loc, (char*) UI_MAIN_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_PRESET:        ui_draw_icon_text(loc, (char*) UI_PRESETS_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_TEMP:          ui_draw_icon_text(loc, (char*) UI_TEMP_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_VIBE_AMP:      ui_draw_icon_text(loc, (char*) UI_VIBE_AMP_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_VIBE_FREQ:     ui_draw_icon_text(loc, (char*) UI_VIBE_FREQ_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_TENS_FREQ:     ui_draw_icon_text(loc, (char*) UI_TENS_FREQ_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_TENS_AMP:      ui_draw_icon_text(loc, (char*) UI_TENS_AMP_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_TENS_DUR:      ui_draw_icon_text(loc, (char*) UI_TENS_DUR_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_BLUETOOTH:     ui_draw_icon_text(loc, (char*) UI_BLUETOOTH_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
        case UI_MENU_DIAG:          ui_draw_icon_text(loc, (char*) UI_DIAG_MENU_ITEMS[item-UI_MENU_ITEMS_MIN[menu]] );      break;
    }
}

/*******************************************************************************
* Function Name: void ui_draw_diag()
********************************************************************************
*
* Summary:
*   Draws diagnostics
*
*******************************************************************************/
void ui_draw_diag( void ) {
    uint8 d[POWER_DIAG_DATA_SIZE];
    char s0[16];
    //char s1[16];
    
    int vbus_stat;
    int chrg_stat;
    int dpm_stat;
    int pg_stat;
    int therm_stat;
    int vsys_stat;
    
    int wtd_fault;
    int otg_fault;
    int chrg_fault;
    int bat_fault;
    int ntc_fault;
    
    power_get_diag_data(d);

//#define CHG_REG_DUMP
#ifdef CHG_REG_DUMP
    
    sprintf(s0, "%02X%02X%02X%02X%02X%02X%02X%02X", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
    sprintf(s1, "%02X%02X%02X", d[8], d[9], d[10]);
    
    
    ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 1+UI_TITLE_HEIGHT+1, 2*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    ST7735_SetPosition (2, 1+UI_TITLE_HEIGHT+1);  
    ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    
    ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 2*(1+UI_TITLE_HEIGHT)+1, 3*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    ST7735_SetPosition (2, 2*(1+UI_TITLE_HEIGHT)+1);  
    ST7735_DrawString (s1, UI_COLOR_ICON_TEXT, X2);
    
#else
  
    vbus_stat = (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_VBUS_MASK)>>CHG_SYS_STAT_VBUS_START;
    chrg_stat = (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_CHG_MASK)>>CHG_SYS_STAT_CHG_START;
    dpm_stat =  (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_DPM_MASK)>>CHG_SYS_STAT_DPM_START;
    pg_stat =   (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_PGM_MASK)>>CHG_SYS_STAT_PGM_START;
    therm_stat = (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_THERM_MASK)>>CHG_SYS_STAT_THERM_START;
    vsys_stat = (d[CHG_SYS_STAT_REG] & CHG_SYS_STAT_VSYS_MASK)>>CHG_SYS_STAT_VSYS_START;
    
    sprintf(s0, "VBUS:%d", vbus_stat);
    //ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 1+UI_TITLE_HEIGHT+1, 2*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2, 1+UI_TITLE_HEIGHT+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "CHRG:%d", chrg_stat);
    //ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 2*(1+UI_TITLE_HEIGHT)+1, 3*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2, 2*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "DPM:%d", dpm_stat);
    //ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 3*(1+UI_TITLE_HEIGHT)+1, 4*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2, 3*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "PG:%d", pg_stat);
    //ST7735_DrawRectangle (2, 1+UI_TITLE_WIDTH, 4*(1+UI_TITLE_HEIGHT)+1, 5*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2, 4*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    
    sprintf(s0, "THERM:%d", therm_stat);
    //ST7735_DrawRectangle (2+UI_TITLE_WIDTH/2, 1+UI_TITLE_WIDTH/2, 1+UI_TITLE_HEIGHT+1, 2*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+UI_TITLE_WIDTH/2, 1+UI_TITLE_HEIGHT+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "VSYS:%d", vsys_stat);
    //ST7735_DrawRectangle (2+UI_TITLE_WIDTH/2, 1+UI_TITLE_WIDTH/2, 2*(1+UI_TITLE_HEIGHT)+1, 3*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+UI_TITLE_WIDTH/2, 2*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    wtd_fault = (d[CHG_NEW_FAULT_REG] & CHG_NEW_FAULT_WDT_MASK)>>CHG_NEW_FAULT_WDT_START;
    otg_fault = (d[CHG_NEW_FAULT_REG] & CHG_NEW_FAULT_OTG_MASK)>>CHG_NEW_FAULT_OTG_START;
    chrg_fault = (d[CHG_NEW_FAULT_REG] & CHG_NEW_FAULT_CHG_MASK)>>CHG_NEW_FAULT_CHG_START;
    bat_fault = (d[CHG_NEW_FAULT_REG] & CHG_NEW_FAULT_BATT_MASK)>>CHG_NEW_FAULT_BATT_START;
    ntc_fault = (d[CHG_NEW_FAULT_REG] & CHG_NEW_FAULT_NTC_MASK)>>CHG_NEW_FAULT_NTC_START;
    
    sprintf(s0, "WTDF:%d", wtd_fault);
    //ST7735_DrawRectangle (2+UI_TITLE_WIDTH/2, 1+UI_TITLE_WIDTH, 3*(1+UI_TITLE_HEIGHT)+1, 4*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+UI_TITLE_WIDTH/2, 3*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "OTGF:%d", otg_fault);
    //ST7735_DrawRectangle (2+UI_TITLE_WIDTH/2, 1+UI_TITLE_WIDTH, 4*(1+UI_TITLE_HEIGHT)+1, 5*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+UI_TITLE_WIDTH/2, 4*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    
    
    sprintf(s0, "CHGF:%d", chrg_fault);
    //ST7735_DrawRectangle (2+2*(UI_TITLE_WIDTH/2), 1+UI_TITLE_WIDTH/2, 1+UI_TITLE_HEIGHT+1, 2*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+2*(UI_TITLE_WIDTH/2), 1+UI_TITLE_HEIGHT+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "BATF:%d", bat_fault);
    //ST7735_DrawRectangle (2+2*(UI_TITLE_WIDTH/2), 1+UI_TITLE_WIDTH/2, 2*(1+UI_TITLE_HEIGHT)+1, 3*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+2*(UI_TITLE_WIDTH/2), 2*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);
    
    sprintf(s0, "NTCF:%d", ntc_fault);
    //ST7735_DrawRectangle (2+2*(UI_TITLE_WIDTH/2), 1+UI_TITLE_WIDTH, 3*(1+UI_TITLE_HEIGHT)+1, 4*(1+UI_TITLE_HEIGHT), UI_COLOR_BACKGROUND);
    //ST7735_SetPosition (2+2*(UI_TITLE_WIDTH/2), 3*(1+UI_TITLE_HEIGHT)+1);  
    //ST7735_DrawString (s0, UI_COLOR_ICON_TEXT, X2);

#endif
    
}


/*******************************************************************************
* Function Name: void tens_task
********************************************************************************
*
* Parameter:    None
*
* Summary:
*   TENS task
*
*******************************************************************************/
void ui_tens_task( void ) {
    
    if (tens_timeout == 0) {
        
        //On positive direction
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 1);
        
        CyDelayUs(tens_dur_us);
        
        //Off state in between
        Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
        Cy_GPIO_Write(TENS_POS2_PORT, TENS_POS2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        CyDelayUs(tens_dur_us);
        
        //On negative direction
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 1);
        Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 1);
        
        CyDelayUs(tens_dur_us);
        
        //Off state in between
        Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);
        Cy_GPIO_Write(TENS_NEG2_PORT, TENS_NEG2_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
        Cy_GPIO_Write(TENS_USER_EN2_PORT, TENS_USER_EN2_NUM, 0);
        
        if (tens_interval_ms > 0) 
            tens_timeout = tens_interval_ms;
    }
    
}

/*******************************************************************************
* Function Name: void ui_settings_tens_freq(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   TENS freq
*
*******************************************************************************/
void ui_settings_tens_freq (int item) {
    
    switch (item) {
        default: tens_interval_ms = -1; break;   
        case 1: tens_interval_ms = 1000; break;
        case 2: tens_interval_ms = 500; break;
        case 3: tens_interval_ms = 333; break;
        case 4: tens_interval_ms = 250; break;
        case 5: tens_interval_ms = 200; break;
        case 6: tens_interval_ms = 167; break;
        case 7: tens_interval_ms = 143; break;
        case 8: tens_interval_ms = 125; break;
        case 9: tens_interval_ms = 111; break;
        case 10: tens_interval_ms = 100; break;
    }
    
    if (tens_interval_ms > 0) 
        tens_timeout = tens_interval_ms;
}

/*******************************************************************************
* Function Name: void ui_settings_tens_dur(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   TENS duration
*
*******************************************************************************/
void ui_settings_tens_dur (int item) {
    
    switch (item) {
        default: tens_dur_us = 0; break;   
        case 1: tens_dur_us = 300; break;
        case 2: tens_dur_us = 400; break;
        case 3: tens_dur_us = 500; break;
        case 4: tens_dur_us = 600; break;
        case 5: tens_dur_us = 700; break;
        case 6: tens_dur_us = 800; break;
        case 7: tens_dur_us = 900; break;
        case 8: tens_dur_us = 1000; break;
        case 9: tens_dur_us = 1100; break;
        case 10: tens_dur_us = 1200; break;
    }
}

/*******************************************************************************
* Function Name: void ui_settings_tens_amp(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   TENS amp
*
*******************************************************************************/
void ui_settings_tens_amp(int item) {

    //300 us on, 300 us off and 300 us in reverse direction
    
    
    if (item == 0) {
        power_5v_off(); // Not need for new version
        PWM_TENS_SetCompare0(0);
        power_flags_update(UI_MENU_TENS_AMP, 0);
        
        tens_interval_ms = -1; //Disable timer
        
    } else {
        power_5v_on(); // Not need for new version
        //PWM_TENS_SetCompare0(257 * item/ 10);
        //PWM_TENS_SetCompare0(624 * item / 10);
        power_flags_update(UI_MENU_TENS_AMP, 1);
        
        ui_settings_tens_dur(ui_menu_item[UI_MENU_TENS_DUR]); 
        
        ui_settings_tens_freq(ui_menu_item[UI_MENU_TENS_FREQ]); //Enable tens timer
        
        switch (item) {
            case 1: PWM_TENS_SetCompare0(1); break;             
            case 2: PWM_TENS_SetCompare0(8); break; 
            case 3: PWM_TENS_SetCompare0(21); break;             
            case 4: PWM_TENS_SetCompare0(33); break;
            case 5: PWM_TENS_SetCompare0(45); break;             
            case 6: PWM_TENS_SetCompare0(58); break; 
            case 7: PWM_TENS_SetCompare0(71); break;             
            case 8: PWM_TENS_SetCompare0(83); break;
            case 9: PWM_TENS_SetCompare0(124); break;             
            case 10: PWM_TENS_SetCompare0(558); break;
            default: PWM_TENS_SetCompare0(0); break;
        }
    }
    
}



/*******************************************************************************
* Function Name: void ui_vibe_task( void )
********************************************************************************
*
* Parameter:    None
*
* Summary:
*   vibe task
*
*******************************************************************************/
void ui_vibe_task( void ) {
    
    switch (vibe_state) {
        
        default:
            if (vibe_amp > 0) {
                vibe_state = 1;
                vibe_timeout = vibe_amp;
                //PWM_VIBE_SetCompare0(vibe_freq);
            }
        break;
            
        case 1: //on time
            if (vibe_timeout == 0) {
                vibe_state = 2;
                vibe_timeout = VIBE_PERIOD_MS - vibe_amp;
                if (vibe_timeout < 0) vibe_timeout = 0;
                //PWM_VIBE_SetCompare0(0);
            }

        case 2: //off time
            if (vibe_amp == 0) {
                vibe_state = 0;
            } else if (vibe_timeout == 0) {
                vibe_state = 1;
                vibe_timeout = vibe_amp;
                //PWM_VIBE_SetCompare0(vibe_freq);
            }
        break;
    }
}    

/*******************************************************************************
* Function Name: void ui_settings_vibe_freq(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   Update vibe settings
*
*******************************************************************************/
void ui_settings_vibe_freq(int item) {
    
    //min freq was 4 which is 102
    if (item > 0) {
        vibe_freq = (257 - 102) * item / 10 + 102;
    } else {
        vibe_freq = 0;
    }
}

/*******************************************************************************
* Function Name: void ui_settings_vibe_amp(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   Update vibe settings
*
*******************************************************************************/
void ui_settings_vibe_amp(int item) {
    
    //Min speed was 5 which is 50%
    
    if (item > 0) {
        vibe_amp = (VIBE_PERIOD_MS/2) * item / 10 + VIBE_PERIOD_MS/2;
        power_flags_update(UI_MENU_VIBE_AMP, 1);
    } else {
        vibe_amp = 0;
        power_flags_update(UI_MENU_VIBE_AMP, 0);
    }
  
}

/*******************************************************************************
* Function Name: void ui_settings_temp(int item)
********************************************************************************
*
* Parameter:    item
*
* Summary:
*   Update temp settings
*
*******************************************************************************/
    void ui_settings_temp(int item) {
        
  //void ui_settings_temp(int item, int direction, int dutyCyle) {
 
    //Update Fan
    if (item == 0) {
        //Turn fan off
        PWM_FAN_SetCompare0(0);
    } else {
        //Set fan on max   
        PWM_FAN_SetCompare0(257);
    }
    
    
    /* NEED TO WORK ON THIS
    //Update Peltier
    if(direction == 0){
        Cy_GPIO_Write(PEL_1_0_PORT, PEL_1_0_NUM, dutyCyle);
        Cy_GPIO_Write(PEL_2_0_PORT, PEL_2_0_NUM, 0);
    } else {
        Cy_GPIO_Write(PEL_1_0_PORT, PEL_1_0_NUM, 0);
        Cy_GPIO_Write(PEL_2_0_PORT, PEL_2_0_NUM, dutyCyle);
    }
    */
            
    if (item == 0) {
        power_flags_update(UI_MENU_TEMP, 0);
    } else {
        power_flags_update(UI_MENU_TEMP, 1);
    }    
}


/*******************************************************************************
* Function Name: void ui_setting(int menu, item)
********************************************************************************
*
* Parameter:    menu 
*               item
*
* Summary:
*   Update menu settings
*
*******************************************************************************/
void ui_settings(int menu, int item) {
 
    switch (menu) {
        default: break;
        case UI_MENU_MAIN:             break;
        case UI_MENU_PRESET:              break;
        case UI_MENU_TEMP:          ui_settings_temp(item);   break;
        case UI_MENU_VIBE_AMP:      ui_settings_vibe_amp(item);     break;
        case UI_MENU_VIBE_FREQ:     ui_settings_vibe_freq(item);      break;
        case UI_MENU_TENS_FREQ:     ui_settings_tens_freq(item);      break;
        case UI_MENU_TENS_AMP:      ui_settings_tens_amp(item);     break;
        case UI_MENU_TENS_DUR:      ui_settings_tens_dur(item);        break;
        case UI_MENU_BLUETOOTH:           break;
        case UI_MENU_DIAG:           break;
    }
    
}


/*******************************************************************************
* Function Name: void ui_menu()
********************************************************************************
*
* Summary:
*   Menu task
*
*******************************************************************************/
void ui_menu_task(int btn_events[]) {
    
    ui_vibe_task();
    ui_tens_task();
            
    switch (ui_menu_state) {
        default: 
            //ST7735_ClearScreen (WHITE);
            //Render main menu screen    
            ui_draw_title( (char*) UI_TITLES[ui_menu]);
            if (ui_menu == UI_MENU_MAIN) {
                ui_draw_buttons("<", "SELECT", ">");
            } else if (ui_menu == UI_MENU_BLUETOOTH) {
                ui_draw_buttons("BACK", "PAIR", "RESET");
            } else if (ui_menu == UI_MENU_DIAG) {
                ui_draw_buttons(" ", " ", " ");
            } else {
                ui_draw_buttons("<", "BACK", ">");
            }
            
            ui_draw_select_window();
                                        
            ui_menu_state = 1;
        break;
    
        case 1:  
            
            //Draw menu items left
            ui_draw_icons(ui_menu, UI_ITEM_LEFT_X, ui_menu_item[ui_menu]-1);
            
            //Draw menu items center
            ui_draw_icons(ui_menu, UI_ITEM_CENTER_X, ui_menu_item[ui_menu]);
            
            //Draw menu items right
            ui_draw_icons(ui_menu, UI_ITEM_RIGHT_X, ui_menu_item[ui_menu]+1); 
            
            if (ui_menu == UI_MENU_DIAG) {
                ui_draw_diag();
            } else {
                ui_settings(ui_menu, ui_menu_item[ui_menu]);
            }
            
            ui_menu_state = 2;
        break;
        
        case 2:
            //Wait for button input
            
            //Left button
            if (btn_events[0]) {
                btn_events[0] = 0;
                
                //Exception for bluetooth and diagnostics
                if (ui_menu == UI_MENU_BLUETOOTH || ui_menu == UI_MENU_DIAG) {
                    ui_menu = 0;
                    ui_menu_state = 0;
                    break;
                }
                
                if (ui_menu_item[ui_menu] > UI_MENU_ITEMS_MIN[ui_menu]) {
                    ui_menu_item[ui_menu]--;
                    ui_menu_state = 1;
                    break;
                }
            }
            
            //Center button
            if (btn_events[1]) {
               btn_events[1] = 0;
                if (ui_menu > 0) {
                    ui_menu = 0;
                } else {
                    ui_menu = ui_menu_item[ui_menu];
                }
                ui_menu_state = 0;
                break;
            }
            
            //Right button
            if (btn_events[2]) {
                btn_events[2] = 0;
                
                //Exception for bluetooth
                if (ui_menu == UI_MENU_BLUETOOTH || ui_menu == UI_MENU_DIAG) {
                    ui_menu = 0;
                    ui_menu_state = 0;
                    break;
                }
                
                if (ui_menu_item[ui_menu] < UI_MENU_ITEMS_MAX[ui_menu]) {
                    ui_menu_item[ui_menu]++;
                    ui_menu_state = 1;
                    break;
                }
            }
            
            
        break;
    
    }

}



/*******************************************************************************
* Function Name: void ui_task()
********************************************************************************
*
* Summary:
*   user interface task
*
*******************************************************************************/
void ui_task(void) {
    int i;
    int btn[UI_NUM_BUTTONS];
    
    //btn[0] = Cy_GPIO_Read(BTN0_PORT, BTN0_NUM);
    //btn[1] = Cy_GPIO_Read(BTN1_PORT, BTN1_NUM);
    //btn[2] = Cy_GPIO_Read(BTN2_PORT, BTN2_NUM);
    
    //Button events
    for (i=0;i<UI_NUM_BUTTONS;i++) {
        switch (btn_state[i]) {
            default: //0
                if (btn[i] == 0) {
                    //Button down
                    btn_state[i] = 1;
                    btn_events[i] = 1;
                    btn_timeout[i] = UI_BUTTON_DEBOUNCE_MS;
                    
                    
                    power_wakeup();
                }
            break;
                
            case 1:
                if (btn_timeout[i] == 0) {
                    btn_state[i] = 2;
                }
            break;
                
            case 2:
                if (btn[i] == 1) {
                    //Button up
                    btn_state[i] = 3;
                    btn_timeout[i] = UI_BUTTON_DEBOUNCE_MS;
                 }
            break;
                
            case 3:
                if (btn_timeout[i] == 0) {
                    btn_state[i] = 0;
                }
            break;
        }
        
    
    }
    
    

    switch (ui_state) {

        default: //UI_STATE_IDLE
        
            //ui_bluetooth_icon();
            //ui_battery_icon();
            
            ui_menu_task(btn_events);
        
        break;
        
        case UI_STATE_INIT:
        
            //ST7735_Init();
            //ST7735_ClearScreen (WHITE);
            
            for (i=0;i<UI_MENU_MAX;i++) {
                ui_menu_item[i] = UI_MENU_ITEMS_INIT[i];
            }
                 
            ui_state = UI_STATE_DRAW_SPLASH_SCREEN;
        break;
        
        case UI_STATE_DRAW_SPLASH_SCREEN:
            ui_draw_splash_screen();           
            ui_timeout = 3000; 
            ui_state = UI_STATE_SPLASH_SCREEN_DELAY;
        break;
            
        case UI_STATE_SPLASH_SCREEN_DELAY:
            if (ui_timeout == 0) {
                ui_state = UI_STATE_IDLE;
            }
        break;
        
        

    }
}







/*******************************************************************************
* Function Name: void InitUserInterface(void)
********************************************************************************
*
* Summary:
*   Initialization the user interface: LEDs, SW2, etc. 
*
*******************************************************************************/
void InitUserInterface(void)
{
    /* Initialize wakeup pin for Hibernate */
    Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW);
    
    /* Initialize LEDs */
    //DisableAllLeds();
}


/*******************************************************************************
* Function Name: UpdateLedState
********************************************************************************
*
* Summary:
*  This function updates LED status based on current BLE state.
*
*******************************************************************************/
void UpdateLedState(void)
{
#if(SYS_VOLTAGE >= RGB_LED_MIN_VOLTAGE_MV) 
    if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
    {
        /* In advertising state, turn off disconnect indication LED */
        //Disconnect_LED_Write(LED_OFF);

        /* Blink advertising indication LED */
        //Advertising_LED_INV();
        
        /* Turn off Alert LED */
        //Alert_LED_Write(LED_OFF);
    }
    else if(Cy_BLE_GetNumOfActiveConn() == 0u)
    {
        /* If in disconnected state, turn on disconnect indication LED and turn
        * off Advertising LED.
        */
        //Disconnect_LED_Write(LED_ON);
        //Advertising_LED_Write(LED_OFF);
        
        /* Turn off Alert LED */
        //Alert_LED_Write(LED_OFF);
    }
    else 
    {
        /* In connected state, turn off disconnect indication and advertising 
        * indication LEDs. 
        */
        //Disconnect_LED_Write(LED_OFF);
        Advertising_LED_Write(LED_OFF);
    }
#else
    /* 
     *   If VDDD < 2.7 volts (DWR->System), only the red LED will be used:
     *     BLINK - Alert Level: Mild Alert
     *     OFF   - Alert Level: No Alert
     *     ON    - Alert Level: High Alert    
     *
     *     Updating Alert_LED executed in the Main polling loop
     */        
#endif /* #if(CYDEV_VDDD_MV >= RGB_LED_MIN_VOLTAGE_MV) */  
}
/* [] END OF FILE */
