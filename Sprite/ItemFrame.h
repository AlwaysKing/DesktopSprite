#pragma once
#include "UIlib.h"
#include <map>
#include <shellapi.h>

using namespace std;
using namespace DuiLib;

class CItemFrame
{
public:
	CItemFrame(CControlUI* pUI);
	void SetInfo(int Type, LPWSTR ShortCutKey, LPWSTR lpszFilePath);
	void Open();
	VOID SetModel(BOOL bDelete);
	CDuiString m_strString;
	CControlUI* pThis;
};
