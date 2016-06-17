#include "stdafx.h"
#include "UART.h"

HANDLE OpenUART(LPTSTR pzName, DWORD dwBaudRate, BYTE biDataBits, BYTE biStopBits, BYTE biParity)
{
	HANDLE hUART;
	DCB	dcb;
	COMMTIMEOUTS timeout;
	char szPortName[32];
	sprintf(szPortName,"\\\\.\\%s",pzName);
	hUART = CreateFile(szPortName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if((hUART==INVALID_HANDLE_VALUE)||(hUART==NULL))
	{
		return NULL;
	}
	
	GetCommState(hUART, &dcb);
	dcb.BaudRate = dwBaudRate;
	dcb.Parity   = biParity;
	dcb.ByteSize = biDataBits;
	dcb.StopBits = biStopBits;
	SetCommState(hUART, &dcb);

	GetCommTimeouts(hUART, &timeout);
	timeout.ReadIntervalTimeout = MAXDWORD;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.WriteTotalTimeoutConstant = 0;
	timeout.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hUART,&timeout);
	SetupComm(hUART,1024,1024);
	PurgeComm(hUART,PURGE_RXCLEAR | PURGE_TXCLEAR);
	
	return hUART;
}

void CloseUART(HANDLE hUART)
{
	CloseHandle(hUART);
}

DWORD  ReadUART(HANDLE hUART, void* pData, DWORD dwLen)
{
	BOOL  biRet;
	DWORD dwRead;
	biRet = ReadFile(hUART,pData,dwLen,&dwRead,NULL);
	return biRet?dwRead:0;
}

DWORD  WriteUART(HANDLE hUART, void* pData, DWORD dwLen)
{
	BOOL	biRet;
	DWORD	dwWrite;

	biRet = WriteFile(hUART,pData,dwLen,&dwWrite,NULL);
	return biRet?dwWrite:0;
}
