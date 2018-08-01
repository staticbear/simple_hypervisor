#include "serialport_log64.h"
#include "types.h"
#include "inline_asm.h"

/*---------------------------------------------------------------------------------------------------*/

void SerialPrintStr64(BYTE *ptrStr)
{
    int i = 0;
    if(ptrStr[i] == 0)
        return;
    
    do
    {
        while(!(inb(COM1 + 5) & 0x20));
        outb(COM1, ptrStr[i]);
        i++;
        
    }while(ptrStr[i]);
    
    while(!(inb(COM1 + 5) & 0x20));
    outb(COM1, 0x0D);

    return;
}

/*---------------------------------------------------------------------------------------------------*/

void SerialPrintDigit64(QWORD val)
{
    for(int i = 15; i >= 0; i--){
        
        BYTE smbl = (val >> i * 4) & 0xF;
        
        if(smbl >= 0xA)
            smbl += 0x57;
        else
            smbl += 0x30;
        
        while(!(inb(COM1 + 5) & 0x20));
        outb(COM1, smbl);
    }
    
    while(!(inb(COM1 + 5) & 0x20));
    outb(COM1, 0x0D);

    return;
}
