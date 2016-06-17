#ifndef __BOXHELPER_H
#define __BOXHELPER_H


#define MF_RESIZEWIDTH	0x01	//改变宽度
#define MF_RESIZEHEIGHT	0x02	//改变高度
#define MF_OFFSETHOR	0x04	//水平移动
#define MF_OFFSETVER	0x08	//垂直移动
void MoveDlgItem(HWND hWnd,int nItem, int dx, int dy, UINT nFlag);
void EnableDlgItem(HWND hWnd, int nItem,BOOL bEnable);

#endif
