#ifndef __ENUM_UART_H
#define __ENUM_UART_H

int   EnumUART(void);
char* GetPortName(int index);
char* GetDeviceName(int index);
void  FreeUART(void);

#endif
