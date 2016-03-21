#include "stdafx.h"
#include "SpriteView.h"
#include "lock.hpp"
#include <map>
using namespace std;

map<HWND, SpriteView*> g_WndClassMap;
Locker g_Locker;

void SpriteView::Init(HINSTANCE hinst, LPWSTR gifpath, SpriteCallBack pCallBack, int PosX, int PosY, int Width, int Height)
{
	m_bStart = FALSE;
	m_PosX = m_PosY = 0;
	m_Width = m_Height = 450;
	m_DrawDimensionIndex = 0;
	m_DrawFrameIndex = 0;
	m_PlayInterval = 500;
	m_Hwnd = NULL;
	m_Qucik = 1.0;
	m_bMoveEnbale = TRUE;
	m_bMoving = FALSE;

	// 初始话实例对象
	if(hinst != 0){
		m_Hinst = hinst;
	}

	// 加载图片
	if(gifpath != NULL){
		// 加载Gif图片
		m_pImage = new Image(gifpath);
		if(m_pImage != NULL)
		{
			// 获取Gif的Dimensions数量
			m_DimensionsCount = m_pImage->GetFrameDimensionsCount();
			if(m_DimensionsCount != 0)
			{
				m_pDimensionIDs = ( GUID* )new GUID[m_DimensionsCount];
				m_pImage->GetFrameDimensionsList(m_pDimensionIDs, m_DimensionsCount);
			}
		}
	}

	// 设置回调通知
	if(pCallBack != NULL)
	{
		m_CallBack = pCallBack;
	}

	if(PosX != 0)
	{
		m_PosX = PosX;
	}

	if(PosY != 0)
	{
		m_PosY = PosY;
	}

	if(Width != 0)
	{
		m_Width = Width;
	}

	if(Height != 0)
	{
		m_Height = Height;
	}

}

void SpriteView::UnInit()
{
	if(m_pDimensionIDs)
	{
		delete m_pDimensionIDs;
		m_pDimensionIDs = NULL;
	}

	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(m_hJobEvent != NULL)
	{
		CloseHandle(m_hJobEvent);
	}

	if(m_hJobThread != NULL)
	{
		CloseHandle(m_hJobThread);
	}


	if(m_pFrameTime != NULL)
	{
		LocalFree(m_pFrameTime);
		m_pFrameTime = NULL;
	}
}

SpriteView::SpriteView(HINSTANCE hinst)
{ 
	Init(hinst, NULL, NULL, 0, 0, 0, 0);
}

SpriteView::SpriteView(HINSTANCE hinst, LPWSTR gifpath)
{
	Init(hinst, gifpath, NULL, 0, 0, 0, 0);
}

SpriteView::SpriteView(HINSTANCE hinst, LPWSTR gifpath, SpriteCallBack pCallBack)
{
	Init(hinst, gifpath, pCallBack, 0, 0, 0, 0);
}

SpriteView::SpriteView(HINSTANCE hInst, LPWSTR GifPath, SpriteCallBack pCallBack, int PosX, int PosY, int Width, int Height)
{ 
	Init(hInst, GifPath, pCallBack, PosX, PosY, Width, Height);
}

SpriteView::~SpriteView()
{
	// 先停止
	Stop();

	// 清理内容
	UnInit();

	m_DrawDimensionIndex = 0;
	m_DrawFrameIndex = 0;
	m_PlayInterval = 500;
}

void SpriteView::SetMoveEnable(BOOL bEnable)
{
	m_bMoveEnbale = bEnable;
}

