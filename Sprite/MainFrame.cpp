#include "stdafx.h"
#include "resource.h"
#include "UIlib.h"
#include "MainFrame.h"
#include "ItemFrame.h"
#include "AddFrame.h"
#include "json.h"

#define WM_USER_HIDE 123

DUI_BEGIN_MESSAGE_MAP(CDuiFrameWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_TEXTCHANGED, OnTextChange)
DUI_END_MESSAGE_MAP()

LPCTSTR CDuiFrameWnd::GetWindowClassName() const
{
	return _T("DUIMainFrame");
}

CDuiString CDuiFrameWnd::GetSkinFile()
{
	return _T("duilib.xml");
}

CDuiString CDuiFrameWnd::GetSkinFolder()
{
	return _T("");
}

LRESULT CDuiFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	HLay_E = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(_T("ItemList_E")));
	HLay_D = static_cast<CTileLayoutUI*>( m_PaintManager.FindControl(_T("ItemList_D")));
	HLay_F = static_cast<CTileLayoutUI*>( m_PaintManager.FindControl(_T("ItemList_F")));
	HLay_Tab = static_cast<CTabLayoutUI*>( m_PaintManager.FindControl(_T("Switch")) );
	HLay_Edit = static_cast<CEditUI*>( m_PaintManager.FindControl(_T("ShortKey")) );
	HLay_Tab_E = static_cast<COptionUI*>( m_PaintManager.FindControl(_T("List_E")) );
	HLay_Tab_D = static_cast<COptionUI*>( m_PaintManager.FindControl(_T("List_D")) );
	HLay_Tab_F = static_cast<COptionUI*>( m_PaintManager.FindControl(_T("List_F")) );

	m_bEditMode = FALSE;

	for(int i = 0; i < 26; i++)
	{
		m_D_char[i] = false;
		m_F_char[i] = false;
		m_E_char[i] = false;
	}

	// 读取配置文件
	LoadDate();

	ShowWindow(false, false);

	return rv;
}

void CDuiFrameWnd::AddItem(int Type, LPWSTR ShortCutKey, LPWSTR lpszFilePath)
{
	CDialogBuilder builder;
	CControlUI* pUI = (CControlUI*)builder.Create(_T("item.xml"));
	CItemFrame* pFrame = new CItemFrame(pUI);
	pFrame->SetInfo(Type, ShortCutKey, lpszFilePath);

	if(Type == 1){
		if(HLay_E)
		{
			HLay_E->Add(pUI);
			m_E_List.insert(make_pair(ShortCutKey[0], pFrame));
			m_E_char[ShortCutKey[0] - 'A'] = true;
		}
	}
	else if(Type == 2){
		if(HLay_D)
		{
			HLay_D->Add(pUI);
			m_D_List.insert(make_pair(ShortCutKey[0], pFrame));
			m_D_char[ShortCutKey[0] - 'A'] = true;
		}
	}
	else{
		if(HLay_F){
			HLay_F->Add(pUI);
			m_F_List.insert(make_pair(ShortCutKey[0], pFrame));
			int i = ShortCutKey[0] - 'A';
			m_F_char[ShortCutKey[0] - 'A'] = true;
		}
	}
}
void CDuiFrameWnd::DeleteItem(int Type, WCHAR ShortCutKey)
{
	map<WCHAR, CItemFrame*>* pDeleteMap;
	CTileLayoutUI*	pTab;
	map<WCHAR, CItemFrame*>::iterator it;
	BOOL rv = FALSE;

	switch(Type)
	{
		case 1:
			pDeleteMap = &m_E_List;
			pTab = HLay_E;
			m_E_char[ShortCutKey - 'A'] = false;
			break;
		case 2:
			pDeleteMap = &m_D_List;
			pTab = HLay_D;
			m_D_char[ShortCutKey - 'A'] = false;
			break;
		case 3:
			pDeleteMap = &m_F_List;
			pTab = HLay_F;
			m_F_char[ShortCutKey - 'A'] = false;
			break;
		default:
			goto end;
	}

	// 寻找对应的控件
	it = pDeleteMap->find(ShortCutKey);
	if(it == pDeleteMap->end()){
		goto end;
	}

	// 从控件中删除
	if(!pTab->Remove(it->second->pThis)){
		goto end;
	}

	// 从列表中删除
	pDeleteMap->erase(it);
	rv = TRUE;
end:

	if(!rv){
		MessageBox(this->GetHWND(), L"删除失败", L"失败", MB_OK);

	}
	else{
		// 保存文件
		SaveData();
	}
}

