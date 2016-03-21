// Sprite.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Sprite.h"
#include <windows.h>
#include "SpriteView.h"
#include "MainFrame.h"
#include "ItemFrame.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void OnBnLeft()
{
	keybd_event(VK_LWIN, 0, 0, 0);
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_LEFT, 0, 0, 0);

	keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
}

void OnBnRigth()
{
	keybd_event(VK_LWIN, 0, 0, 0);
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_RIGHT, 0, 0, 0);

	keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
}

void LeftCallBack(SpriteView* pthis, HWND, UINT mess, LPARAM, LPARAM)
{
	if(mess == WM_LBUTTONUP)
	{
		OnBnLeft();
	}
	if(mess == WM_MOUSEIN)
	{
		pthis->SetVisable(TRUE);
	}
	if(mess == WM_MOUSEOUT || mess == WM_RBUTTONUP)
	{
		pthis->SetVisable(FALSE);
	}
}

void RigthCallBack(SpriteView* pthis, HWND, UINT mess, LPARAM, LPARAM)
{
	if(mess == WM_LBUTTONUP)
	{
		OnBnRigth();
	}
	if(mess == WM_MOUSEIN)
	{
		pthis->SetVisable(TRUE);
	}
	if(mess == WM_MOUSEOUT || mess == WM_RBUTTONUP)
	{
		pthis->SetVisable(FALSE);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	ULONG_PTR        token;
	GdiplusStartupInput          gin;
	GdiplusStartup(&token, &gin, NULL);

	// 读取配置文件

	// 创建SpriteView
	SpriteView SpriteViewCell(hInstance, L"Data\\sprite.gif");
	SpriteViewCell.SetImageBack(L"Data\\background.png");
	SpriteViewCell.SetPos(0, 0);
	SpriteViewCell.SetSize(85, 85);
	SpriteViewCell.Start();

	int g_ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int g_ScreenHeigth = GetSystemMetrics(SM_CYFULLSCREEN);

	// 创建左边的精灵
	SpriteView SpriteViewSwitchLeftCell(hInstance, L"Data\\1.png");
	SpriteViewSwitchLeftCell.SetPos(0, 0);
	SpriteViewSwitchLeftCell.SetSize(25, g_ScreenHeigth);
	SpriteViewSwitchLeftCell.SetCallBack(LeftCallBack);
	SpriteViewSwitchLeftCell.SetMoveEnable(FALSE);
	SpriteViewSwitchLeftCell.SetTopMost(TRUE);
	SpriteViewSwitchLeftCell.Start();
	SpriteViewSwitchLeftCell.SetVisable(FALSE);

	// 创建右边的精灵
	SpriteView SpriteViewSwitchRigthCell(hInstance, L"Data\\2.png");
	SpriteViewSwitchRigthCell.SetPos(g_ScreenWidth - 25, 0);
	SpriteViewSwitchRigthCell.SetSize(25, g_ScreenHeigth);
	SpriteViewSwitchRigthCell.SetCallBack(RigthCallBack);
	SpriteViewSwitchRigthCell.SetMoveEnable(FALSE);
	SpriteViewSwitchRigthCell.SetTopMost(TRUE);

	SpriteViewSwitchRigthCell.Start();
	SpriteViewSwitchRigthCell.SetVisable(FALSE);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"\\SysBtn\\");

	CDuiFrameWnd duiFrame;

	duiFrame.Create(NULL, _T("SpriteWmd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();

	GdiplusShutdown(token);

	return 0;
}
