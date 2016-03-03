// Sprite.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Sprite.h"
#include <windows.h>
#include "UIlib.h"
#include "SpriteView.h"
#include <shellapi.h>

// TODO: 在此处引用程序需要的其他头文件
using namespace DuiLib;

#pragma comment (lib, "GdiPlus.lib")



class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const
	{
		return _T("DUIMainFrame");
	}
	virtual CDuiString GetSkinFile()
	{
		return _T("duilib.xml");
	}
	virtual CDuiString GetSkinFolder()
	{
		return _T("");
	}
	DUI_DECLARE_MESSAGE_MAP()

	virtual void OnClick(TNotifyUI& msg)
	{
		MessageBox(this->GetHWND(), L"", L"", MB_OK);
	}

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT rv = WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
		// 注册热键
		if(!RegisterHotKey(m_hWnd, 1, MOD_ALT | MOD_NOREPEAT, 0x51))
		{
			MessageBox(m_hWnd, L"热键注册失败", L"失败", MB_OKCANCEL);
		}

		// 创建托盘图标
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		nid.cbSize = sizeof(nid);
		nid.hWnd = m_hWnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_USER;
		nid.hIcon = LoadIcon(pCreate->hInstance, MAKEINTRESOURCE(IDI_SPRITE));
		lstrcpy(nid.szTip, L"多桌面切换v2");
		Shell_NotifyIcon(NIM_ADD, &nid);

		return rv;
	}

	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT rv = WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
		UnregisterHotKey(m_hWnd, 1);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		return rv;
	}

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch (uMsg)
		{
			case WM_USER:
			{
				// 托盘点击事件处理
				if(lParam == WM_RBUTTONDOWN)
				{
					if(MessageBox(m_hWnd, L"是否关闭Spite", L"询问", MB_OKCANCEL) == IDOK)
					{
						// 清理托盘图标
						Shell_NotifyIcon(NIM_DELETE, &nid);
						PostQuitMessage(0);
					}
				}

				break;
			}
			 case  WM_HOTKEY:
			{
				if(wParam == 1)
				{
					if(IsWindowVisible(m_hWnd))
					{
						ShowWindow(false, false);
					}
					else
					{
						ShowWindow();
						SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
				}
				break;
			}

		}

		return 0;
	}

private:
	NOTIFYICONDATA nid;
};


DUI_BEGIN_MESSAGE_MAP(CDuiFrameWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

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
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();

	GdiplusShutdown(token);

	return 0;
}
