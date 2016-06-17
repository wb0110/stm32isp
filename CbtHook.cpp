#include "stdafx.h"
#include "cbthook.h"

static HHOOK hHook;
static WNDPROC BackupBoxProc;

static void CenterDialogBox(HWND hWnd)
{
	RECT rcChild;
	RECT rcParent;
	int  nWidth;
	int  nHeight;
	HWND hOwner;
	POINT pt;
	LONG style;
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);

	hOwner = GetWindow(hWnd,GW_OWNER);
	if(hOwner == NULL)
	{
		return;
	}
	style=GetWindowLong(hOwner,GWL_STYLE);
	if(style & WS_MINIMIZE)
	{
		return;
	}
	GetMonitorInfo(MonitorFromWindow(hOwner, MONITOR_DEFAULTTONEAREST), &mi);
	GetWindowRect(hOwner,&rcParent);
	GetWindowRect(hWnd,&rcChild);
	nWidth = rcChild.right-rcChild.left;
	nHeight= rcChild.bottom-rcChild.top;
	pt.x = rcParent.left + (rcParent.right-rcParent.left- nWidth)/2;
	pt.y = rcParent.top + (rcParent.bottom-rcParent.top - nHeight)/2;
	pt.x = max(0,pt.x);
	pt.y = max(0,pt.y);
	if(pt.x + nWidth >= mi.rcWork.right)
	{
		pt.x =mi.rcWork.right- nWidth;
	}
	if(pt.y + nHeight>= mi.rcWork.bottom)
	{
		pt.y =  mi.rcWork.bottom-nHeight;
	}
	MoveWindow(hWnd,pt.x,pt.y,nWidth,nHeight,TRUE);
}

static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet;
	lRet=CallWindowProc(BackupBoxProc,hWnd,msg,wParam,lParam);
	if(msg==WM_INITDIALOG)
	{
		CenterDialogBox(hWnd);
	}
	return lRet;
}

static LRESULT CALLBACK CbtFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
	LPCREATESTRUCT lpcs;
	if(code == HCBT_CREATEWND)
	{
		lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
		if((DWORD_PTR)lpcs->lpszClass == 0x8002)	//0x8002是对话框类
		{
			BackupBoxProc=(WNDPROC)GetWindowLongPtr((HWND)wParam,GWLP_WNDPROC);
			SetWindowLongPtr((HWND)wParam,GWLP_WNDPROC,(LONG)HookWndProc);
		}
	}
	
	return CallNextHookEx(hHook,code,wParam,lParam);
}

void HookWindowCreate(void)
{
	DWORD dwTID;
	dwTID = GetCurrentThreadId();
	hHook=SetWindowsHookEx(WH_CBT,CbtFilterHook, NULL, dwTID);
}

void UnHookWindowCreate(void)
{
	UnhookWindowsHookEx(hHook);
}
