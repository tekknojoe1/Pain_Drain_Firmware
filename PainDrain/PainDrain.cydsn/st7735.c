/** 
 * --------------------------------------------------------------------------------------------+ 
 * @name        ST773 1.8" LCD Driver
 * --------------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       13.10.2020
 * @update      01.07.2021
 * @file        st7735.c
 * @version     2.0
 * @tested      AVR Atmega328
 *
 * @depend      font.h
 * --------------------------------------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> applicable for more than 1 display
 * --------------------------------------------------------------------------------------------+
 * @inspir      http://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
 *              https://github.com/adafruit/Adafruit-ST7735-Library
 *              http://w8bh.net/avr/AvrTFT.pdf
 */

//#include <avr/io.h>
//#include <avr/pgmspace.h>
//#include <util/delay.h>
#include <project.h>
#include "font.h"
#include "st7735.h"

#define SWAP_X
#define SWAP_Y

void _delay_ms(int ms) {
    CyDelay(ms);
}

uint8_t pgm_read_byte(const uint8_t * i) {
    return *i;
}



/** @array Init command */
const uint8_t INIT_ST7735B[] = {
  // number of initializers
  7,
  // ---------------------------------------
  // Software reset - no arguments,  delay
  0, 150, SWRESET,
  // Out of sleep mode, no arguments, delay
  0, 200, SLPOUT,  
  // Set color mode, 1 argument delay
  1,  10, COLMOD, 0x05,
  // D7  D6  D5  D4  D3  D2  D1  D0
  // MY  MX  MV  ML RGB  MH   -   -
  // ------------------------------
  // ------------------------------
  // MV  MX  MY -> {MV (row / column exchange) MX (column address order), MY (row address order)}
  // ------------------------------
  //  0   0   0 -> begin left-up corner, end right-down corner 
  //               left-right (normal view) 
  //  0   0   1 -> begin left-down corner, end right-up corner 
  //               left-right (Y-mirror)
  //  0   1   0 -> begin right-up corner, end left-down corner 
  //               right-left (X-mirror)
  //  0   1   1 -> begin right-down corner, end left-up corner
  //               right-left (X-mirror, Y-mirror)
  //  1   0   0 -> begin left-up corner, end right-down corner
  //               up-down (X-Y exchange)  
  //  1   0   1 -> begin left-down corner, end right-up corner
  //               down-up (X-Y exchange, Y-mirror)
  //  1   1   0 -> begin right-up corner, end left-down corner 
  //               up-down (X-Y exchange, X-mirror)  
  //  1   1   1 -> begin right-down corner, end left-up corner
  //               down-up (X-Y exchange, X-mirror, Y-mirror)
  // ------------------------------
  //  ML: vertical refresh order 
  //      0 -> refresh top to bottom 
  //      1 -> refresh bottom to top
  // ------------------------------
  // RGB: filter panel
  //      0 -> RGB 
  //      1 -> BGR        
  // ------------------------------ 
  //  MH: horizontal refresh order 
  //      0 -> refresh left to right 
  //      1 -> refresh right to left
  // 0xA0 = 1010 0000
  1,   0, MADCTL, 0xA8, 
  0,   0, PTLON,
  4,   0, PTLAR, 0, 0x0, 0, 0x9F,
  2, 0, VSCSAD, 0, 0,
  // Main screen turn on
  //0, 200, DISPON 
  // ---------------------------------------
};

/** @var array Chache memory char index row */
unsigned short int cacheMemIndexRow = 0;
/** @var array Chache memory char index column */
unsigned short int cacheMemIndexCol = 0;

/**
 * @desc    Hardware Reset
 *
 * @param   struct signal *
 *
 * @return  void
 */
