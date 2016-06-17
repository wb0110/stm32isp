#include <stdafx.h>
#include <CommDlg.h>
#include "FileBox.h"

BOOL OpenFileBox(HWND hWnd, char* pzCaption, char* pzFilter, char* pzFileName)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hInstance         = NULL;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = NULL;
	ofn.nMaxFile          = MAX_PATH;
	ofn.Flags             = 0;

	ofn.hwndOwner         = hWnd;
	ofn.lpstrTitle        = pzCaption;
	ofn.lpstrFilter       = pzFilter;
	ofn.lpstrFile         = pzFileName;
	ofn.Flags             = OFN_FILEMUSTEXIST;

	ofn.lpstrFile[0] = 0;
	return GetOpenFileName (&ofn);
}

BOOL SaveFileBox(HWND hWnd, char* pzCaption, char* pzFilter, char* pzFileName)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hInstance         = NULL;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = NULL;          // Set in Open and Close functions
	ofn.nMaxFile          = MAX_PATH;
	ofn.Flags             = 0;             // Set in Open and Close functions

	ofn.hwndOwner         = hWnd;
	ofn.lpstrTitle        = pzCaption;
	ofn.lpstrFilter       = pzFilter;
	ofn.lpstrFile         = pzFileName;
	ofn.Flags             = OFN_OVERWRITEPROMPT;

	ofn.lpstrFile[0] = 0;
	return GetSaveFileName (&ofn);
}
