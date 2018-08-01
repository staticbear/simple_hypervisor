
// http://wiki.osdev.org/Serial_Ports
#pragma once
#include "types.h"

#define COM1 0x3f8
void SerialPrintStr64(BYTE *ptrStr);
void SerialPrintDigit64(QWORD val);
