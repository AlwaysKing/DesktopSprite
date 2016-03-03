#pragma once
#include "stdafx.h"

class SpriteView;

typedef void(*SpriteCallBack)(SpriteView*, HWND, UINT, LPARAM, LPARAM );

#define WM_MOUSEIN   WM_USER+1
#define WM_MOUSEOUT  WM_MOUSEIN+1

// 定义用于显示SpriteView的窗口
class SpriteView
{
public:
	/**	初始化函数
		hInst 实例句柄 
	*/
	SpriteView(HINSTANCE hInst);

	/**初始化函数
		hInst		实例句柄
		GifPath		GIF图片的路径
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath);

	/**初始化函数
		hInst		实例句柄
		GifPath		GIF图片的路径
		pCallBack	回调函数地址
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath, SpriteCallBack pCallBack);

	/**初始化函数
		hInst		实例句柄
		GifPath		GIF图片的路径
		pCallBack	回调函数地址
		PosX		窗口X坐标
		PosY		窗口Y坐标
		Width		窗口宽
		Height		窗口高
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath, SpriteCallBack pCallBack, int PosX, int PosY, int Width, int Height);

	/**析构函数*/
	~SpriteView();

	/**设置是否可以手动移动*/
	void SetMoveEnable(BOOL);

	/**设置是否置顶*/
	void SetTopMost(BOOL);

	/**设置回调函数地址*/
	void SetCallBack(SpriteCallBack);

	/**设置背景图片的地址*/
	BOOL SetImageBack(LPWSTR);

	/**设置GIF图片的地址*/
	BOOL SetImage(LPWSTR);

	/**获取GIF图片中动画的个数*/
	UINT GetDimensionsCount();

	/**开始显示 默认显示第一个*/
	BOOL Start();

	/**开始显示
		Index 指定显示的动画的索引 从0开始
	*/
	BOOL Start(UINT Index);

	/**停止显示*/
	void Stop();

	/**设置完成一次播放后距离下一次的播放间隔
		Interval 间隔时间单位是1ms -1 表示暂停播放
	*/
	void SetPlayInterval(DWORD Interval);

	/**设置窗口位置
		PosX	x坐标
		PosY	y坐标
	*/
	void SetPos(int PosX, int PosY);

	/**设置窗口大小
		Width	窗口款
		Height	窗口高
	*/
	void SetSize(int Width, int Height);

	/**设置窗口位置和大小
		PosX	x坐标
		PosY	y坐标
		Width	窗口款
		Height	窗口高
	*/
	void SetWndPos(int PosX, int PosY, int Width, int Height);

	/**加速播放
		Qucik 加速系数 1 表示正常被，小于1 是加快，大于1 是减速
	*/
	void SetQuick(float Qucik);

	/**设置窗口是否可见
		enable TRUE为可见
	*/
	void SetVisable(BOOL enable);

	// 窗口的位置和大小
	int m_PosX, m_PosY, m_Width, m_Height;

	// 加速倍数
	float m_Qucik;

	// 是否可以手动移动
	BOOL m_bMoveEnbale;

	// 窗口句柄
	HWND m_Hwnd;

	// 是否做置顶
	BOOL m_bTopMost;

protected:
	void Init(HINSTANCE, LPWSTR, SpriteCallBack, int, int, int, int);				// 初始化函数
	void UnInit();

	SpriteCallBack m_CallBack;	// 回调通知
	BOOL m_bStart;				// 是否已经开始
	HINSTANCE m_Hinst;			// 实例句柄
	DWORD m_PlayInterval;		// 播放间隔
	BOOL m_MouseIn;				// 鼠标是否在范围内
	BOOL m_bMoving;				// 移动中
	BOOL m_bButtonDown;			// 按钮是否按下

	Image* m_pImage;			// 图像实例
	Image* m_pImageBack;		// 背景图片
	UINT m_DimensionsCount;		// 动画数量
	GUID *m_pDimensionIDs;		// 动画数量的GUID
	UINT m_FrameCount;			// 指定动画的帧数
	PropertyItem* m_pFrameTime;	// 记录当前动画的每帧的时间

	BOOL CreateView();			// 创建线程
	static DWORD WINAPI JobThread(PVOID);	// 线程函数，主要是注册窗口类和启动消息循环
	HANDLE m_hJobThread;					// 现成句柄
	HANDLE m_hJobEvent;						// 同步句柄

	// 过程函数
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProcSelf(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


	UINT m_DrawDimensionIndex;				// 当前绘画的动画
	UINT m_DrawFrameIndex;					// 当前绘画到的帧数索引

	void DrawFrame(HWND, UINT, UINT, BOOL);		// 绘制函数
	BOOL DrawCtrl(DWORD&);					// 绘制控制函数

protected:
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wparam, LPARAM lparam);
	VOID CALLBACK MouseProcSelf(int nCode, WPARAM wparam, LPARAM lparam);
	static VOID InitialFunc(HINSTANCE hInstance);
	static VOID UnInitialFunc(VOID);
};