/**设置是否置顶*/
void SpriteView::SetTopMost(BOOL bEnable)
{
	m_bTopMost = bEnable;
	if(m_Hwnd != NULL)
	{
		if(bEnable)
		{
			SetWindowPos(m_Hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(m_Hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);			
		}
	}
}

void SpriteView::SetCallBack(SpriteCallBack pCallBack)
{ 
	m_CallBack = pCallBack;
}

BOOL SpriteView::SetImageBack(LPWSTR gifpath)
{
	BOOL rv = FALSE;
	if(!m_bStart)
	{
		// 加载图片
		if(gifpath != NULL)
		{
			if(m_pImageBack)
			{
				delete m_pImageBack;
				m_pImageBack = NULL;
			}

			// 加载Gif图片
			m_pImageBack = new Image(gifpath);
			if(m_pImageBack != NULL)
			{
				rv = TRUE;
			}
		}
		else
		{
			if(m_pImageBack)
			{
				delete m_pImageBack;
				m_pImageBack = NULL;
				rv = TRUE;
			}
		}
	}
	return rv;
}

BOOL SpriteView::SetImage(LPWSTR gifpath)
{
	BOOL rv = FALSE;

	if(!m_bStart)
	{
		// 加载图片
		if(gifpath != NULL)
		{
			if(m_pImage)
			{
				delete m_pImage;
				m_pImage = NULL;
			}

			if(m_pDimensionIDs)
			{
				delete m_pDimensionIDs;
				m_pDimensionIDs = NULL;
			}

			// 加载Gif图片
			m_pImage = new Image(gifpath);
			if(m_pImage != NULL)
			{
				// 获取Gif的Dimensions数量
				m_DimensionsCount = m_pImage->GetFrameDimensionsCount();
				if(m_DimensionsCount != 0)
				{
					m_pDimensionIDs = ( GUID* )new GUID[m_DimensionsCount];
					m_pImage->GetFrameDimensionsList(m_pDimensionIDs, m_DimensionsCount);
				}

				rv = TRUE;
			}
		}
	}
	
	return rv;
}

/**获取GIF图片中动画的个数*/
UINT SpriteView::GetDimensionsCount()
{
	return m_DimensionsCount;
}

BOOL SpriteView::Start()
{ 
	return Start(0);
}

BOOL SpriteView::Start(UINT Index)
{
	if(m_bStart){
		return FALSE;
	}

	if(m_pImage == NULL)
	{
		return FALSE;
	}

	if(Index < m_DimensionsCount)
	{
		m_FrameCount = m_pImage->GetFrameCount(&m_pDimensionIDs[Index]);

		// 可以的话清理掉上次遗留内容
		if(m_pFrameTime != NULL){
			LocalFree(m_pFrameTime);
			m_pFrameTime = NULL;
		}

		UINT size = 0;
		size = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		if(size != 0)
		{
			m_pFrameTime = (PropertyItem*)LocalAlloc(LPTR, size);
			if(m_pFrameTime != NULL)
			{
				if(m_pImage->GetPropertyItem(PropertyTagFrameDelay, size, m_pFrameTime) != 0)
				{
					LocalFree(m_pFrameTime);
					m_pFrameTime = NULL;
					return FALSE;
				}
			}
		}
	}

	return CreateView();
}

void SpriteView::Stop()
{ 
	// 通知窗口结束
	SendMessage(m_Hwnd, WM_CLOSE, 0, 0);

	// 等待线程返回
	WaitForSingleObject(m_hJobThread, INFINITE);
	CloseHandle(m_hJobThread);
	CloseHandle(m_hJobEvent);
	m_hJobThread = NULL;
	m_hJobEvent = NULL;
}

void SpriteView::SetPlayInterval(DWORD PlayInterval)
{ 
	m_PlayInterval = PlayInterval;
}

void SpriteView::SetPos(int x, int y)
{
	SetWndPos(x, y, m_Width, m_Height);
}

void SpriteView::SetSize(int width, int Height)
{ 
	SetWndPos(m_PosX, m_PosY, width, Height);
}

void SpriteView::SetWndPos(int PosX, int PosY, int Width, int Height)
{
	m_PosX = PosX;
	m_PosY = PosY;
	m_Width = Width;
	m_Height = Height;
	if(m_bStart && m_Hwnd)
	{
		MoveWindow(m_Hwnd, m_PosX, m_PosY, m_Width, m_Height, TRUE);
	}
}

void SpriteView::SetQuick(float Qucik)
{
	if(Qucik > 0)
	{
		m_Qucik = Qucik;
	}
}

void SpriteView::SetVisable(BOOL enable)
{
	if(enable)
	{
		ShowWindow(m_Hwnd, SW_SHOW);
		SetTopMost(m_bTopMost);
	}
	else
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}

BOOL SpriteView::CreateView()
{
	// 创建窗口类
	m_hJobEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(m_hJobEvent){
		m_hJobThread = CreateThread(NULL, 0, SpriteView::JobThread, this, 0, NULL);
		if(m_hJobThread){
			WaitForSingleObject(m_hJobEvent, INFINITE);
		}
		else{
			CloseHandle(m_hJobEvent);
		}
	}

	return m_bStart;
}

DWORD WINAPI SpriteView::JobThread(PVOID lparam)
{
	SpriteView* pThis = (SpriteView*)lparam;
	TCHAR                   szAppName[] = TEXT("SwitchDesktop2");
	WNDCLASSEX     wndClass;
	MSG                                        msg;
	ULONG_PTR        token;
	GdiplusStartupInput          gin;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hIconSm = NULL;
	wndClass.hInstance = pThis->m_Hinst;
	wndClass.lpszClassName = TEXT("SwitchDesktop2");
	wndClass.lpszMenuName = NULL;
	wndClass.lpfnWndProc = SpriteView::WndProc;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_IME | CS_DBLCLKS;

	GdiplusStartup(&token, &gin, NULL);
	RegisterClassEx(&wndClass);
	pThis->m_Hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, TEXT("SwitchDesktop2"), szAppName, WS_POPUP | WS_EX_TOPMOST
		, pThis->m_PosX, pThis->m_PosY, pThis->m_Width, pThis->m_Height,
		NULL, NULL, pThis->m_Hinst, NULL);

	g_Locker.Lock();
	g_WndClassMap.insert(make_pair(pThis->m_Hwnd, pThis));
	g_Locker.UnLock();

	//设置本窗口为分层窗口支持透明
	//分层窗口没有WM_PAINT消息
	ShowWindow(pThis->m_Hwnd, SW_SHOW);
	UpdateWindow(pThis->m_Hwnd);

	InitialFunc(pThis->m_Hinst);

	pThis->m_bStart = TRUE;
	SetEvent(pThis->m_hJobEvent);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	pThis->m_bStart = FALSE;

	g_Locker.Lock();
	g_WndClassMap.erase(pThis->m_Hwnd);
	g_Locker.UnLock();
	pThis->m_Hwnd = NULL;

	UnInitialFunc();

	return 0;
}

