#pragma once

int ISP_Connect(HANDLE hUART);
int ISP_Launch(DWORD dwAddr);
int ISP_GetVersion(BYTE* pVer);
int ISP_GetPID(WORD* pPid);
int ISP_WriteMemory(DWORD dwAddr, BYTE* pBuff, int len);
int ISP_ReadMemory(DWORD dwAddr, BYTE* pBuff, int len);
int ISP_Erase(void);
int ISP_EraseEx(void);
int ISP_EnableRDP(void);
int ISP_DisableRDP(void);