LRESULT CDuiFrameWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT rv = WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
	UnregisterHotKey(m_hWnd, 1);
	Shell_NotifyIcon(NIM_DELETE, &nid);

	map<WCHAR, CItemFrame*>::iterator it = m_E_List.begin();
	for(; it != m_E_List.end(); it++)
	{
		delete it->second;
	}
	for(; it != m_D_List.end(); it++)
	{
		delete it->second;
	}
	for(; it != m_F_List.end(); it++)
	{
		delete it->second;
	}

	return rv;
}

LRESULT CDuiFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE://拦截ESC退出界面
					return FALSE;
			}
	}
	return WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
}

BOOL CDuiFrameWnd::TestShortCut(int i, int s)
{
	BOOL rv = FALSE;
	switch (i)
	{
		case 1:rv = m_E_char[s]; break;
		case 2:rv = m_D_char[s]; break;
		case 3:rv = m_F_char[s]; break;
	}
	return rv;
}

VOID CDuiFrameWnd::SetCmdCallback(pCmdCallBack p)
{
	m_CMDFunc = p;
}

LRESULT  CDuiFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
		case WM_COMMAND:
		{
			if(wParam == 0)
			{
				if(MessageBox(m_hWnd, L"是否关闭Spite", L"询问", MB_OKCANCEL) == IDOK)
				{
					// 清理托盘图标
					Shell_NotifyIcon(NIM_DELETE, &nid);
					PostQuitMessage(0);
				}
			}
			else if(m_CMDFunc)
			{
				m_CMDFunc(uMsg, wParam, lParam, this->GetHWND());
			}

		}break;
		case WM_USER:
		{
			// 托盘点击事件处理
			if(lParam == WM_RBUTTONDOWN)
			{
				if(m_CMDFunc)
				{
					m_CMDFunc(uMsg, wParam, lParam, this->GetHWND());
				}
			}

			if(lParam == WM_USER_HIDE){
				ShowWindow(false, false);
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
					HLay_Edit->SetText(L"");
				}
				else
				{
					ShowWindow();
					SetForegroundWindow(this->GetHWND());
					SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					HLay_Edit->SetText(L"");
				}
			}
			break;
		}
		case WM_SETFOCUS:
			break;
		case WM_KEYDOWN:
		{
			int iStart = 'A';
			int iEnd = 'Z';
			if(iStart <= wParam && wParam <= iEnd){
				CDuiString m_strInputKey;
				m_strInputKey = HLay_Edit->GetText();
				m_strInputKey += (WCHAR)wParam;
				HLay_Edit->SetText(m_strInputKey);
				TextChange(m_strInputKey);
			}

			// 删除
			if(wParam == VK_BACK)
			{
				CDuiString m_strInputKey;
				m_strInputKey = HLay_Edit->GetText();
				m_strInputKey = m_strInputKey.Left(m_strInputKey.GetLength() - 1);
				HLay_Edit->SetText(m_strInputKey);
				TextChange(m_strInputKey);
			}
		}
		break;

		case WM_DROPFILES:{
			OnDropFiles(this->GetHWND(), (HDROP)wParam);
		}

		break;
	}

	return 0;
}

