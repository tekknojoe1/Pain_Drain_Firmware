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


#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H
#include <project.h>
#include "tens.h"
#include "vibe.h"
#include "temp.h"




/*Logical Size of Em_EEPROM*/
#define LOGICAL_EEPROM_SIZE     50u
#define LOGICAL_EEPROM_START    0u
#define MAX_PRESETS 3

/*location of reset counter in Em_EEPROM*/
#define RESET_COUNT_LOCATION    13u
#define RESET_COUNT_SIZE        2u

/*ASCII "9"*/
#define ASCII_NINE              0x39

/*ASCII "0"*/
#define ASCII_ZERO              0x30

/*ASCII "P"*/
#define ASCII_P                 0x50

void setupEpprom( void );
void initEeprom( void );
void getEepromPreset(int presetNumber);
void writeToEeprom(uint8_t* data, size_t size, int preset);
void readFromEeprom(uint8_t* buffer, size_t size);
void loadAndApplyPreset(int preset);

// Define a struct for a single preset
typedef struct {
    uint8_t header;                  // Header marker
    uint8_t preset_id;               // Preset ID (1, 2, or 3)
    TensSetting tens;                // TENS settings
    VibrationSetting vibration;      // Vibration settings
    TemperatureSetting temperature;  // Temperature settings
    uint8_t footer;                  // Footer marker
} Preset;


extern Preset presets[MAX_PRESETS];

// Define constants for header and footer markers
#define HEADER_MARKER 0xA1
#define FOOTER_MARKER 0xFF


// This is the array that we use to read from our EEPROM flash storage
uint8_t eepromArray[50];
// Use this to store values in and use this array to write to our EEPROM flash storage
uint8_t writeFlashStorage[50];


#endif
/* [] END OF FILE */