void ST7735_Reset ( void )
{
  // Actiavte pull-up resistor logical high on pin RST
  // posible write: SET_BIT (*(*reset).port, reset->pin)
  //SET_BIT (*(reset->port), reset->pin);
  //Cy_GPIO_Write(RSTN_PORT, RSTN_NUM, 1);
    
  // DDR as output
  //SET_BIT (*(reset->ddr), reset->pin);
  // delay 200 ms
  _delay_ms(200);
  // Reset Low 
  //CLR_BIT (*(reset->port), reset->pin);
    //Cy_GPIO_Write(RSTN_PORT, RSTN_NUM, 0);
  // delay 200 ms
  _delay_ms(200);
  // Reset High
  //SET_BIT (*(reset->port), reset->pin);
  //Cy_GPIO_Write(RSTN_PORT, RSTN_NUM, 1);
}

/**
 * @desc    Init SPI
 *
 * @param   void
 *
 * @return  void
 */
void ST7735_SPI_Init (void)
{
  
    //cy_en_scb_spi_status_t initStatus;
        
    /* Configure component */
    //initStatus = Cy_SCB_SPI_Init(SPI_HW, &SPI_config, NULL);
        
    //Cy_SCB_SPI_Enable(SPI_HW);
    
    
    __enable_irq(); /* Enable global interrupts. */
    
    
    // Output: SCK, MOSI 
  //SET_BIT (DDR, ST7735_SCK);
  //SET_BIT (DDR, ST7735_MOSI);
/*
  // Input: MISO with pullup
  CLR_BIT (DDR, ST7735_MOSI);
  PORT_BIT (PORT, ST7735_MOSI);
*/
  // SPE  - SPI Enale
  // MSTR - Master device
  //SET_BIT (SPCR, SPE);
  //SET_BIT (SPCR, MSTR);
  // SPI2X - Prescaler fclk/2 = 8MHz
  //SET_BIT (SPSR, SPI2X);
}

/**
 * @desc    Init Pins
 *
 * @param   struct st7735 *
 *
 * @return  void
 */
void ST7735_Pins_Init ( void )
{
  // DDR
  // --------------------------------------
  //SET_BIT (*(lcd->cs->ddr), lcd->cs->pin);
  //SET_BIT (*(lcd->bl->ddr), lcd->bl->pin);
  //SET_BIT (*(lcd->dc->ddr), lcd->dc->pin);
  // PORT
  // --------------------------------------
  //SET_BIT (*(lcd->cs->port), lcd->cs->pin);   // Chip Select H
  //SET_BIT (*(lcd->bl->port), lcd->bl->pin);   // BackLigt ON
}

/**
 * @desc    Init st7735 driver
 *
 * @param   struct st7735 *
 *
 * @return  void
 */
void ST7735_Init ( void )
{
  // init pins
  ST7735_Pins_Init ();
  // init SPI
  ST7735_SPI_Init ();
  // hardware reset
  ST7735_Reset ();
  // load list of commands
  ST7735_Commands (INIT_ST7735B);
}

/**
 * @desc    Send list commands
 *
 * @param   struct st7735 *
 * @param   const uint8_t *
 *
 * @return  void
 */
void ST7735_Commands (const uint8_t * initializers)
{
  uint8_t args;
  uint8_t cmnd;
  uint8_t time;
  uint8_t loop = pgm_read_byte (initializers++);

  // loop through whole initializer list
  while (loop--) {

    // 1st arg - number of command arguments
    args = pgm_read_byte (initializers++);
    // 2nd arg - delay time
    time = pgm_read_byte (initializers++);
    // 3th arg - command
    cmnd = pgm_read_byte (initializers++);

    // send command
    ST7735_CommandSend (cmnd);
    // send arguments
    while (args--) {
      // send argument
      ST7735_Data8BitsSend (pgm_read_byte (initializers++));
    }
    // delay
    ST7735_DelayMs (time);
  }
}

/**
 * @desc    Command send
 *
 * @param   struct st7735 *
 * @param   uint8_t
 *
 * @return void
 */
