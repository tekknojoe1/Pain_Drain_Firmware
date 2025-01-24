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



#include <project.h>


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

void setupEpprom( void );
void initEeprom( void );
void processEeprom(void);
void getEepromPreset(int presetNumber);
void writeToEeprom(uint8_t* data, size_t size);
void readFromEeprom(uint8_t* buffer, size_t size);


// This is the array that we use to read from our EEPROM flash storage
uint8_t eepromArray[50];
// Use this to store values in and use this array to write to our EEPROM flash storage
uint8_t writeFlashStorage[50];

/* [] END OF FILE */
