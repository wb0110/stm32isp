#ifndef __ENUM_UART_H
#define __ENUM_UART_H

int   EnumUART(void);
char* GetUARTName(int index);
char* GetUARTDesc(int index);
void  FreeUART(void);

#endif
