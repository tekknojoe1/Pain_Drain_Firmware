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

#include <bitbang_spi.h>

void bbSPI_write(uint16 buffer[], uint8 count) {

    uint16 wd, rd;
    int i, j;
    
    Cy_GPIO_SetDrivemode(DISP_MOSI_PORT, DISP_MOSI_NUM, CY_GPIO_DM_STRONG);
    
    for (j=0;j<count;j++) {
        wd = buffer[j];
        rd = 0;
        
        for (i=0;i<9;i++) {
            
            rd <<= 1;
            
            if (wd & 0x100) 
                //ISD_MOSI_Write(1); //MSB out first
                Cy_GPIO_Write(DISP_MOSI_PORT, DISP_MOSI_NUM, 1);
            else
                //ISD_MOSI_Write(0); //MSB out first
                Cy_GPIO_Write(DISP_MOSI_PORT, DISP_MOSI_NUM, 0);
            wd <<= 1;
            
            CyDelayUs(1);
            
            //ISD_SCK_Write(0);
            Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 1);
            
            CyDelayUs(1);
            
            //rd |= (ISD_MISO_Read() & 0x01);
            
            //ISD_SCK_Write(1);
            Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 0);
                        
        }
        
        //buffer[j] = rd;
        
    }
	
}

void bbSPI_read(uint16 buffer[], uint8 count) {

    uint16 wd, rd;
    int i, j;
    
    Cy_GPIO_SetDrivemode(DISP_MOSI_PORT, DISP_MOSI_NUM, CY_GPIO_DM_PULLUP);
    
    for (j=0;j<count;j++) {
        wd = buffer[j];
        rd = 0;
        
        for (i=0;i<8;i++) {
            
            rd <<= 1;
            
            //if (wd & 0x80) 
            //    //ISD_MOSI_Write(1); //MSB out first
            //    Cy_GPIO_Write(DISP_MOSI_PORT, DISP_MOSI_NUM, 1);
            //else
            //    //ISD_MOSI_Write(0); //MSB out first
            //    Cy_GPIO_Write(DISP_MOSI_PORT, DISP_MOSI_NUM, 0);
            wd <<= 1;
            
            CyDelayUs(1);
            
            //ISD_SCK_Write(0);
            Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 1);
            
            CyDelayUs(1);
            
            //rd |= (ISD_MISO_Read() & 0x01);
            rd |= ( Cy_GPIO_Read(DISP_MOSI_PORT, DISP_MOSI_NUM) & 0x01);
            
            //ISD_SCK_Write(1);
            Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 0);
                        
        }
        
        buffer[j] = rd;
        
    }
	
}

void bbSPI_dummy_clock_cycle(uint8 count) {
    int j;
    for (j=0;j<count;j++) {
        CyDelayUs(1);
        Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 1);
        CyDelayUs(1);
        Cy_GPIO_Write(DISP_SCK_PORT, DISP_SCK_NUM, 0);
    }
}



/* [] END OF FILE */