uint8_t ST7735_CommandSend (uint8_t data)
{
    uint32_t masterStatus;
    uint8_t d;
    int count;
    
    // chip enable - active low
    //CLR_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 0);
    // command (active low)
    //CLR_BIT (*(lcd->dc->port), lcd->dc->pin);
    //Cy_GPIO_Write(DC_PORT, DC_NUM, 0);
    // transmitting data
      
    /* Clear Master status and Tx FIFO status. */
    //Cy_SCB_SPI_ClearSlaveMasterStatus(SPI_HW, masterStatus);
    //Cy_SCB_SPI_ClearTxFifoStatus(SPI_HW, CY_SCB_SPI_TX_INTR_MASK );
    //Cy_SCB_SPI_ClearTxFifo(SPI_HW);
    
    /* Clear Rx FIFO status. */
    //Cy_SCB_SPI_ClearRxFifoStatus(SPI_HW, CY_SCB_SPI_RX_INTR_MASK );
    //Cy_SCB_SPI_ClearRxFifo(SPI_HW);
    
    
    
    //Cy_SCB_SPI_Write(SPI_HW, data);
    
    do {
        //count = Cy_SCB_SPI_GetNumInRxFifo(SPI_HW);
    } while (count < 1);
        
    //do
    //{
    //    masterStatus  = Cy_SCB_SPI_GetSlaveMasterStatus(SPI_HW);
    //    
    //}while ( (masterStatus != (CY_SCB_SPI_MASTER_DONE)) );
    
    //d = Cy_SCB_SPI_Read(SPI_HW);  
    
    
    
    //SPDR = data;
    // wait till data transmit
    //WAIT_UNTIL_BIT_IS_SET (SPSR, SPIF);
    // chip disable - idle high
    //SET_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 1);
    // return received data
    return d;
}

/**
 * @desc    8bits data send
 *
 * @param   struct st7735 *
 * @param   uint8_t
 *
 * @return  void
 */
uint8_t ST7735_Data8BitsSend (uint8_t data)
{
    uint32_t masterStatus;
    uint8_t d;
    int count;
    
    // chip enable - active low
    //CLR_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 0);
    // data (active high)
    //SET_BIT (*(lcd->dc->port), lcd->dc->pin);
    //Cy_GPIO_Write(DC_PORT, DC_NUM, 1);
    // transmitting data
      
    /* Clear Master status and Tx FIFO status. */
    //Cy_SCB_SPI_ClearSlaveMasterStatus(SPI_HW, masterStatus);
    //Cy_SCB_SPI_ClearTxFifoStatus(SPI_HW, CY_SCB_SPI_TX_INTR_MASK );
    //Cy_SCB_SPI_ClearTxFifo(SPI_HW);
    
    /* Clear Rx FIFO status. */
    //Cy_SCB_SPI_ClearRxFifoStatus(SPI_HW, CY_SCB_SPI_RX_INTR_MASK );
   // Cy_SCB_SPI_ClearRxFifo(SPI_HW);
    
    
    
    //Cy_SCB_SPI_Write(SPI_HW, data);
    
    do {
        //count = Cy_SCB_SPI_GetNumInRxFifo(SPI_HW);
    } while (count < 1);
        
    //do
    //{
    //    masterStatus  = Cy_SCB_SPI_GetSlaveMasterStatus(SPI_HW);
    //    
    //}while ( (masterStatus != (CY_SCB_SPI_MASTER_DONE)) );
    
    //d = Cy_SCB_SPI_Read(SPI_HW);  
    
    
    
    //SPDR = data;
    // wait till data transmit
    //WAIT_UNTIL_BIT_IS_SET (SPSR, SPIF);
    // chip disable - idle high
    //SET_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 1);
    // return received data
    return d;
}

/**
 * @desc    16bits data send
 *
 * @param   struct st7735 * lcd
 * @param   uint16_t
 *
 * @return  void
 */