void CDuiFrameWnd::OnClick(TNotifyUI& msg)
{
	if(msg.pSender->GetName() == _T("List_E"))
	{
		TextChange(L"E");
		HLay_Edit->SetText(L"E");
	}
	else if(msg.pSender->GetName() == _T("List_D"))
	{
		TextChange(L"D");
		HLay_Edit->SetText(L"D");
	}
	else if(msg.pSender->GetName() == _T("List_F"))
	{
		TextChange(L"F");
		HLay_Edit->SetText(L"F");
	}
	else if(msg.pSender->GetName() == _T("Edit"))
	{
		// 标记此时进入编辑模式
		COptionUI* pOp = (COptionUI*)msg.pSender;
		pOp->Selected(m_bEditMode);
		m_bEditMode = !m_bEditMode;
		map<WCHAR, CItemFrame*>::iterator it;
		
		it = m_E_List.begin();
		for(; it != m_E_List.end(); it++)
		{
			it->second->SetModel(m_bEditMode);
		}
		it = m_D_List.begin();
		for(; it != m_D_List.end(); it++)
		{
			it->second->SetModel(m_bEditMode);
		}
		it = m_F_List.begin();
		for(; it != m_F_List.end(); it++)
		{
			it->second->SetModel(m_bEditMode);
		}

		if(m_bEditMode == false){
			SaveData();
		}
	}
	else if(msg.pSender->GetName() == _T("Delete"))
	{
		CDuiString strTypel = msg.pSender->GetUserData();
		switch(strTypel[0])
		{
			case '1':
				DeleteItem(1, strTypel[2]);
				break;
			case '2':
				DeleteItem(2, strTypel[2]);
				break;
			case '3':
				DeleteItem(3, strTypel[2]);
				break;
		}
	}
}

void CDuiFrameWnd::OnTextChange(TNotifyUI& msg)
{
	if(msg.pSender->GetName() == L"ShortKey")
	{

		CDuiString strInfo = msg.pSender->GetText();
		strInfo.MakeUpper();
		TextChange(strInfo);
	}
}

void CDuiFrameWnd::TextChange(CDuiString strInfo)
{
	if(strInfo.GetLength() >= 1)
	{
		if(strInfo[0] == 'e' || strInfo[0] == 'E')
		{
			HLay_Tab->SelectItem(0);
			m_Now_List = &m_E_List;
			HLay_Tab_E->Selected(true);
		}
		if(strInfo[0] == 'd' || strInfo[0] == 'D')
		{
			HLay_Tab->SelectItem(1);
			m_Now_List = &m_D_List;
			HLay_Tab_D->Selected(true);
		}
		if(strInfo[0] == 'f' || strInfo[0] == 'F')
		{
			HLay_Tab->SelectItem(2);
			m_Now_List = &m_F_List;
			HLay_Tab_F->Selected(true);
		}
	}

	if(strInfo.GetLength() >= 2 && m_Now_List)
	{
		WCHAR Key = strInfo[1];
		map<WCHAR, CItemFrame*>::iterator it = m_Now_List->find(Key);
		if(it != m_Now_List->end())
		{
			it->second->Open();
			HLay_Edit->SetText(L"");
			PostMessage(WM_USER, 0, WM_USER_HIDE);
		}
		else
		{
			HLay_Edit->SetText(L"");
		}
	}

}

