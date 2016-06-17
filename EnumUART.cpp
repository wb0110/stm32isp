/*
* 枚举系统中的串口设备
* 蒋晓岗<kerndev@foxmail.com>
*/
#include "stdafx.h"
#include <WinIoCtl.h>
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")

struct UARTNODE
{
	char szName[16];
	char szDesc[256];
	struct UARTNODE* next;
};

static struct UARTNODE* head;
static struct UARTNODE* tail;

static void AppendUART(char* pzName, char* pzDesc)
{
	struct UARTNODE* node;
	node = (struct UARTNODE*)malloc(sizeof(struct UARTNODE));
	if(node != NULL)
	{
		strcpy(node->szName,pzName);
		strcpy(node->szDesc,pzDesc);
		node->next = NULL;
		
		if(head == NULL)
		{
			head = node;
			tail = node;
		}
		else
		{
			tail->next = node;
			tail = node;
		}
	}
}

static void GetNameByDesc(char* pzName, char* pzDesc)
{
	char* begin;
	char* end;
	char* pch;
	begin=strrchr(pzDesc,'(');
	end  =strrchr(pzDesc,')');
	if((begin != NULL) && (end!=NULL)) 
	{
		for(pch=begin+1;pch!=end;pch++)
		{
			*pzName = *pch;
			pzName++;
		}
	}
	*pzName = 0;
}

char* GetUARTName(int index)
{
	int i;
	struct UARTNODE* node;
	i=0;
	for(node=head;node!=NULL;node=node->next)
	{
		if(i == index)
		{
			return node->szName;
		}
		i++;
	}
	return NULL;
}

char* GetUARTDesc(int index)
{
	int i;
	struct UARTNODE* node;
	i=0;
	for(node=head;node!=NULL;node=node->next)
	{
		if(i == index)
		{
			return node->szDesc;
		}
		i++;
	}
	return NULL;
}

void  FreeUART(void)
{
	struct UARTNODE* node;
	struct UARTNODE* next;
	for(node=head;node!=NULL;node=next)
	{
		next = node->next;
		free(node);
	}
	head = NULL;
	tail = NULL;
}


int EnumUART(void)
{
	int i;
	int count;
	HDEVINFO hDevInfo; 
	SP_DEVINFO_DATA DevInfoData;
	PBYTE Buffer;
	DWORD BufferSize; 
	DWORD RequireSize;
	BOOL  bRet;
	char PortName[32];
	count = 0;

	FreeUART();

	//访问系统的硬件库
	hDevInfo = SetupDiGetClassDevs(&GUID_CLASS_COMPORT, NULL, NULL, DIGCF_DEVICEINTERFACE|DIGCF_PRESENT); 
	if(INVALID_HANDLE_VALUE == hDevInfo) 
	{
		return FALSE; 
	}

	BufferSize = 1024;
	Buffer = (PBYTE)malloc(BufferSize);
	DevInfoData.cbSize=sizeof(SP_DEVINFO_DATA); 

	//枚举硬件，获得需要的接口 
	for(i=0;TRUE;i++) 
	{
		bRet = SetupDiEnumDeviceInfo(hDevInfo, i, &DevInfoData);
		if(!bRet)
		{
			break;
		}

		bRet = SetupDiGetDeviceRegistryProperty(hDevInfo,&DevInfoData,SPDRP_FRIENDLYNAME,NULL,Buffer,BufferSize,&RequireSize);
		if(bRet)
		{
			GetNameByDesc(PortName,(char*)Buffer);
			AppendUART(PortName,(char*)Buffer);
			count++;
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	free(Buffer);
	return count;
}
