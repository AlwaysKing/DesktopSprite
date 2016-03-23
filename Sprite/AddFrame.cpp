
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
	//�豸������
	int iBits;
	//��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���
	WORD wBitCount;
	//λͼ��ÿ��������ռ�ֽ���
	//�����ɫ���С�� λͼ�������ֽڴ�С ��  λͼ�ļ���С �� д���ļ��ֽ���
	DWORD  dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP  Bitmap;
	//λͼ���Խṹ
	BITMAPFILEHEADER   bmfHdr;
	//λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER   bi;
	//λͼ��Ϣͷ�ṹ
	LPBITMAPINFOHEADER lpbi;
	//ָ��λͼ��Ϣͷ�ṹ
	HANDLE  fh, hDib, hPal;
	HPALETTE  hOldPal = NULL;
	//�����ļ��������ڴ�������ɫ����
	//����λͼ�ļ�ÿ��������ռ�ֽ���
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
	//�����ɫ���С
	if(wBitCount <= 8)
		dwPaletteSize = ( 1 << wBitCount )*sizeof(RGBQUAD);
	//����λͼ��Ϣͷ�ṹ
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

	//Ϊλͼ���ݷ����ڴ�
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��
	hPal = GetStockObject(DEFAULT_PALETTE);
	if(hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	//�ָ���ɫ��   
	if(hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�    
	fh = CreateFile(lpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(fh == INVALID_HANDLE_VALUE)
		return FALSE;

	//����λͼ�ļ�ͷ
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

	// ��ȡͼƬ
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
			// ���
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
		// �������

		CDuiString strType;
		CDuiString strShortCut;
		CDuiString strImage;
		strShortCut = m_pShortCut->GetText();
		CDuiString strFilePath;
		strFilePath = m_pPath->GetText();
		
		// �������ͻ�ȡ��Ӧ��Ŀ¼
		switch(m_Type)
		{
			case 1: strType = _T("Exe\0");  break;
			case 2: strType = _T("Directory\0");  break;
			case 3: strType = _T("File\0");  break;
		}
		// ���ȿ���ͼƬ
		strImage = CPaintManagerUI::GetInstancePath() + L"Icon\\" + strType + L"\\" + strShortCut + L".png";
		if(CopyFile(m_ImagePath, strImage, FALSE)){
			// Ȼ�����
			m_pWnd->AddItem(m_Type, (LPWSTR)strShortCut.GetData(), (LPWSTR)strFilePath.GetData());
			// ȡ��
			this->Close(1);
		}
		else{
			// ���ʧ��
			MessageBox(NULL, L"���ʧ��", L"ʧ��", MB_OK);
		}
	}
	else if(msg.pSender->GetName() == _T("DUI_BTN_CANCEL") || msg.pSender->GetName() == _T("closebtn"))
	{
		// ȡ��
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
		if(strInfo == L"Ŀ¼")
		{
			m_Type = 2;
		}
		if(strInfo == L"�ļ�")
		{
			m_Type = 3;
		}
		ModifyShortCutComo();
	}
}