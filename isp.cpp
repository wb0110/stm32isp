#include "stdafx.h"
#include "UART.h"
#include "isp.h"

#define SYNC		0x7F
#define ACK			0x79
#define NACK		0x1F

#define CMD_GETVER	0x01
#define CMD_GETPID	0x02
#define CMD_READ	0x11
#define CMD_LAUNCH	0x21
#define CMD_WRITE	0x31
#define CMD_ERASE	0x43
#define CMD_ERASEEX	0x44
#define CMD_ENRDP	0x82
#define CMD_UNRDP	0x92

static HANDLE hDestUART;

static void ISP_SendData(BYTE* pData, int len)
{
	WriteUART(hDestUART,pData,len);
}

static BOOL ISP_RecvData(BYTE* pData, int len, DWORD dwTimeout)
{
	int i;
	BYTE biRead;
	DWORD dwBeginTime;
	dwBeginTime = GetTickCount();
	for(i=0;i<len;)
	{
		if(ReadUART(hDestUART,&biRead,1)==1)
		{
			pData[i++] = biRead;
		}
		else if((GetTickCount()-dwBeginTime)>dwTimeout)
		{
			return 0;
		}
	}
	return 1;
}

static BYTE ISP_CalcXOR(BYTE* pData, int len)
{
	int i;
	BYTE biXOR;
	biXOR = 0;
	for(i=0;i<len;i++)
	{
		biXOR ^= pData[i];
	}
	return biXOR;
}

int ISP_EnableRDP(void)
{
	BYTE biResp;
	BYTE biPacket[4];
	biPacket[0] = CMD_ENRDP;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	if(ISP_RecvData(&biResp, 1, 5000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	return 1;
}

int ISP_DisableRDP(void)
{
	BYTE biResp;
	BYTE biPacket[4];
	biPacket[0] = CMD_UNRDP;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	if(ISP_RecvData(&biResp, 1, 60000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	return 1;
}

int ISP_Erase(void)
{
	BYTE biResp;
	BYTE biPacket[8];
	biPacket[0] = CMD_ERASE;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = 0xFF;
	biPacket[1] = 0x00;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 60000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	return 1;
}

int ISP_EraseEx(void)
{
	BYTE biResp;
	BYTE biPacket[8];
	biPacket[0] = CMD_ERASEEX;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = 0xFF;
	biPacket[1] = 0xFF;
	biPacket[2] = 0x00;
	ISP_SendData(biPacket,3);
	if(ISP_RecvData(&biResp, 1, 60000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	return 1;
}

int ISP_ReadMemory(DWORD dwAddr, BYTE* pBuff, int len)
{
	BYTE biResp;
	BYTE biPacket[260];
	biPacket[0] = CMD_READ;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = dwAddr>>24;
	biPacket[1] = dwAddr>>16;
	biPacket[2] = dwAddr>>8;
	biPacket[3] = dwAddr;
	biPacket[4] = ISP_CalcXOR(biPacket,4);
	ISP_SendData(biPacket,5);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = len-1;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	if(ISP_RecvData(pBuff,len,5000)==0)
	{
		return -1;
	}
	return 1;
}

int ISP_WriteMemory(DWORD dwAddr, BYTE* pBuff, int len)
{
	BYTE biResp;
	BYTE biPacket[260];
	biPacket[0] = CMD_WRITE;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = dwAddr>>24;
	biPacket[1] = dwAddr>>16;
	biPacket[2] = dwAddr>>8;
	biPacket[3] = dwAddr;
	biPacket[4] = ISP_CalcXOR(biPacket,4);
	ISP_SendData(biPacket,5);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = len-1;
	CopyMemory(biPacket+1,pBuff,len);
	biPacket[len+1] = ISP_CalcXOR(biPacket,len+1);
	ISP_SendData(biPacket,len+2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	return (biResp==ACK);
}

int ISP_GetPID(WORD* pPid)
{
	BYTE biResp;
	BYTE biPacket[8];
	biPacket[0] = CMD_GETPID;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	if(ISP_RecvData(biPacket,4,1000)==0)
	{
		return -1;
	}
	*pPid = (biPacket[1]<<8)|biPacket[2];
	return 1;
}

int ISP_GetVersion(BYTE* pVer)
{
	BYTE biResp;
	BYTE biPacket[8];
	biPacket[0] = CMD_GETVER;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	if(ISP_RecvData(biPacket,4,1000)==0)
	{
		return -1;
	}
	*pVer = biPacket[0];
	return 1;
}

int ISP_Launch(DWORD dwAddr)
{
	BYTE biResp;
	BYTE biPacket[8];
	biPacket[0] = CMD_LAUNCH;
	biPacket[1] = biPacket[0] ^ 0xFF;
	ISP_SendData(biPacket,2);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	if(biResp != ACK)
	{
		return 0;
	}
	biPacket[0] = dwAddr>>24;
	biPacket[1] = dwAddr>>16;
	biPacket[2] = dwAddr>>8;
	biPacket[3] = dwAddr;
	biPacket[4] = ISP_CalcXOR(biPacket,4);
	ISP_SendData(biPacket,5);
	if(ISP_RecvData(&biResp, 1, 1000)==0)
	{
		return -1;
	}
	return (biResp==ACK);
}

int ISP_Connect(HANDLE hUART)
{
	BYTE biLead;
	BYTE biRead;
	hDestUART = hUART;
	biLead = SYNC;
	while(1)
	{
		ISP_SendData(&biLead,1);
		if(ISP_RecvData(&biRead, 1,500)!=0)
		{
			break;
		}
	}
	return ((biRead==ACK)||(biRead==NACK));
}
