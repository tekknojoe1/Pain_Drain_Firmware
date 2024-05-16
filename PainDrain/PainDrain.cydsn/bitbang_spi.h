
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

#if !defined(bitbang_spi_H)
#define bitbang_spi_H
	
#include <project.h>

void bbSPI_write(uint16 buffer[], uint8 count);
void bbSPI_read(uint16 buffer[], uint8 count);
void bbSPI_dummy_clock_cycle(uint8 count);

#endif /* bitbang_spi_H */

/* [] END OF FILE */
