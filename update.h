#pragma once

typedef struct 
{
	HWND   hWnd;
	HANDLE hUART;
	FILE*  fp;
	DWORD  dwLoadAddr;
	BOOL   bAutoBoot;
	BOOL   bAutoErase;
}UPDATEPARAM;

BOOL StartUpdate(UPDATEPARAM* uParam);
void StopUpdate(void);


/*
* 窗口消息:更新日志
* WPARAM: 0=线程结束,1=追加信息,2=更新信息
* LPARAM: 信息文本
*/
#define WM_UPDATE_EVENT	(WM_USER+1)
