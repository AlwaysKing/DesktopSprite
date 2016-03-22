#include "stdafx.h"
#include "resource.h"
#include "UIlib.h"
#include "MainFrame.h"
#include "ItemFrame.h"
#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

CItemFrame::CItemFrame(CControlUI* pUI)
{
	pThis = pUI;
}

void CItemFrame::SetInfo(int Type, LPWSTR ShortCutKey, LPWSTR lpszFilePath)
{
	m_strString = lpszFilePath;
	CDuiString strType;

	// 根据类型获取对应的目录
	switch(Type)
	{
		case 1: strType = _T("Exe\0");  break;
		case 2: strType = _T("Directory\0");  break;
		case 3: strType = _T("File\0");  break;
	}

	// 修改界面内容
	CButtonUI* pButton = static_cast<CButtonUI*>( pThis->GetManager()->FindSubControlByName(pThis, _T("Image")) );
	if(pButton)
	{
		CDuiString strImage;

		// 拼接图片目录
		strImage= CPaintManagerUI::GetInstancePath() + L"Icon\\"+ strType+L"\\" + ShortCutKey + L".png";

		pButton->SetBkImage(strImage);
	}

	CTextUI* pText = static_cast<CTextUI*>( pThis->GetManager()->FindSubControlByName(pThis, _T("Text")) );
	if(pText)
	{
		CDuiString strText;
		strText = lpszFilePath;
		strText = strText.Right(strText.GetLength() - strText.ReverseFind(L'\\') - 1);
		pText->SetText(strText);
	}
	
	CTextUI* pShort = static_cast<CTextUI*>( pThis->GetManager()->FindSubControlByName(pThis, _T("Short")) );
	if(pText)
	{
		CDuiString strText(ShortCutKey);
		pShort->SetText(strText);
	}

	CButtonUI* pDelete = static_cast<CButtonUI*>( pThis->GetManager()->FindSubControlByName(pThis, _T("Delete")) );
	if(pDelete){
		CDuiString strName;
		strName.Format(L"%d-%c", Type, ShortCutKey[0]);
		pDelete->SetUserData(strName);
	}
}

void CItemFrame::Open()
{
	ShellExecute(NULL, _T("open"), _T("Explorer.exe"), m_strString, NULL, SW_SHOWDEFAULT);
}

VOID CItemFrame::SetModel(BOOL bDelete)
{
	CButtonUI* pButton = static_cast<CButtonUI*>( pThis->GetManager()->FindSubControlByName(pThis, _T("Delete")) );
	if(pButton)
	{
		pButton->SetVisible(bDelete);
	}
}