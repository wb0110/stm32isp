#pragma once

/*
* 打开一个串口,并设置参数
* dwBaudRate:	1200-921600
* biDataBits:	5,6,7,8
* biStopBits:	ONESTOPBIT          0
				ONE5STOPBITS        1 
				TWOSTOPBITS         2
* biParity:		NOPARITY            0
				ODDPARITY           1
				EVENPARITY          2
				MARKPARITY          3
				SPACEPARITY         4
*/
HANDLE OpenUART(LPTSTR pzName, DWORD dwBaudRate, BYTE biDataBits, BYTE biStopBits, BYTE biParity);
void   CloseUART(HANDLE hUART);
DWORD  ReadUART(HANDLE hUART, void* pData, DWORD dwLen);
DWORD  WriteUART(HANDLE hUART, void* pData, DWORD dwLen);
