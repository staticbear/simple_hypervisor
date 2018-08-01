#pragma once
void InitLongModeGdt(void);
void InitLongModeIdt(QWORD VectorAddr);
void InitLongModeTSS(void);
void InitLongModePages(void);
void InitControlAndSegmenRegs(void);