#pragma once
#include "UIlib.h"
#include <map>
#include <shellapi.h>
#include "ItemFrame.h"

using namespace std;
using namespace DuiLib;

typedef void(*pCmdCallBack)(UINT uMsg, WPARAM wParam, LPARAM lParam, HWND hwnd);

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnTextChange(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);

	BOOL TestShortCut(int i, int s);

	VOID SetCmdCallback(pCmdCallBack);
	pCmdCallBack m_CMDFunc;
	DUI_DECLARE_MESSAGE_MAP()

private:
	NOTIFYICONDATA nid;
	CTileLayoutUI* HLay_E = NULL;
	CTileLayoutUI* HLay_D = NULL;
	CTileLayoutUI* HLay_F = NULL;
	CTabLayoutUI* HLay_Tab = NULL;
	COptionUI* HLay_Tab_E = NULL;
	COptionUI* HLay_Tab_D = NULL;
	COptionUI* HLay_Tab_F = NULL;

	CEditUI* HLay_Edit = NULL;

	void TextChange(CDuiString strInfo);

	map<WCHAR, CItemFrame*> m_E_List;
	map<WCHAR, CItemFrame*> m_D_List;

	map<WCHAR, CItemFrame*> m_F_List;
	map<WCHAR, CItemFrame*>* m_Now_List;

	VOID OnDropFiles(HWND hwnd, HDROP hDropInfo);

	BOOL m_bEditMode;
	BOOL LoadDate();
	BOOL SaveData();

public:
	volatile BOOL m_D_char[26];
	volatile BOOL m_F_char[26];
	volatile BOOL m_E_char[26];
	void AddItem(int Type, LPWSTR ShortCutKey, LPWSTR lpszFilePath);
	void DeleteItem(int Type, WCHAR ShortCutKey);
};
