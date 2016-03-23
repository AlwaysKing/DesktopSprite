// Sprite.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Sprite.h"
#include <windows.h>
#include "SpriteView.h"
#include "MainFrame.h"
#include "ItemFrame.h"
#include "json.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Json::Value g_Global;
Json::Value g_Sprite;
Json::Value g_Switch;
Json::Value g_Quick;

SpriteView* pSpriteViewCell = NULL;
SpriteView* pSpriteViewSwitchLeftCell = NULL;
SpriteView* pSpriteViewSwitchRigthCell = NULL;
BOOL g_use_switch = FALSE;

BOOL LoadDate()
{
	BOOL rv = FALSE;
	PBYTE pBuff = NULL;
	ULONG ulFileSize = 0;

	CDuiString strDataFile = CPaintManagerUI::GetInstancePath() + L"Data\\ini.data";
	HANDLE m_hFile = CreateFile(strDataFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		goto end;
	}

	ulFileSize = GetFileSize(m_hFile, NULL);

	// 申请内存
	pBuff = (PBYTE)LocalAlloc(LPTR, ulFileSize);
	if(pBuff == NULL)
	{
		goto end;
	}

	DWORD dwToread = 0;
	if(ReadFile(m_hFile, pBuff, ulFileSize, &dwToread, NULL) == false)
	{
		goto end;
	}

	if(dwToread != ulFileSize)
	{
		goto end;
	}
	{
		// 解析内容
		Json::Reader Reader;
		Json::Value root;
		std::string buff((PCHAR)pBuff);

		if(!Reader.parse(buff, root))
		{
			goto end;
		}


		g_Global = root["Gloabl"];
		g_Sprite = root["Sprite"];
		g_Switch = root["Switch"];
		g_Quick = root["Quick"];
	}
end:
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}

	if(pBuff)
	{
		LocalFree(pBuff);
	}

	return rv;
}