void  SpriteView::DrawFrame(HWND hwnd, UINT DimensionsIndex, UINT FrameIndex, BOOL bDraw)
{
	// 如果正在移动则需要刷新X,Y
	if(m_bMoving)
	{
		RECT Rect = {0x0};
		GetWindowRect(m_Hwnd, &Rect);
		m_PosX = Rect.left;
		m_PosY = Rect.top;
	}

	//加载窗口图形
	int iWidth = m_pImage->GetWidth();
	int iHeight = m_pImage->GetHeight();
	HDC hdcScreen = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, m_Width, m_Height);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
	Graphics gp(hdcMem);

	// 绘制背景
	if(m_MouseIn || m_bMoving){
		if(m_pImageBack){
			gp.DrawImage(m_pImageBack, 0, 0, m_Width, m_Height); //将png图像绘制到后台DC中
		}
	}

	if (bDraw) {
		//在这里建立窗口的图形,可以用代码来生成
		m_pImage->SelectActiveFrame(&m_pDimensionIDs[0], FrameIndex);
	}

	gp.DrawImage(m_pImage, 0, 0, m_Width, m_Height); //将png图像绘制到后台DC中

	BLENDFUNCTION blend = {0};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 200;
	blend.AlphaFormat = AC_SRC_ALPHA; //按通道混合
	POINT    pPos = {m_PosX, m_PosY};
	POINT    pSrc = {0, 0};
	SIZE       sizeWnd = {m_Width, m_Height};

	UpdateLayeredWindow(hwnd, hdcScreen, &pPos, &sizeWnd, hdcMem, &pSrc, NULL, &blend, ULW_ALPHA); //更新分层窗口

	SelectObject(hdcMem, hBitmapOld);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcScreen);
}

BOOL SpriteView::DrawCtrl(DWORD& PreTime)
{
	BOOL rv = false;
	DWORD NowTime = GetTickCount();
	DWORD TimeGo = NowTime - PreTime;

	// 如果不是第一次播放的第一张，则算上播放延迟
	if(PreTime != 0 && m_DrawFrameIndex == 0)
	{
		// 停止播放了
		if(m_PlayInterval == -1)
		{
			rv = false;
		}
		// 否则超过播放间隔才播放
		else if( TimeGo >= m_PlayInterval)
		{
			rv = true;
			m_DrawFrameIndex++;
			if(m_DrawFrameIndex >= m_FrameCount)
			{
				m_DrawFrameIndex = 0;
			}
			PreTime = GetTickCount();
		}
		else {
			rv = false;
		}
	}
	else if(PreTime == 0)
	{
		m_DrawFrameIndex = 0;
		PreTime = GetTickCount();
		rv = true;
	}
	else
	{
		long lPause = ( (long*)m_pFrameTime->value )[m_DrawFrameIndex] * 10 * m_Qucik;
		if(TimeGo >= lPause)
		{
			m_DrawFrameIndex++;
			if(m_DrawFrameIndex >= m_FrameCount)
			{
				m_DrawFrameIndex = 0;
			}
			rv = true;
			PreTime = GetTickCount();
		}
	}

	return rv;
}

