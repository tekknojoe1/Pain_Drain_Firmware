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

#include "power.h"
#include "my_i2c.h"
#include "debug.h"
#include "tens.h"
#include "temp.h"
#include "vibe.h"
#include "flash_storage.h"



/*Funciton protorype for handling errors*/
void HandleError(void){
    DBG_PRINTF("There was an error\r\n");
}

/*Logical Size of Em_EEPROM*/
#define LOGICAL_EEPROM_SIZE     50u
#define LOGICAL_EEPROM_START    0u

/*location of reset counter in Em_EEPROM*/
#define RESET_COUNT_LOCATION    13u
#define RESET_COUNT_SIZE        2u

/*ASCII "9"*/
#define ASCII_NINE              0x39

/*ASCII "0"*/
#define ASCII_ZERO              0x30

/*ASCII "P"*/
#define ASCII_P                 0x50

/*Return status for EEPROM and UART*/ 
cy_en_em_eeprom_status_t eepromReturnValue;

/* EEPROM storage in work flash, this is defined in Em_EEPROM.c*/
extern const uint8_t Em_EEPROM_em_EepromStorage[];

/*RAM arrays for holding EEPROM read and write data*/
uint8_t eepromArray[50];
uint8_t array[14] = { 0x50, 0x6F, 0x77, 0x65, 0x72, 0x20, 0x43, 0x79, 0x63, 0x6C, 0x65, 0x23, 0x20, 0x30};
                    /* P     o     w     e     r           C     y     c     l     e     #           0*/

uint8_t array1[50] = {
    // Packet 1: TENS
    0x54,                      // 'T'
    0x20,
    0x35, 0x30,                // Amplitude: "50"
    0x20,
    0x31, 0x30,                // Mode: "10"
    0x20,
    0x31,                      // Play/Pause: "1"
    0x20,
    0x32, 0x30,                // Channel: "20"
    0x20,
    0x33, 0x30,                // Phase: "30"
    0x00,                      // Delimiter

    // Packet 2: Vibration
    0x76,                      // 'v'
    0x20,
    0x37, 0x30,                // Amplitude: "70"
    0x20,
    0x36, 0x30,                // Frequency: "60"
    0x20,
    0x38, 0x30,                // Waveform: "80"
    0x00,                      // Delimiter

    // Packet 3: Temperature
    0x74,                      // 't'
    0x20,
    0x34, 0x30,                // Temperature: "40"
    0x00                       // Delimiter
};


void printPreset(const Preset *preset) {
    DBG_PRINTF("Preset Data:\r\n");
    DBG_PRINTF("  Header: %d\r\n", preset->header);
    DBG_PRINTF("  Preset ID: %d\r\n", preset->preset_id);
    
    DBG_PRINTF("TENS Settings:\r\n");
    DBG_PRINTF("  Amplitude: %d\r\n", preset->tens.intensity);
    DBG_PRINTF("  Mode: %d\r\n", preset->tens.mode);
    DBG_PRINTF("  Play: %d\r\n", preset->tens.play);
    DBG_PRINTF("  Channel: %d\r\n", preset->tens.channel);
    DBG_PRINTF("  Phase: %d\r\n", preset->tens.phase);
    
    DBG_PRINTF("Vibration Settings:\r\n");
    DBG_PRINTF("  Intensity: %d\r\n", preset->vibration.frequency);
    
    DBG_PRINTF("Temperature Settings:\r\n");
    DBG_PRINTF("  Temperature: %d\r\n", preset->temperature.temp);
    
    DBG_PRINTF("  Footer: %d\r\n", preset->footer);
}


void applyPreset(const Preset* preset) {
    if (!preset) {
        DBG_PRINTF("Preset pointer is NULL!\r\n");
        return;
    }

    // Example: Update TENS settings
    set_tens_signal(preset->tens.intensity,
                    preset->tens.mode,
                    preset->tens.play,
                    preset->tens.channel,
                    preset->tens.phase);

    // Example: Update Temperature setting
    set_temp(preset->temperature.temp);

    // Example: Update Vibration setting (if applicable)
    set_vibe(preset->vibration.frequency);

    DBG_PRINTF("Preset applied: Header=%d, PresetID=%d, Footer=%d\r\n",
               preset->header, preset->preset_id, preset->footer);
}