uint8_t ST7735_Data16BitsSend (uint16_t data)
{
    uint32_t masterStatus;
    uint8_t d[2];
    int count;
        
    // chip enable - active low
    //CLR_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 0);
    // data (active high)
    //SET_BIT (*(lcd->dc->port), lcd->dc->pin);
    //Cy_GPIO_Write(DC_PORT, DC_NUM, 1);
    
    
    /* Clear Master status and Tx FIFO status. */
    //Cy_SCB_SPI_ClearSlaveMasterStatus(SPI_HW, masterStatus);
    //Cy_SCB_SPI_ClearTxFifoStatus(SPI_HW, CY_SCB_SPI_TX_INTR_MASK );
   // Cy_SCB_SPI_ClearTxFifo(SPI_HW);
    
    /* Clear Rx FIFO status. */
    //Cy_SCB_SPI_ClearRxFifoStatus(SPI_HW, CY_SCB_SPI_RX_INTR_MASK );
    //Cy_SCB_SPI_ClearRxFifo(SPI_HW);
    
    d[0] = data>>8;
    d[1] = data & 0xFF;
    
    //Cy_SCB_SPI_WriteArray(SPI_HW, d, 2);
    
    do {
        //count = Cy_SCB_SPI_GetNumInRxFifo(SPI_HW);
    } while (count < 2);
    
    //do
    //{
    //    masterStatus  = Cy_SCB_SPI_GetSlaveMasterStatus(SPI_HW);
    //    
    //}while ( (masterStatus != (CY_SCB_SPI_MASTER_DONE)) );
    
    //d[0] = Cy_SCB_SPI_Read(SPI_HW); 
    //d[1] = Cy_SCB_SPI_Read(SPI_HW); 
    
    
    // transmitting data high byte
    //SPDR = (uint8_t) (data >> 8);
    // wait till high byte transmit
    //WAIT_UNTIL_BIT_IS_SET (SPSR, SPIF);
    // transmitting data low byte
    //SPDR = (uint8_t) (data);
    // wait till low byte transmit
    //WAIT_UNTIL_BIT_IS_SET (SPSR, SPIF);
    // chip disable - idle high
    //SET_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 1);
    // return received data
    return ST7735_SUCCESS;
}

/**
 * @desc    Set window
 *
 * @param   struct st7735 * lcd
 * @param   uint8_t x - start position
 * @param   uint8_t x - end position
 * @param   uint8_t y - start position
 * @param   uint8_t y - end position
 *
 * @return  uint8_t
 */
uint8_t ST7735_SetWindow (uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
  uint8_t n0;
  uint8_t n1;
    
  // check if coordinates is out of range
  if ((x0 > x1)     ||
      (x1 > SIZE_X) ||
      (y0 > y1)     ||
      (y1 > SIZE_Y)) { 
    // out of range
    return ST7735_ERROR;
  }  
    
  //180 degree rotate
  
#ifdef SWAP_X    
    n0 = SIZE_X - x1;
    n1 = SIZE_X - x0;
    x0 = n0;
    x1 = n1;
    
    x0 += 1;
    x1 += 1;
    
#endif
    
#ifdef SWAP_Y
    n0 = SIZE_Y - y1;
    n1 = SIZE_Y - y0;
    y0 = n0;
    y1 = n1;
    
    y0 += 2;
    y1 += 2;
#else
    y0 += Y_OFFSET;
    y1 += Y_OFFSET; 
#endif


  // column address set
  ST7735_CommandSend (CASET);
  // send start x position
  ST7735_Data16BitsSend (0x0000 | x0);
  // send end x position
  ST7735_Data16BitsSend (0x0000 | x1);

  

  // row address set
  ST7735_CommandSend (RASET);
  // send start y position
  ST7735_Data16BitsSend (0x0000 | y0);
  // send end y position
  ST7735_Data16BitsSend (0x0000 | y1);

  // success
  return ST7735_SUCCESS;
}

/**
 * @desc    Write color pixels
 *
 * @param   struct st7735 * lcd
 * @param   uint16_t color
 * @param   uint16_t counter
 *
 * @return  void
 */
void ST7735_SendColor565 (uint16_t color, uint16_t count)
{
  // access to RAM
  ST7735_CommandSend (RAMWR);
  // counter
  while (count--) {
    // write color
    ST7735_Data16BitsSend (color);
  }
}