LRESULT CALLBACK SpriteView::WndProcSelf(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;
	RECT        rect;
	static DWORD PreTime = 0; 
	switch(message)
	{
		case WM_TIMER:
				DrawFrame(hwnd, m_DrawDimensionIndex, m_DrawFrameIndex, DrawCtrl(PreTime));

			return 0;
		case WM_MOUSEMOVE:
			if(m_CallBack != NULL){
				m_CallBack(this, hwnd, message, wParam, lParam);
			}
			return 0;
		case WM_LBUTTONDOWN:
			m_bButtonDown = TRUE;
			if(m_CallBack != NULL)
			{
				m_CallBack(this, hwnd, message, wParam, lParam);
			}
			// 实现点击拖动
			if(m_bMoveEnbale){
				m_bMoving = TRUE;
				PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
			break;
		case WM_LBUTTONUP:
			m_bButtonDown = FALSE;
			m_bMoving = FALSE;
			if(m_CallBack != NULL)
			{
				m_CallBack(this, hwnd, message, wParam, lParam);
			}
			return 0;
		case WM_RBUTTONDOWN:
			if(m_CallBack != NULL)
			{
				m_CallBack(this, hwnd, message, wParam, lParam);
			}
			return 0;
		case WM_RBUTTONUP:
			if(m_CallBack != NULL)
			{
				m_CallBack(this, hwnd, message, wParam, lParam);
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK SpriteView::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	map<HWND, SpriteView*>::iterator itClass = g_WndClassMap.end();

	g_Locker.Lock();
	itClass = g_WndClassMap.find(hwnd);
	g_Locker.UnLock();

	switch(message)
	{
		case WM_CREATE:
			SetTimer(hwnd, 1, 10, NULL);
	}

	if(itClass != g_WndClassMap.end())
	{
		return itClass->second->WndProcSelf(hwnd, message, wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static HHOOK g_hook;				// 钩子句柄
static UINT g_CellCount;			// 实例计数

VOID CALLBACK SpriteView::MouseProcSelf(int nCode, WPARAM wparam, LPARAM lparam)
{
	MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lparam;

	if(wparam == WM_MOUSEMOVE)
	{
		// 判断是否是在本窗口内
		if(m_PosX-1 <= pMouse->pt.x && pMouse->pt.x <= (m_PosX + m_Width + 1))
		{
			if( m_PosY <= pMouse->pt.y && pMouse->pt.y <= (m_PosY + m_Height))
			{
				if(m_MouseIn == FALSE)
				{
					// 发送Leave消息
					if(m_CallBack != NULL)
					{
						m_CallBack(this, m_Hwnd, WM_MOUSEIN, 0, 0);
					}

					m_MouseIn = TRUE;
				}
			}
			else
			{
				if(m_MouseIn)
				{
					// 发送Leave消息
					if(m_CallBack != NULL)
					{
						m_CallBack(this, m_Hwnd, WM_MOUSEOUT, 0, 0);
					}
					m_MouseIn = FALSE;
				}
			}
		}
		else
		{
			if(m_MouseIn)
			{
				// 发送Leave消息
				if(m_CallBack != NULL)
				{
					m_CallBack(this, m_Hwnd, WM_MOUSEOUT, 0, 0);
				}
				m_MouseIn = FALSE;
			}
		}
	}

	if(wparam == WM_LBUTTONUP && m_bButtonDown == TRUE && m_bMoveEnbale == TRUE)
	{
		m_bMoveEnbale = m_bMoveEnbale;
		m_bMoving = FALSE;
		if(m_PosX <= pMouse->pt.x && pMouse->pt.x <= ( m_PosX + m_Width ))
		{
			if(m_PosY <= pMouse->pt.y && pMouse->pt.y <= ( m_PosY + m_Height ))
			{

				if(m_CallBack != NULL)
				{
					m_CallBack(this, m_Hwnd, wparam, 0, 0);
				}
			}
		}
	}
}

// 鼠标消息钩子函数
LRESULT CALLBACK SpriteView::MouseProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	if(nCode >= 0)
	{
		MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lparam;
		int g_ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
		int g_ScreenHeigth = GetSystemMetrics(SM_CYFULLSCREEN);

		if(pMouse->pt.x < 0)
		{
			pMouse->pt.x = 0;
		}
		if(pMouse->pt.x > g_ScreenWidth)
		{
			pMouse->pt.x = g_ScreenWidth;
		}

		if(pMouse->pt.y < 0)
		{
			pMouse->pt.y = 0;
		}

		if(pMouse->pt.x > g_ScreenHeigth)
		{
			pMouse->pt.y = g_ScreenHeigth;
		}

		// 比较是否显示
		map<HWND, SpriteView*>::iterator it = g_WndClassMap.begin();
		for(; it != g_WndClassMap.end(); it++)
		{
			it->second->MouseProcSelf(nCode, wparam, lparam);
		}
	}

	return CallNextHookEx(g_hook, nCode, wparam, lparam);
}
LRESULT CALLBACK MouseProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	return CallNextHookEx(g_hook, nCode, wparam, lparam);
}

VOID SpriteView::InitialFunc(HINSTANCE hInstance)
{
	g_CellCount++;
	
	if(g_hook == NULL)
	{
#ifndef DEBUG
		// Hook全局钩子
		g_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInstance, 0);
#endif
	}
}

VOID SpriteView::UnInitialFunc(VOID)
{
	g_CellCount--;
	if(g_CellCount == 0)
	{
		// UnHook全局钩子
		UnhookWindowsHookEx(g_hook);
	}
}