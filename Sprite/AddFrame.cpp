
#include "stdafx.h"
#include "AddFrame.h"
#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

HBITMAP ConvertIconToBitmap(HICON hIcon)
{
	ICONINFO csII;
	if(!::GetIconInfo(hIcon, &csII))
		return NULL;
	DWORD dwWidth = csII.xHotspot * 2;
	DWORD dwHeight = csII.yHotspot * 2;
	HDC hDc = ::GetDC(NULL);
	HDC hMemDc = ::CreateCompatibleDC(hDc);
	HBITMAP hBitMap = ::CreateCompatibleBitmap(hDc, dwWidth, dwHeight);
	if(!hBitMap) return NULL;
	::SelectObject(hMemDc, hBitMap);
	::DrawIconEx(hMemDc, 0, 0, hIcon, dwWidth, dwHeight, 0, NULL, DI_NORMAL);
	::DeleteDC(hMemDc);
	::ReleaseDC(NULL, hDc);
	::DestroyIcon(hIcon);
	return hBitMap;
}

bool SaveBitmapToFile(HBITMAP hBitmap, CString lpFilePath)
{
	HDC hDC;
	//设备描述表
	int iBits;
	//当前显示分辨率下每个像素所占字节数
	WORD wBitCount;
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD  dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP  Bitmap;
	//位图属性结构
	BITMAPFILEHEADER   bmfHdr;
	//位图文件头结构
	BITMAPINFOHEADER   bi;
	//位图信息头结构
	LPBITMAPINFOHEADER lpbi;
	//指向位图信息头结构
	HANDLE  fh, hDib, hPal;
	HPALETTE  hOldPal = NULL;
	//定义文件，分配内存句柄，调色板句柄
	//计算位图文件每个像素所占字节数
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if(iBits <= 1)
		wBitCount = 1;
	else if(iBits <= 4)
		wBitCount = 4;
	else if(iBits <= 8)
		wBitCount = 8;
	else if(iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小
	if(wBitCount <= 8)
		dwPaletteSize = ( 1 << wBitCount )*sizeof(RGBQUAD);
	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ( ( Bitmap.bmWidth*wBitCount + 31 ) / 32 ) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	if(hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	//恢复调色板   
	if(hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件    
	fh = CreateFile(lpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(fh == INVALID_HANDLE_VALUE)
		return FALSE;

	//设置位图文件头
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = ( DWORD )sizeof(BITMAPFILEHEADER) + ( DWORD )sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize, &dwWritten, NULL);

	GlobalUnlock(hDib);
	GlobalFree(hDib);

	CloseHandle(fh);
	return true;
}

DUI_BEGIN_MESSAGE_MAP(CDuiAddFrame, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_END_MESSAGE_MAP()

CDuiAddFrame::CDuiAddFrame()
{
	if(m_pImageList == NULL)
	{
		HRESULT hResult = SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&m_pImageList);
		if(hResult != S_OK){
			m_pImageList = NULL;
		}
	}
	m_ImagePath = CPaintManagerUI::GetInstancePath() + L"Icon\\tmp.png";
}

LPCTSTR CDuiAddFrame::GetWindowClassName() const
{
	return _T("DUIAddFrame");
}

CDuiString CDuiAddFrame::GetSkinFile()
{
	return _T("AddFrame.xml");
}

CDuiString CDuiAddFrame::GetSkinFolder()
{
	return _T("");
}

void CDuiAddFrame::SetInfo(int Type, LPWSTR lpFilePath, CDuiFrameWnd* pWnd)
{
	m_pImage = static_cast<CButtonUI*>( m_PaintManager.FindControl(_T("DUI_BTN_IMAGE")) );
	m_pPath = static_cast<CTextUI*>( m_PaintManager.FindControl(_T("DUI_TXT_PATH")) );
	m_pType = static_cast<CComboUI*>( m_PaintManager.FindControl(_T("DUI_COMB_TYPE")) );
	m_pShortCut = static_cast<CComboUI*>( m_PaintManager.FindControl(_T("DUI_TXT_SHORTCUT")));

	if(m_pPath){
		m_pPath->SetText(lpFilePath);
	}

	m_Type = Type;
	m_pWnd = pWnd;


	m_pType->SelectItem(m_Type - 1);

	ModifyShortCutComo();

	HBITMAP hBitmap;

	// 获取图片
	if(m_pImageList != NULL){
		SHFILEINFOW sfi = {0};
		SHGetFileInfo(lpFilePath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

		HICON hIcon;
		HRESULT hResult = ((IImageList*)m_pImageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
		if(hResult == S_OK){
			hBitmap = ConvertIconToBitmap(hIcon);
			if(SaveBitmapToFile(hBitmap, m_ImagePath.GetData())){
				m_pImage->SetBkImage(m_ImagePath);
			}
		}
	}
}

BOOL CDuiAddFrame::ModifyShortCutComo()
{
	if(!m_pShortCut){
		return false;
	}

	for(int i = 0; i < 26; i++)
	{
		if(!m_pWnd->TestShortCut(m_Type, i))
		{
			// 添加
			CListLabelElementUI* pItem = new CListLabelElementUI;
			CDuiString strInfo;
			strInfo.Format(L"%c", i + 'A');
			pItem->SetText(strInfo);
			m_pShortCut->Add(pItem);
		}
	}

	m_pShortCut->SelectItem(0);
	return TRUE;
}

void CDuiAddFrame::OnClick(TNotifyUI& msg)
{
	if(msg.pSender->GetName() == _T("DUI_BTN_OK"))
	{
		// 尝试添加

		CDuiString strType;
		CDuiString strShortCut;
		CDuiString strImage;
		strShortCut = m_pShortCut->GetText();
		CDuiString strFilePath;
		strFilePath = m_pPath->GetText();
		
		// 根据类型获取对应的目录
		switch(m_Type)
		{
			case 1: strType = _T("Exe\0");  break;
			case 2: strType = _T("Directory\0");  break;
			case 3: strType = _T("File\0");  break;
		}
		// 首先拷贝图片
		strImage = CPaintManagerUI::GetInstancePath() + L"Icon\\" + strType + L"\\" + strShortCut + L".png";
		if(CopyFile(m_ImagePath, strImage, FALSE)){
			// 然后添加
			m_pWnd->AddItem(m_Type, (LPWSTR)strShortCut.GetData(), (LPWSTR)strFilePath.GetData());
			// 取消
			this->Close(1);
		}
		else{
			// 添加失败
			MessageBox(NULL, L"添加失败", L"失败", MB_OK);
		}
	}
	else if(msg.pSender->GetName() == _T("DUI_BTN_CANCEL") || msg.pSender->GetName() == _T("closebtn"))
	{
		// 取消
		this->Close(0);
	}
	else if(msg.pSender->GetName() == _T("DUI_BTN_IMAGE"))
	{

	}
}

void CDuiAddFrame::OnItemSelect(TNotifyUI& msg)
{
	if(msg.pSender->GetName() == _T("DUI_COMB_TYPE"))
	{
		CDuiString strInfo = m_pType->GetText();
		if(strInfo == L"EXE")
		{
			m_Type = 1;
		}
		if(strInfo == L"目录")
		{
			m_Type = 2;
		}
		if(strInfo == L"文件")
		{
			m_Type = 3;
		}
		ModifyShortCutComo();
	}
}