// This is used to specifically get the preset values from EEPROM
void readPresetFromEeprom(Preset* preset, int presetNumber) {
    if (presetNumber < 1 || presetNumber > MAX_PRESETS) {
        DBG_PRINTF("Invalid preset number: %d\r\n", presetNumber);
        return;
    }

    // Calculate the EEPROM address for this preset
    uint32_t presetAddress = LOGICAL_EEPROM_START + ((presetNumber - 1) * sizeof(Preset));

    DBG_PRINTF("Reading preset %d from EEPROM at address %d\r\n", presetNumber, presetAddress);
    
    eepromReturnValue = Cy_Em_EEPROM_Read(presetAddress, (uint8_t*)preset, sizeof(Preset), &Em_EEPROM_context);

    if (eepromReturnValue != CY_EM_EEPROM_SUCCESS) {
        HandleError();
        DBG_PRINTF("ERROR: %d\r\n", eepromReturnValue);
    }
}

void loadAndApplyPreset(int presetNumber) {
    Preset preset;
    
    __disable_irq();
    // First, read the preset from EEPROM and store it in our preset pointer
    readPresetFromEeprom(&preset, presetNumber);
    __enable_irq();
    // Then, set the preset values on the device
    applyPreset(&preset);
    
    // Print Preset used to just check if it was set correctly
    printPreset(&preset);
}


// Used to write preset to EEPROM or flash memory on device 
void writeToEeprom(uint8_t* data, size_t size, int preset) {
    
    // Calculate the EEPROM address offset for this preset
    uint32_t presetAddress = LOGICAL_EEPROM_START + ((preset - 1) * sizeof(Preset));
    
    eepromReturnValue = Cy_Em_EEPROM_Write(presetAddress, data, size, &Em_EEPROM_context);
    
    if (eepromReturnValue == CY_EM_EEPROM_SUCCESS) {
        DBG_PRINTF("SUCCESS\r\n", eepromReturnValue);
    } else if(eepromReturnValue == CY_EM_EEPROM_BAD_CHECKSUM){
        DBG_PRINTF("BAD CHECKSUM\r\n", eepromReturnValue);
    } else if(eepromReturnValue == CY_EM_EEPROM_BAD_PARAM){
        DBG_PRINTF("BAD PARAM\r\n", eepromReturnValue);
    } else if(eepromReturnValue == CY_EM_EEPROM_BAD_DATA){
        DBG_PRINTF("BAD DATA\r\n", eepromReturnValue);
    } else if(eepromReturnValue == CY_EM_EEPROM_WRITE_FAIL){
        DBG_PRINTF("WRITE FAIL\r\n", eepromReturnValue);
    } else{
        DBG_PRINTF("OTHER\r\n", eepromReturnValue);
    }
   
}


// This is a general way to read from device flash. Not using it right now but might be nice later for debugging
void readFromEeprom(uint8_t* buffer, size_t size) {
    /* Read data from EEPROM */
    DBG_PRINTF("Reading data from EEPROM\r\n");
    eepromReturnValue = Cy_Em_EEPROM_Read(LOGICAL_EEPROM_START, buffer, size, &Em_EEPROM_context);
    if (eepromReturnValue != CY_EM_EEPROM_SUCCESS) {
        HandleError();
        DBG_PRINTF("ERROR: %d\r\n", eepromReturnValue);
    }
}


void initEeprom(){
     /* Set Start Address to work flash */
    Em_EEPROM_config.userFlashStartAddr = (uint32_t)Em_EEPROM_em_EepromStorage;

    /* Initialize EEPROM, config, and context structures defined in Em_EEPROM.c/.h */
    eepromReturnValue = Cy_Em_EEPROM_Init(&Em_EEPROM_config, &Em_EEPROM_context);
    if (eepromReturnValue != CY_EM_EEPROM_SUCCESS) {
        HandleError();
    } 
}


/* [] END OF FILE */
