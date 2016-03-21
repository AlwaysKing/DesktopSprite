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

	// ��ʼ��ʵ������
	if(hinst != 0){
		m_Hinst = hinst;
	}

	// ����ͼƬ
	if(gifpath != NULL){
		// ����GifͼƬ
		m_pImage = new Image(gifpath);
		if(m_pImage != NULL)
		{
			// ��ȡGif��Dimensions����
			m_DimensionsCount = m_pImage->GetFrameDimensionsCount();
			if(m_DimensionsCount != 0)
			{
				m_pDimensionIDs = ( GUID* )new GUID[m_DimensionsCount];
				m_pImage->GetFrameDimensionsList(m_pDimensionIDs, m_DimensionsCount);
			}
		}
	}

	// ���ûص�֪ͨ
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
	// ��ֹͣ
	Stop();

	// ��������
	UnInit();

	m_DrawDimensionIndex = 0;
	m_DrawFrameIndex = 0;
	m_PlayInterval = 500;
}

void SpriteView::SetMoveEnable(BOOL bEnable)
{
	m_bMoveEnbale = bEnable;
}

/**�����Ƿ��ö�*/
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
		// ����ͼƬ
		if(gifpath != NULL)
		{
			if(m_pImageBack)
			{
				delete m_pImageBack;
				m_pImageBack = NULL;
			}

			// ����GifͼƬ
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
		// ����ͼƬ
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

			// ����GifͼƬ
			m_pImage = new Image(gifpath);
			if(m_pImage != NULL)
			{
				// ��ȡGif��Dimensions����
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

/**��ȡGIFͼƬ�ж����ĸ���*/
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

		// ���ԵĻ�������ϴ���������
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
	// ֪ͨ���ڽ���
	SendMessage(m_Hwnd, WM_CLOSE, 0, 0);

	// �ȴ��̷߳���
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
	// ����������
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

	//���ñ�����Ϊ�ֲ㴰��֧��͸��
	//�ֲ㴰��û��WM_PAINT��Ϣ
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
	// ��������ƶ�����Ҫˢ��X,Y
	if(m_bMoving)
	{
		RECT Rect = {0x0};
		GetWindowRect(m_Hwnd, &Rect);
		m_PosX = Rect.left;
		m_PosY = Rect.top;
	}

	//���ش���ͼ��
	int iWidth = m_pImage->GetWidth();
	int iHeight = m_pImage->GetHeight();
	HDC hdcScreen = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, m_Width, m_Height);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
	Graphics gp(hdcMem);

	// ���Ʊ���
	if(m_MouseIn || m_bMoving){
		if(m_pImageBack){
			gp.DrawImage(m_pImageBack, 0, 0, m_Width, m_Height); //��pngͼ����Ƶ���̨DC��
		}
	}

	if (bDraw) {
		//�����ｨ�����ڵ�ͼ��,�����ô���������
		m_pImage->SelectActiveFrame(&m_pDimensionIDs[0], FrameIndex);
	}

	gp.DrawImage(m_pImage, 0, 0, m_Width, m_Height); //��pngͼ����Ƶ���̨DC��

	BLENDFUNCTION blend = {0};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 200;
	blend.AlphaFormat = AC_SRC_ALPHA; //��ͨ�����
	POINT    pPos = {m_PosX, m_PosY};
	POINT    pSrc = {0, 0};
	SIZE       sizeWnd = {m_Width, m_Height};

	UpdateLayeredWindow(hwnd, hdcScreen, &pPos, &sizeWnd, hdcMem, &pSrc, NULL, &blend, ULW_ALPHA); //���·ֲ㴰��

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

	// ������ǵ�һ�β��ŵĵ�һ�ţ������ϲ����ӳ�
	if(PreTime != 0 && m_DrawFrameIndex == 0)
	{
		// ֹͣ������
		if(m_PlayInterval == -1)
		{
			rv = false;
		}
		// ���򳬹����ż���Ų���
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
			// ʵ�ֵ���϶�
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

static HHOOK g_hook;				// ���Ӿ��
static UINT g_CellCount;			// ʵ������

VOID CALLBACK SpriteView::MouseProcSelf(int nCode, WPARAM wparam, LPARAM lparam)
{
	MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lparam;

	if(wparam == WM_MOUSEMOVE)
	{
		// �ж��Ƿ����ڱ�������
		if(m_PosX-1 <= pMouse->pt.x && pMouse->pt.x <= (m_PosX + m_Width + 1))
		{
			if( m_PosY <= pMouse->pt.y && pMouse->pt.y <= (m_PosY + m_Height))
			{
				if(m_MouseIn == FALSE)
				{
					// ����Leave��Ϣ
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
					// ����Leave��Ϣ
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
				// ����Leave��Ϣ
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

// �����Ϣ���Ӻ���
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

		// �Ƚ��Ƿ���ʾ
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
		// Hookȫ�ֹ���
		g_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInstance, 0);
#endif
	}
}

VOID SpriteView::UnInitialFunc(VOID)
{
	g_CellCount--;
	if(g_CellCount == 0)
	{
		// UnHookȫ�ֹ���
		UnhookWindowsHookEx(g_hook);
	}
}