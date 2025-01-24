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


void getEepromPreset(int presetNumber){
    switch (presetNumber) {
        case 1:
            
            DBG_PRINTF("PRESET1 \r\n");
            break;
        case 2:
            DBG_PRINTF("PRESET3 \r\n");
            
            break;
        case 3:
            DBG_PRINTF("PRESET3 \r\n");
            break;
    }
}

void writeToEeprom(uint8_t* data, size_t size) {
    /* Write data to EEPROM */
    DBG_PRINTF("Writing data to EEPROM\r\n");
    eepromReturnValue = Cy_Em_EEPROM_Write(LOGICAL_EEPROM_START, data, size, &Em_EEPROM_context);
    
    if (eepromReturnValue != CY_EM_EEPROM_SUCCESS) {
        HandleError();
    }
}

void readFromEeprom(uint8_t* buffer, size_t size) {
    /* Read data from EEPROM */
    DBG_PRINTF("Reading data from EEPROM\r\n");
    eepromReturnValue = Cy_Em_EEPROM_Read(LOGICAL_EEPROM_START, buffer, size, &Em_EEPROM_context);
    if (eepromReturnValue != CY_EM_EEPROM_SUCCESS) {
        HandleError();
    }
}

void processEeprom(void) {
    uint8_t defaultValue = 0x54; // Default value to check

    /* Read EEPROM contents */
    //readFromEeprom(eepromArray, LOGICAL_EEPROM_SIZE);
    //
    ///* Check if the first byte is not the expected value */
    //if (eepromArray[0] != defaultValue) {
    //    DBG_PRINTF("Overwriting array\r\n");
    //    writeToEeprom(array1, LOGICAL_EEPROM_SIZE);
    //} else {
    //    DBG_PRINTF("EEPROM already initialized, not overwriting\r\n");
    //}

    /* Verify EEPROM contents after the operation */
    readFromEeprom(eepromArray, LOGICAL_EEPROM_SIZE);

    /* Print EEPROM contents to console */
    for (uint8_t i = 0; i < LOGICAL_EEPROM_SIZE; i++) {
        uint8_t value = eepromArray[i];
        if (value >= 32 && value <= 126) { // Check for printable ASCII characters
            DBG_PRINTF("letter %d -> '%c'\r\n", value, value);
        } else {
            DBG_PRINTF("letter %d -> Non-printable\r\n", value);
        }
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
    
    //processEeprom();
}


/* [] END OF FILE */