/**
 * @desc    Draw pixel
 *
 * @param   struct st7735 * lcd
 * @param   uint8_t x position / 0 <= cols <= MAX_X-1
 * @param   uint8_t y position / 0 <= rows <= MAX_Y-1
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7735_DrawPixel (uint8_t x, uint8_t y, uint16_t color)
{
  // set window
  ST7735_SetWindow (x, x, y, y);
  // draw pixel by 565 mode
  ST7735_SendColor565 (color, 1);
}

/**
 * @desc    RAM Content Show
 *
 * @param   struct st7735 * lcd
 *
 * @return  void
 */
void ST7735_RAM_Content_Show ( void )
{
  // display content on
  ST7735_CommandSend (DISPON);
}

/**
 * @desc    RAM Content Hide
 *
 * @param   struct st7735 * lcd
 *
 * @return  void
 */
void ST7735_RAM_Content_Hide ( void )
{
  // display content off
  ST7735_CommandSend (DISPOFF);
}

/**
 * @desc    Clear screen
 *
 * @param   struct st7735 *
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7735_ClearScreen (uint16_t color)
{
  // set whole window
  ST7735_SetWindow (0, SIZE_X, 0, SIZE_Y);
  // draw individual pixels
  ST7735_SendColor565 (color, CACHE_SIZE_MEM);
}

/**
 * @desc    Draw character
 *
 * @param   struct st7735 *
 * @param   char character
 * @param   uint16_t color
 * @param   enum Size (X1, X2, X3)
 *
 * @return  void
 */
char ST7735_DrawChar (char character, uint16_t color, enum Size size)
{
  // variables
  uint8_t letter, idxCol, idxRow;
  // check if character is out of range
  if ((character < 0x20) &&
      (character > 0x7f)) { 
    // out of range
    return 0;
  }
  // last column of character array - 5 columns 
  idxCol = CHARS_COLS_LEN;
  // last row of character array - 8 rows / bits
  idxRow = CHARS_ROWS_LEN;

  // --------------------------------------
  // SIZE X1 - normal font 1x high, 1x wide
  // --------------------------------------
  if (size == X1) {  
    // loop through 5 bits
    while (idxCol--) {
      // read from ROM memory 
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      // loop through 8 bits
      while (idxRow--) {
        // check if bit set
        if (letter & (1 << idxRow)) {
          // draw pixel 
          ST7735_DrawPixel (cacheMemIndexCol + idxCol, cacheMemIndexRow + idxRow, color);
        }
      }
      // fill index row again
      idxRow = CHARS_ROWS_LEN;
    }
    // update x position
    cacheMemIndexCol = cacheMemIndexCol + CHARS_COLS_LEN + 1;
  
  // --------------------------------------
  // SIZE X2 - font 2x higher, normal wide
  // --------------------------------------
  } else if (size == X2) {
    // loop through 5 bytes
    while (idxCol--) {
      // read from ROM memory 
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      // loop through 8 bits
      while (idxRow--) {
        // check if bit set
        if (letter & (1 << idxRow)) {
          // draw first left up pixel; 
          // (idxRow << 1) - 2x multiplied 
          ST7735_DrawPixel (cacheMemIndexCol + idxCol, cacheMemIndexRow + (idxRow << 1), color);
          // draw second left down pixel
          ST7735_DrawPixel (cacheMemIndexCol + idxCol, cacheMemIndexRow + (idxRow << 1) + 1, color);
        }
      }
      // fill index row again
      idxRow = CHARS_ROWS_LEN;
    }
    // update x position
    cacheMemIndexCol = cacheMemIndexCol + CHARS_COLS_LEN + 1;

  // --------------------------------------
  // SIZE X3 - font 2x higher, 2x wider
  // --------------------------------------
  } else if (size == X3) {
    // loop through 5 bytes
    while (idxCol--) {
      // read from ROM memory 
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      // loop through 8 bits
      while (idxRow--) {
        // check if bit set
        if (letter & (1 << idxRow)) {
          // draw first left up pixel; 
          // (idxRow << 1) - 2x multiplied 
          ST7735_DrawPixel (cacheMemIndexCol + (idxCol << 1), cacheMemIndexRow + (idxRow << 1), color);
          // draw second left down pixel
          ST7735_DrawPixel (cacheMemIndexCol + (idxCol << 1), cacheMemIndexRow + (idxRow << 1) + 1, color);
          // draw third right up pixel
          ST7735_DrawPixel (cacheMemIndexCol + (idxCol << 1) + 1, cacheMemIndexRow + (idxRow << 1), color);
          // draw fourth right down pixel
          ST7735_DrawPixel (cacheMemIndexCol + (idxCol << 1) + 1, cacheMemIndexRow + (idxRow << 1) + 1, color);
        }
      }
      // fill index row again
      idxRow = CHARS_ROWS_LEN;
    }

    // update x position
    cacheMemIndexCol = cacheMemIndexCol + CHARS_COLS_LEN + CHARS_COLS_LEN + 1;
  }

  // return exit
  return ST7735_SUCCESS;
}

