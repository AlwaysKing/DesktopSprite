#pragma once
#include "stdafx.h"
#include <windows.h>

class Locker
{
public:
	CRITICAL_SECTION		m_Locker; //һ��ȫ�ֱ���

	Locker()
	{
		InitializeCriticalSection(&m_Locker); //��ĳ���ط���ʼ�����ȫ�ֱ���
	}

	~Locker()
	{
		DeleteCriticalSection(&m_Locker);// ɾ���ٽ���
	}

	void Lock()
	{
		EnterCriticalSection(&m_Locker);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_Locker);
	}
};
