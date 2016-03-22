#pragma once
#include "UIlib.h"
#include "MainFrame.h"

using namespace DuiLib;

class CDuiAddFrame : public WindowImplBase
{
public:
	CDuiAddFrame();
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	DUI_DECLARE_MESSAGE_MAP()

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnItemSelect(TNotifyUI& msg);

public:
	int m_Type;
	CDuiFrameWnd* m_pWnd;
	BOOL ModifyShortCutComo();
	void SetInfo(int Type, LPWSTR lpFilePath, CDuiFrameWnd* pWnd);
	CButtonUI* m_pImage;
	CTextUI* m_pPath;
	CComboUI* m_pType;
	CComboUI* m_pShortCut;
	HIMAGELIST* m_pImageList;
	CDuiString m_ImagePath;
};