BOOL CDuiFrameWnd::LoadDate()
{
	BOOL rv = FALSE;
	PBYTE pBuff = NULL;
	ULONG ulFileSize = 0;

	CDuiString strDataFile 	= CPaintManagerUI::GetInstancePath() + L"Data\\conf.data";
	HANDLE m_hFile = CreateFile(strDataFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE){
		goto end;
	}

	ulFileSize = GetFileSize(m_hFile, NULL);

	// 申请内存
	pBuff = (PBYTE)LocalAlloc(LPTR, ulFileSize);
	if(pBuff == NULL){
		goto end;
	}

	DWORD dwToread = 0;
	if(ReadFile(m_hFile, pBuff, ulFileSize, &dwToread, NULL) == false){
		goto end;
	}

	if(dwToread != ulFileSize){
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

		if(!root["Exe"].isNull())
		{
			Json::Value Exe = root["Exe"];
			int file_size = root["Exe"].size();
			for(int i = 0; i < file_size; i++)
			{
				CString Shrotcut(Exe[i]["ShortCut"].asString().c_str());
				CString FilePath(Exe[i]["FilePath"].asString().c_str());
				AddItem(1, Shrotcut.GetBuffer(), FilePath.GetBuffer());
			}
		}
		if(!root["Directory"].isNull())
		{
			Json::Value Dir = root["Directory"];
			int file_size = root["Directory"].size();
			for(int i = 0; i < file_size; i++)
			{
				CString Shrotcut(Dir[i]["ShortCut"].asString().c_str());
				CString FilePath(Dir[i]["FilePath"].asString().c_str());
				AddItem(2, Shrotcut.GetBuffer(), FilePath.GetBuffer());
			}
		}
		if(!root["Files"].isNull())
		{
			Json::Value Files = root["Files"];
			int file_size = root["Files"].size();
			for(int i = 0; i < file_size; i++)
			{
				CString Shrotcut(Files[i]["ShortCut"].asString().c_str());
				CString FilePath(Files[i]["FilePath"].asString().c_str());
				AddItem(3, Shrotcut.GetBuffer(), FilePath.GetBuffer());
			}
		}
	}
end:
	if(m_hFile != INVALID_HANDLE_VALUE){
		CloseHandle(m_hFile);
	}

	if(pBuff){
		LocalFree(pBuff);
	}

	return rv;
}

BOOL CDuiFrameWnd::SaveData()
{
	CString strFilePath;
	strFilePath = CPaintManagerUI::GetInstancePath() + L"Data\\conf.data";
	
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		return false;
	}

	Json::Value root;
	// 先写入Exe的
	map<WCHAR, CItemFrame*>::iterator it;

	it = m_E_List.begin();
	Json::Value Exe;
	for(; it != m_E_List.end(); it++)
	{
		CStringA ShortCut(it->first);
		CStringA FilePath(it->second->m_strString.GetData());
		int index = Exe.size();
		Exe[index]["ShortCut"] = ShortCut.GetBuffer();
		Exe[index]["FilePath"] = FilePath.GetBuffer();
	}
	root["Exe"] = Exe;
	Json::Value Dir;
	it = m_D_List.begin();
	for(; it != m_D_List.end(); it++)
	{
		CStringA ShortCut(it->first);
		CStringA FilePath(it->second->m_strString.GetData());
		int index = Dir.size();
		Dir[index]["ShortCut"] = ShortCut.GetBuffer();
		Dir[index]["FilePath"] = FilePath.GetBuffer();
	}
	 root["Directory"] = Dir;

	it = m_F_List.begin();
	Json::Value Files;
	for(; it != m_F_List.end(); it++)
	{
		CStringA ShortCut(it->first);
		CStringA FilePath(it->second->m_strString.GetData());
		int index = Files.size();
		Files[index]["ShortCut"] = ShortCut.GetBuffer();
		Files[index]["FilePath"] = FilePath.GetBuffer();
	}
	root["Files"] = Files;

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

VOID CDuiFrameWnd::OnDropFiles(HWND hwnd, HDROP hDropInfo)
{
	UINT  nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	TCHAR szFileName[_MAX_PATH] = _T("");
	DWORD dwAttribute;
	int Type = 3;


	// 获取拖拽进来文件和文件夹  
	for(UINT i = 0; i < nFileCount; i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));
		dwAttribute = ::GetFileAttributes(szFileName);

		// 是否为文件夹  
		if(dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			Type = 2;
		}
		else
		{
			// 判断是否是可执行文件
			if(GetBinaryType(szFileName, &dwAttribute))
			{
				Type = 1;
			}
		}

		CDuiAddFrame duiFrame;
		duiFrame.Create(NULL, _T("SpriteWmdAdd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE | WS_EX_TOPMOST);
		duiFrame.CenterWindow();
		duiFrame.SetInfo(Type, szFileName, this);
		duiFrame.ShowModal();
	}

	::DragFinish(hDropInfo);
	SaveData();
}