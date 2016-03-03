#pragma once
#include "stdafx.h"
#include <windows.h>

class Locker
{
public:
	CRITICAL_SECTION		m_Locker; //一个全局变量

	Locker()
	{
		InitializeCriticalSection(&m_Locker); //在某个地方初始化这个全局变量
	}

	~Locker()
	{
		DeleteCriticalSection(&m_Locker);// 删除临界区
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
