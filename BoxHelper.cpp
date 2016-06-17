#include "stdafx.h"
#include "BoxHelper.h"

void EnableDlgItem(HWND hWnd, int nItem,BOOL bEnable)
{
	HWND hItem;
	hItem = GetDlgItem(hWnd,nItem);
	EnableWindow(hItem,bEnable);
}

void MoveDlgItem(HWND hWnd,int nItem, int dx, int dy, UINT nFlag)
{
	RECT rc;
	HWND hItem;
	POINT pt;

	hItem = GetDlgItem(hWnd,nItem);
	GetWindowRect(hItem,&rc);
	if(nFlag & MF_OFFSETHOR)
	{
		rc.left+=dx;
		rc.right+=dx;
	}
	if(nFlag & MF_OFFSETVER)
	{
		rc.top+=dy;
		rc.bottom+=dy;
	}
	if(nFlag & MF_RESIZEWIDTH)
	{
		rc.right += dx;
	}
	if(nFlag & MF_RESIZEHEIGHT)
	{
		rc.bottom += dy;
	}
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(hWnd,&pt);
	MoveWindow(hItem, pt.x, pt.y, rc.right-rc.left, rc.bottom-rc.top, TRUE);
}