BOOL SaveData()
{
	CString strFilePath;
	strFilePath = CPaintManagerUI::GetInstancePath() + L"Data\\ini.data";

	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	Json::Value root;

	root["Gloabl"] = g_Global;
	root["Sprite"] = g_Sprite;
	root["Switch"] = g_Switch;
	root["Quick"] = g_Quick;

	Json::StyledWriter writer;
	string buff = writer.write(root).c_str();

	DWORD dwFileSize = buff.size();
	DWORD dwToWrite;
	if(!WriteFile(hFile, buff.c_str(), dwFileSize, &dwToWrite, NULL) || dwFileSize != dwToWrite)
	{
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return TRUE;
}

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

void LeftCallBack(SpriteView* pthis, HWND, UINT mess, WPARAM wparam, LPARAM lparam)
{
	if(mess == WM_LBUTTONUP && g_use_switch)
	{
		OnBnLeft();
	}
	if(mess == WM_MOUSEIN && g_use_switch)
	{
		pthis->SetVisable(TRUE);
	}
	if(mess == WM_MOUSEOUT || mess == WM_RBUTTONUP)
	{
		pthis->SetVisable(FALSE);
	}
}

void RigthCallBack(SpriteView* pthis, HWND, UINT mess, WPARAM wparam, LPARAM lparam)
{
	if(mess == WM_LBUTTONUP && g_use_switch)
	{
		OnBnRigth();
	}
	if(mess == WM_MOUSEIN && g_use_switch)
	{
		pthis->SetVisable(TRUE);
	}
	if(mess == WM_MOUSEOUT || mess == WM_RBUTTONUP)
	{
		pthis->SetVisable(FALSE);
	}
}


void CmdCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, HWND hwnd)
{

	switch(uMsg)
	{
		case WM_COMMAND:
		{
			switch(wParam)
			{
				case 2:
					if(pSpriteViewCell)
					{
						pSpriteViewCell->SetVisable(!IsWindowVisible(pSpriteViewCell->m_Hwnd));
					}
					break;

				case 3:
					g_use_switch = !g_use_switch;

					break;
			}
		}break;
		case WM_USER:
		{
			// 托盘点击事件处理
			if(lParam == WM_RBUTTONDOWN)
			{
				// 显示菜单
				POINT pt;
				GetCursorPos(&pt);
				HMENU hMenu;
				hMenu = CreatePopupMenu();

				AppendMenu(hMenu, MF_STRING, 2, L"小精灵");
				if(IsWindowVisible(pSpriteViewCell->m_Hwnd)){
					CheckMenuItem(hMenu, 2, MF_CHECKED);
				}

				AppendMenu(hMenu, MF_STRING, 3, L"桌面切换");
				if(g_use_switch){
					CheckMenuItem(hMenu, 3, MF_CHECKED);
				}

				AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(hMenu, MF_STRING, 0, L"退出");
				::SetForegroundWindow(hwnd);
				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hwnd, NULL);
			}
			break;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	ULONG_PTR        token;
	GdiplusStartupInput          gin;
	GdiplusStartup(&token, &gin, NULL);
	int g_ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int g_ScreenHeigth = GetSystemMetrics(SM_CYFULLSCREEN);

	CPaintManagerUI::SetInstance(hInstance);

	// 读取配置文件
	LoadDate();


	// 创建SpriteView
	SpriteView SpriteViewCell(hInstance, L"Data\\sprite.gif");
	SpriteViewCell.SetImageBack(L"Data\\background.png");
	SpriteViewCell.SetPos(g_Sprite["x"].asInt(), g_Sprite["y"].asInt());
	SpriteViewCell.SetSize(85, 85);
	SpriteViewCell.Start();
	SpriteViewCell.SetVisable(g_Sprite["enable"].asBool());
	pSpriteViewCell = &SpriteViewCell;

	// 创建左边的精灵
	SpriteView SpriteViewSwitchLeftCell(hInstance, L"Data\\1.png");
	SpriteViewSwitchLeftCell.SetPos(0, 0);
	SpriteViewSwitchLeftCell.SetSize(8, g_ScreenHeigth);
	SpriteViewSwitchLeftCell.SetCallBack(LeftCallBack);
	SpriteViewSwitchLeftCell.SetMoveEnable(FALSE);
	SpriteViewSwitchLeftCell.SetTopMost(TRUE);
	SpriteViewSwitchLeftCell.Start();
	SpriteViewSwitchLeftCell.SetVisable(false);
	pSpriteViewSwitchLeftCell = &SpriteViewSwitchLeftCell;

	// 创建右边的精灵
	SpriteView SpriteViewSwitchRigthCell(hInstance, L"Data\\2.png");
	SpriteViewSwitchRigthCell.SetPos(g_ScreenWidth - 8, 0);
	SpriteViewSwitchRigthCell.SetSize(8, g_ScreenHeigth);
	SpriteViewSwitchRigthCell.SetCallBack(RigthCallBack);
	SpriteViewSwitchRigthCell.SetMoveEnable(FALSE);
	SpriteViewSwitchRigthCell.SetTopMost(TRUE);
	SpriteViewSwitchRigthCell.Start();
	SpriteViewSwitchRigthCell.SetVisable(false);
	pSpriteViewSwitchRigthCell = &SpriteViewSwitchRigthCell;
	g_use_switch = g_Switch["enable"].asBool();


	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"\\SysBtn\\");
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("SpriteWmd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES);
	duiFrame.CenterWindow();
	duiFrame.SetCmdCallback(CmdCallBack);
	duiFrame.ShowModal();

	GdiplusShutdown(token);

	g_Sprite["x"] = SpriteViewCell.m_PosX;
	g_Sprite["y"] = SpriteViewCell.m_PosY;
	g_Sprite["enable"] = IsWindowVisible(SpriteViewCell.m_Hwnd);
	g_Switch["enable"] = g_use_switch;

	SaveData();
	return 0;
}