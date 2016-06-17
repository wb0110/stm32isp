// ISP.cpp : 定义应用程序的入口点。
#include "stdafx.h"
#include "resource.h"
#include "isp.h"
#include "UART.h"
#include "EnumUART.h"
#include "AboutBox.h"
#include "FileBox.h"
#include "BoxHelper.h"
#include "CbtHook.h"
#include "Update.h"

static HINSTANCE hInst;
static UPDATEPARAM uParam;

static void FillSerialPort(HWND hWnd)
{
	int i;
	int count;
	SendMessage(hWnd,CB_RESETCONTENT,0,0);
	count=EnumUART();
	for(i=0;i<count;i++)
	{
		SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)GetUARTName(i));
	}
	SendMessage(hWnd,CB_SETCURSEL,0,0);
}

static void FillBaudRate(HWND hWnd)
{
	SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)"9600");
	SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)"19200");
	SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)"38400");
	SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)"57600");
	SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)"115200");
	SendMessage(hWnd,CB_SETCURSEL,4,0);
}

static void OnInitDialog(HWND hWnd)
{
	HICON hIcon;
	hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_MAIN));
	SendMessage(hWnd,WM_SETICON,0,(LPARAM)hIcon);
	SetWindowText(hWnd,"STM32 UART ISP");
	FillSerialPort(GetDlgItem(hWnd,IDC_LIST_PORT));
	FillBaudRate(GetDlgItem(hWnd,IDC_LIST_BAUD));
	SetDlgItemText(hWnd,IDC_EDIT_LOADADDR,"08000000");
}

static void OnBnClickedSelect(HWND hWnd)
{
	char szFile[MAX_PATH];
	BOOL bRet;
	bRet=OpenFileBox(hWnd,"选择文件","所有文件",szFile);
	if(bRet)
	{
		SetDlgItemText(hWnd,IDC_EDIT_FILE,szFile);
	}
}

static void OnBnClickedDownload(HWND hWnd)
{
	char szText[MAX_PATH];
	DWORD dword;
	GetDlgItemText(hWnd,IDC_LIST_PORT,szText,MAX_PATH);
	dword = GetDlgItemInt(hWnd,IDC_LIST_BAUD,NULL,0);
	uParam.hUART = OpenUART(szText,dword,8,ONESTOPBIT,EVENPARITY);
	if(uParam.hUART == NULL)
	{
		MessageBox(hWnd,"无法打开串口设备!","烧录失败", MB_OK);
		return;
	}
	GetDlgItemText(hWnd,IDC_EDIT_FILE,szText,MAX_PATH);
	uParam.fp = fopen(szText,"rb");
	if(uParam.fp == NULL)
	{
		CloseUART(uParam.hUART);
		MessageBox(hWnd,"无法打开指定文件!","烧录失败", MB_OK);
		return;
	}
	GetDlgItemText(hWnd,IDC_EDIT_LOADADDR,szText,MAX_PATH);
	sscanf(szText,"%08X", &uParam.dwLoadAddr);
	uParam.bAutoErase = IsDlgButtonChecked(hWnd,IDC_BTN_AUTOERASE);
	uParam.bAutoBoot  = IsDlgButtonChecked(hWnd,IDC_BTN_AUTOBOOT);
	uParam.hWnd = hWnd;
	SendDlgItemMessage(hWnd,IDC_LIST_INFO,LB_RESETCONTENT,0,0);
	if(StartUpdate(&uParam))
	{
		EnableDlgItem(hWnd,IDC_BTN_DOWNLOAD,0);
		EnableDlgItem(hWnd,IDC_LIST_PORT,0);
		EnableDlgItem(hWnd,IDC_LIST_BAUD,0);
		EnableDlgItem(hWnd,IDC_BTN_ENUM,0);
		EnableDlgItem(hWnd,IDC_BTN_AUTOBOOT,0);
		EnableDlgItem(hWnd,IDC_EDIT_BOOTADDR,0);
		EnableDlgItem(hWnd,IDC_EDIT_LOADADDR,0);
	}
	else
	{
		MessageBox(hWnd,"无法启动线程!","烧录失败", MB_OK);
	}
}

static void OnCommand(HWND hWnd, UINT id, UINT code, HWND hItem)
{
	switch(id)
	{
	case IDCANCEL:
		EndDialog(hWnd,0);
		break;
	case IDC_BTN_SELECT:
		OnBnClickedSelect(hWnd);
		break;
	case IDC_BTN_DOWNLOAD:
		OnBnClickedDownload(hWnd);
		break;
	case IDC_BTN_ENUM:
		FillSerialPort(GetDlgItem(hWnd,IDC_LIST_PORT));
		break;
	}
}

static void OnUpdateEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hItem;
	LRESULT lRet;
	hItem = GetDlgItem(hWnd,IDC_LIST_INFO);
	switch(wParam)
	{
	case 0:
		EnableDlgItem(hWnd,IDC_BTN_DOWNLOAD,1);
		EnableDlgItem(hWnd,IDC_LIST_PORT,1);
		EnableDlgItem(hWnd,IDC_LIST_BAUD,1);
		EnableDlgItem(hWnd,IDC_BTN_ENUM,1);
		EnableDlgItem(hWnd,IDC_BTN_AUTOBOOT,1);
		EnableDlgItem(hWnd,IDC_EDIT_BOOTADDR,1);
		EnableDlgItem(hWnd,IDC_EDIT_LOADADDR,1);
		CloseUART(uParam.hUART);
		fclose(uParam.fp);
		break;
	case 1:
		lRet = SendMessage(hItem,LB_ADDSTRING,0,lParam);
		SendMessage(hItem,LB_SETTOPINDEX,lRet,0);
		break;
	case 2:
		SendMessage(hItem,WM_SETREDRAW,0,0);
		lRet = SendMessage(hItem,LB_GETCOUNT,0,0);
		SendMessage(hItem,LB_DELETESTRING,lRet-1,0);
		lRet = SendMessage(hItem,LB_ADDSTRING,0,lParam);
		SendMessage(hItem,WM_SETREDRAW,1,0);
		SendMessage(hItem,LB_SETTOPINDEX,lRet,0);
		break;
	}
}

static INT_PTR WINAPI MainBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		break;
	case WM_COMMAND:
		OnCommand(hWnd,LOWORD(wParam),HIWORD(wParam), (HWND)lParam);
		break;
	case WM_HELP:
		PopupAboutBox(hInst,hWnd);
		break;
	case WM_UPDATE_EVENT:
		OnUpdateEvent(hWnd,wParam,lParam);
		break;
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	HookWindowCreate();
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,MainBoxProc);
	UnHookWindowCreate();
	return 0;
}
