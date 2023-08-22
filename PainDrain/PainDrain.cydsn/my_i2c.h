
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

#if !defined(my_i2c_H)
#define my_i2c_H
	
#include <project.h>

#define myI2C_SUCCESS 0
#define myI2C_FAILED 1
	
	
//States	
#define myI2C_IDLE 0
#define myI2C_ADDR 1
#define myI2C_WRITE 2
#define myI2C_READ 3

/*******************************************************************************
* Function Name: myI2C_SCLidle
********************************************************************************
*
* Summary:
*  Checks if SCL is idle, fails if longer than 50us
*
* Parameters:
* 	none
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_SCLidle( void );

/*******************************************************************************
* Function Name: myI2C_SDAidle
********************************************************************************
*
* Summary:
*  Checks if SDA is idle, fails if longer than 50us
*
* Parameters:
* 	none
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_SDAidle( void );
	
/*******************************************************************************
* Function Name: myI2C_Start
********************************************************************************
*
* Summary:
*  Initializes myI2C code
*
* Parameters:
* 	none
*
* Return:
*  none
*
*******************************************************************************/
void myI2C_Start( void );


/*******************************************************************************
* Function Name: myI2C_Start
********************************************************************************
*
* Summary:
*  Clears myI2C code
*
* Parameters:
* 	none
*
* Return:
*  none
*
*******************************************************************************/
void myI2C_Stop( void );

/*******************************************************************************
* Function Name: myI2C_I2CMasterSendStart
********************************************************************************
*
* Summary:
*  Sends start condition and slave device address
*
* Parameters:
* 	slaveAddress - 7-bit slave address
*   bitRnW - 0 = write, 1 = read
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_I2CMasterSendStart(uint32 slaveAddress, uint32 bitRnW);

/*******************************************************************************
* Function Name: myI2C_I2CMasterSendRestart
********************************************************************************
*
* Summary:
*  Sends start condition and slave device address
*
* Parameters:
* 	slaveAddress - 7-bit slave address
*   bitRnW - 0 = write, 1 = read
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_I2CMasterSendRestart(uint32 slaveAddress, uint32 bitRnW);
	
/*******************************************************************************
* Function Name: myI2C_I2CMasterSendStop
********************************************************************************
*
* Summary:
*  Sends stop condition
*
* Parameters:
*	none
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_I2CMasterSendStop( void );

/*******************************************************************************
* Function Name: myI2C_I2CMasterClearStatus
********************************************************************************
*
* Summary:
*  Clears state
*
* Parameters:
*	none
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_I2CMasterClearStatus( void );


/*******************************************************************************
* Function Name: myI2C_I2CMasterWriteByte
********************************************************************************
*
* Summary:
*  Sends a byte and checks the ack
*
* Parameters:
* 	theByte - data
*
* Return:
*  error
*
*******************************************************************************/
uint32 myI2C_I2CMasterWriteByte(uint32 theByte);
	

/*******************************************************************************
* Function Name: myI2C_I2CMasterReadByte
********************************************************************************
*
* Summary:
*  Reads a byte and asserts ack or nack
*
* Parameters:
* 	ackNack - 1 assert ACK, 0 assert NACK
*
* Return:
*  read data
*
*******************************************************************************/
uint32  myI2C_I2CMasterReadByte(uint32 ackNack);

#endif /* my_i2c_H */

/* [] END OF FILE */