/**
 * @desc    Set text position x, y
 *
 * @param   uint8_t x - position
 * @param   uint8_t y - position
 *
 * @return  char
 */
char ST7735_SetPosition (uint8_t x, uint8_t y)
{
  // check if coordinates is out of range
  if ((x > MAX_X) && (y > MAX_Y)) {
    // error
    return ST7735_ERROR;

  } else if ((x > MAX_X) && (y <= MAX_Y)) {
    // set position y
    cacheMemIndexRow = y;
    // set position x
    cacheMemIndexCol = 2;
  } else {
    // set position y 
    cacheMemIndexRow = y;
    // set position x
    cacheMemIndexCol = x;
  }
  // success
  return ST7735_SUCCESS;
}

/**
 * @desc    Check text position x, y
 *
 * @param   unsigned char x - position
 * @param   unsigned char y - position
 * @param   unsigned char
 *
 * @return  char
 */
char ST7735_CheckPosition (unsigned char x, unsigned char y, unsigned char max_y, enum Size size)
{
  // check if coordinates is out of range
  if ((x > MAX_X) && (y > max_y)) {
    // out of range
    return ST7735_ERROR;

  }
  // if next line
  if ((x > MAX_X) && (y <= max_y)) {
    // set position y
    cacheMemIndexRow = y;
    // set position x
    cacheMemIndexCol = 2;
  } 

  // success
  return ST7735_SUCCESS;
}

/**
 * @desc    Draw string
 *
 * @param   struct st7735 *
 * @param   char * string 
 * @param   uint16_t color
 * @param   enum Size (X1, X2, X3)
 *
 * @return  void
 */
void ST7735_DrawString (char *str, uint16_t color, enum Size size)
{
  // variables
  unsigned int i = 0;
  unsigned char check;
  unsigned char delta_y;
  unsigned char max_y_pos;
  unsigned char new_x_pos;
  unsigned char new_y_pos;

  // loop through character of string
  while (str[i] != '\0') {
    // max x position character
    new_x_pos = cacheMemIndexCol + CHARS_COLS_LEN + (size & 0x0F);
    // delta y
    delta_y = CHARS_ROWS_LEN + (size >> 4);
    // max y position character
    new_y_pos = cacheMemIndexRow + delta_y;
    // max y pos
    max_y_pos = MAX_Y - delta_y;
    // control if will be in range
    check = ST7735_CheckPosition (new_x_pos, new_y_pos, max_y_pos, size);
    // update position
    if (ST7735_SUCCESS == check) {
      // read characters and increment index
      ST7735_DrawChar (str[i++], color, size);
    }
  }
}

/**
 * @desc    Draw line by Bresenham algoritm
 * @surce   https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *  
 * @param   struct st7735 *
 * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint8_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1 
 * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
 * @param   uint16_t color
 *
 * @return  void
 */
