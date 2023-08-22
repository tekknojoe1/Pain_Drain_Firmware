/******************************************************************************
*This file is for myprintf()
*The myprintf() is a simple myprintf() and only can print string with %s,%d,%c,%x.
*******************************************************************************/

#include "myUART.h"
#include "myprintf.h"

//static int len = 0;
//static char8 iprintstr[256];

static void iputc(char8 ch)
{
	/*This function has to be replaced by user*/	
	myUART_PutChar(ch);
	//add to the fifo
	//iprintstr[len++] = ch;
}

static uint8* change(uint32 Index)
{
    return (uint8*)("0123456789abcdef"+Index);
}


void myprintf(char8 *pszFmt,...)
{
    uint8 *pszVal;
    uint32 xVal, i = 0, buffer[24], index = 1;
    int32 iVal;
    uint8 cVal;
    uint32 *pArg;
    pArg =(uint32 *)&pszFmt;
	
	//len = 0;

    while(*pszFmt)
    {
        if('%' != *pszFmt)
        {
            iputc(*pszFmt);
            pszFmt++;
            continue;
        }
        pszFmt++;

        if(*pszFmt == 's')
        {
            pszVal = (uint8*)pArg[index++];
            for(; *pszVal != '\0'; pszVal++)
                iputc(*pszVal);
            pszFmt++;
            continue;
        }
        if(*pszFmt == 'd')
        {
            iVal = pArg[index++];
            
            if (iVal < 0) {
                iputc('-'); 
                iVal = ~iVal; 
                iVal++;   
            }
                       
            i = 0;
            do{
                buffer[i++] = iVal % 10;
                iVal /= 10;
            }while(iVal);
            while(i > 0)
            {
                i--;
                iputc(*change(buffer[i]));
            }
            pszFmt++;
            continue;
        }
        if(*pszFmt == 'c')
        {
            cVal = (uint8)pArg[index++];
            iputc(cVal);
            pszFmt++;
            continue;
        }
        if(*pszFmt == 'x')
        {
            xVal = pArg[index++];
            i = 0;
            do{
                buffer[i++] = xVal % 16;
                xVal /= 16;
            }while(xVal);
            if(i%2!=0)
                buffer[i++]=0;
            if(i<2)
                buffer[i++]=0;

            while(i > 0)
            {
                i--;
                iputc(*change(buffer[i]));
            }
            pszFmt++;
            continue;
        }
        if(pszFmt == '\0')
        {
            break;
        }

			
    }
	
	//len = usb_put_data(iprintstr, len);
	
}