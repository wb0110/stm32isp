/*
* 枚举系统中的串口设备
* 蒋晓岗<kerndev@foxmail.com>
*/
#include "stdafx.h"
#include "EnumUART.h"
#include <WinIoCtl.h>
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")

struct UARTNODE
{
	char szPortName[16];
	char szDeviceName[MAX_PATH];
	struct UARTNODE* next;
};

static struct UARTNODE* head;
static struct UARTNODE* tail;


static void GetPortNameByFriendlyName(char* pzPortName, char* pzFriendlyName)
{
	char* begin;
	char* end;
	char* pch;
	begin=strrchr(pzFriendlyName,'(');
	end  =strrchr(pzFriendlyName,')');
	if((begin != NULL) && (end!=NULL)) 
	{
		for(pch=begin+1;pch!=end;pch++)
		{
			*pzPortName = *pch;
			pzPortName++;
		}
	}
	*pzPortName = 0;
}


static void AppendUART(char* pzFriendlyName)
{
	struct UARTNODE* node;
	node = (struct UARTNODE*)malloc(sizeof(struct UARTNODE));
	if(node != NULL)
	{
		strcpy(node->szDeviceName,pzFriendlyName);
		GetPortNameByFriendlyName(node->szPortName,pzFriendlyName);
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
		node->next = NULL;
	}
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
	BOOL  ret;

	FreeUART();

	hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT); 
	if(INVALID_HANDLE_VALUE == hDevInfo) 
	{
		return FALSE; 
	}
	
	count = 0;
	BufferSize = 1024;
	Buffer = (PBYTE)malloc(BufferSize);
	DevInfoData.cbSize=sizeof(SP_DEVINFO_DATA); 

	for(i=0;TRUE;i++) 
	{
		ret = SetupDiEnumDeviceInfo(hDevInfo, i, &DevInfoData);
		if(!ret)
		{
			break;
		}
		ret = SetupDiGetDeviceRegistryProperty(hDevInfo,&DevInfoData,SPDRP_CLASS,NULL,Buffer,BufferSize,&RequireSize);
		if(!ret)
		{
			continue;
		}
		if(memcmp(Buffer,"Ports",5)!=0)
		{
			continue;
		}
		ret = SetupDiGetDeviceRegistryProperty(hDevInfo,&DevInfoData,SPDRP_FRIENDLYNAME,NULL,Buffer,BufferSize,&RequireSize);
		if(!ret)
		{
			continue;
		}
		AppendUART((char*)Buffer);
		count++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	free(Buffer);
	return count;
}


char* GetPortName(int index)
{
	int i;
	struct UARTNODE* node;
	i=0;
	for(node=head;node!=NULL;node=node->next)
	{
		if(i == index)
		{
			return node->szPortName;
		}
		i++;
	}
	return NULL;
}

char* GetDeviceName(int index)
{
	int i;
	struct UARTNODE* node;
	i=0;
	for(node=head;node!=NULL;node=node->next)
	{
		if(i == index)
		{
			return node->szDeviceName;
		}
		i++;
	}
	return NULL;
}