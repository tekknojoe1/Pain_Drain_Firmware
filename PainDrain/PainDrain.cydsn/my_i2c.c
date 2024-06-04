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

#include <my_i2c.h>
#include "power.h"
#include "debug.h"

int myI2C_state = myI2C_IDLE;


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
uint32 myI2C_SCLidle( void ) {
	int i;
	
	for (i=0; i<50; i++) {
		if (I2C_SCL_Read() == 1) return myI2C_SUCCESS;
		CyDelayUs(1);
	}
	
	return myI2C_FAILED;
}
	
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
uint32 myI2C_SDAidle( void ) {
	int i;
	
	for (i=0; i<50; i++) {
		if (I2C_SDA_Read() == 1) return myI2C_SUCCESS;
		CyDelayUs(1);
	}
	
	return myI2C_FAILED;	
}

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
*  error
*
*******************************************************************************/
void myI2C_Start( void ) {
	
	myI2C_I2CMasterClearStatus();
	
}

/*******************************************************************************
* Function Name: myI2C_Stop
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
void myI2C_Stop( void ) {
	
	myI2C_I2CMasterClearStatus();
}

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
*  none
*
*******************************************************************************/
uint32 myI2C_I2CMasterSendStart(uint32 slaveAddress, uint32 bitRnW) {
	
	if (myI2C_state != myI2C_IDLE) {
		return myI2C_FAILED;  //I should be in idle state when called
	}
	
	return myI2C_I2CMasterSendRestart(slaveAddress, bitRnW);
	
}

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
uint32 myI2C_I2CMasterSendRestart(uint32 slaveAddress, uint32 bitRnW) {
	uint8 sa;
	uint32 ack;
	
	sa = (slaveAddress<<1 & 0xFE) | (bitRnW & 0x1);
		
	I2C_SDA_Write(1);
	if (myI2C_SDAidle() == myI2C_FAILED)
		return myI2C_FAILED;
	
	CyDelayUs(8);
	
	I2C_SCL_Write(1);
	if (myI2C_SCLidle() == myI2C_FAILED)
		return myI2C_FAILED;
	
	CyDelayUs(8);
	
	I2C_SDA_Write(0);
	if (I2C_SDA_Read() != 0) {
		
		I2C_SCL_Write(1);
		I2C_SDA_Write(1);
		
		return myI2C_FAILED;
	}
		
	CyDelayUs(8);
	
	I2C_SCL_Write(0);
	if (I2C_SCL_Read() != 0) {
		
		I2C_SCL_Write(1);
		I2C_SDA_Write(1);
		DBG_PRINTF("Inside myI2C_I2CMasterSendRestart, myI2C_FAILED:  %d \r\n", ack);
		return myI2C_FAILED;
	}
	
	CyDelayUs(8);
	
	myI2C_state = myI2C_ADDR;
	
	ack = myI2C_I2CMasterWriteByte(sa);
    DBG_PRINTF("Status: %d \r\n", ack);
	
	if (ack) {
		myI2C_I2CMasterSendStop();
		myI2C_I2CMasterClearStatus();
	} else {
		if (bitRnW == 0) {
			myI2C_state = myI2C_WRITE;
		} else {
			myI2C_state = myI2C_READ;
		}
	}
	DBG_PRINTF("Inside myI2C_I2CMasterSendRestart, ack:  %d \r\n", ack);
	return ack;
}
	


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
uint32 myI2C_I2CMasterSendStop( void ) {
	
	CyDelayUs(8);
	
	I2C_SCL_Write(0); //SCL must start low
	
	CyDelayUs(8);
	
	I2C_SDA_Write(0); //SDA must start low
	
	CyDelayUs(8);
	
	I2C_SCL_Write(1);
	if (myI2C_SCLidle() == myI2C_FAILED)
		return myI2C_FAILED;
	
	CyDelayUs(8);
	
	I2C_SDA_Write(1);
	if (myI2C_SDAidle() == myI2C_FAILED)
		return myI2C_FAILED;
	
	
	CyDelayUs(8);
	
	myI2C_state = myI2C_IDLE;
	
	return myI2C_SUCCESS;
	
}

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
uint32 myI2C_I2CMasterClearStatus( void ) {
	
	I2C_SDA_Write(1);
	I2C_SCL_Write(1);
	CyDelayUs(4);
	
	myI2C_state = myI2C_IDLE;
	
	return myI2C_SUCCESS;

}

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
uint32 myI2C_I2CMasterWriteByte(uint32 theByte) 
{
	int i;
	uint32 d;
	uint32 ack;
	uint32 b;
	
	d = theByte;
	
	//SCL and SDA should be low
	
	for (i=0; i<8; i++) 
    {
		b = ( (d & 0x80) == 0x80);
		I2C_SDA_Write(b);
		CyDelayUs(4);
		I2C_SCL_Write(1);
		
		if (myI2C_SCLidle() == myI2C_FAILED)
			return myI2C_FAILED;
		
		CyDelayUs(4);
		I2C_SCL_Write(0);
		
		d <<= 1;
	}
	
	//Ack
	I2C_SDA_Write(1);
	
	CyDelayUs(4);
	I2C_SCL_Write(1);
	CyDelayUs(4);
	
	ack = I2C_SDA_Read();
	
	I2C_SCL_Write(0);
	
	if (ack) 
    {
		myI2C_I2CMasterSendStop();
		myI2C_I2CMasterClearStatus();
	}
	DBG_PRINTF("Inside myI2C_I2CMasterWriteByte, ack:  %d \r\n", ack);	
	return ack;
	
}

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
uint32  myI2C_I2CMasterReadByte(uint32 ackNack) {
	
	int i;
	uint32 d = 0;
	
	//SCL and SDA should be low
	
	//Release SDA
	I2C_SDA_Write(1);
	
	for (i=0; i<8; i++) {
		CyDelayUs(4);
		I2C_SCL_Write(1);
		
		if (myI2C_SCLidle() == myI2C_FAILED){
            DBG_PRINTF("Inside myI2C_I2CMasterReadByte, myI2C_FAILED:  %d \r\n", d);
			return myI2C_FAILED;
        }
		
		CyDelayUs(4);
		
		//Sample SDA
		d <<= 1;
		d |= I2C_SDA_Read();
		
		I2C_SCL_Write(0);
	}
	
	//Ack
	if (ackNack) {
		I2C_SDA_Write(0);
	} else {
		I2C_SDA_Write(1);
	}
	
	CyDelayUs(4);
	I2C_SCL_Write(1);
	CyDelayUs(4);
	I2C_SCL_Write(0);
	DBG_PRINTF("Inside myI2C_I2CMasterReadByte, ackNack:  %d \r\n", ackNack);
	return d;
		
}


/* [] END OF FILE */