char ST7735_DrawLine (uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint16_t color)
{
  // determinant
  int16_t D;
  // deltas
  int16_t delta_x, delta_y;
  // steps
  int16_t trace_x = 1, trace_y = 1;

  // delta x
  delta_x = x2 - x1;
  // delta y
  delta_y = y2 - y1;

  // check if x2 > x1
  if (delta_x < 0) {
    // negate delta x
    delta_x = -delta_x;
    // negate step x
    trace_x = -trace_x;
  }

  // check if y2 > y1
  if (delta_y < 0) {
    // negate detla y
    delta_y = -delta_y;
    // negate step y
    trace_y = -trace_y;
  }

  // Bresenham condition for m < 1 (dy < dx)
  if (delta_y < delta_x) {
    // calculate determinant
    D = (delta_y << 1) - delta_x;
    // draw first pixel
    ST7735_DrawPixel (x1, y1, color);
    // check if x1 equal x2
    while (x1 != x2) {
      // update x1
      x1 += trace_x;
      // check if determinant is positive
      if (D >= 0) {
        // update y1
        y1 += trace_y;
        // update determinant
        D -= 2*delta_x;    
      }
      // update deteminant
      D += 2*delta_y;
      // draw next pixel
      ST7735_DrawPixel (x1, y1, color);
    }
  // for m > 1 (dy > dx)    
  } else {
    // calculate determinant
    D = delta_y - (delta_x << 1);
    // draw first pixel
    ST7735_DrawPixel (x1, y1, color);
    // check if y2 equal y1
    while (y1 != y2) {
      // update y1
      y1 += trace_y;
      // check if determinant is positive
      if (D <= 0) {
        // update y1
        x1 += trace_x;
        // update determinant
        D += 2*delta_y;    
      }
      // update deteminant
      D -= 2*delta_x;
      // draw next pixel
      ST7735_DrawPixel (x1, y1, color);
    }
  }
  // success return
  return 1;
}

/**
 * @desc    Fast draw line horizontal
 *
 * @param   struct st7735 *
 * @param   uint8_t xs - start position
 * @param   uint8_t xe - end position
 * @param   uint8_t y - position
 * @param   uint16_t color
 *
 * @return void
 */
void ST7735_DrawLineHorizontal (uint8_t xs, uint8_t xe, uint8_t y, uint16_t color)
{
  uint8_t temp;
  // check if start is > as end  
  if (xs > xe) {
    // temporary safe
    temp = xs;
    // start change for end
    xe = xs;
    // end change for start
    xs = temp;
  }
  // set window
  ST7735_SetWindow (xs, xe, y, y);
  // draw pixel by 565 mode
  ST7735_SendColor565 (color, xe - xs);
}

/**
 * @desc    Fast draw line vertical
 *
 * @param   struct st7735 *
 * @param   uint8_t x - position
 * @param   uint8_t ys - start position
 * @param   uint8_t ye - end position
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7735_DrawLineVertical (uint8_t x, uint8_t ys, uint8_t ye, uint16_t color)
{
  uint8_t temp;
  // check if start is > as end
  if (ys > ye) {
    // temporary safe
    temp = ys;
    // start change for end
    ye = ys;
    // end change for start
    ys = temp;
  }
  // set window
  ST7735_SetWindow (x, x, ys, ye);
  // draw pixel by 565 mode
  ST7735_SendColor565 (color, ye - ys);
}

/**
 * @desc    Draw rectangle
 *
 * @param   struct st7735 *
 * @param   uint8_t x start position
 * @param   uint8_t x end position
 * @param   uint8_t y start position
 * @param   uint8_t y end position
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7735_DrawRectangle (uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye, uint16_t color)
{
  uint8_t temp;
  // check if start is > as end  
  if (xs > xe) {
    // temporary safe
    temp = xe;
    // start change for end
    xe = xs;
    // end change for start
    xs = temp;
  }
  // check if start is > as end
  if (ys > ye) {
    // temporary safe
    temp = ye;
    // start change for end
    ye = ys;
    // end change for start
    ys = temp;
  }
  // set window
  ST7735_SetWindow (xs, xe, ys, ye);
  // send color
  ST7735_SendColor565 (color, (xe-xs+1)*(ye-ys+1));  
}

/**
 * @desc    Delay
 *
 * @param   uint8_t time in milliseconds / max 256ms
 *
 * @return  void
 */
void ST7735_DelayMs (uint8_t time)
{
  // loop through real time
  while (time--) {
    // 1 s delay
    _delay_ms(1);
  }
}
