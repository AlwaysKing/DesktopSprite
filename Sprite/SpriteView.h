#pragma once
#include "stdafx.h"

class SpriteView;

typedef void(*SpriteCallBack)(SpriteView*, HWND, UINT, LPARAM, LPARAM );

#define WM_MOUSEIN   WM_USER+1
#define WM_MOUSEOUT  WM_MOUSEIN+1

// ����������ʾSpriteView�Ĵ���
class SpriteView
{
public:
	/**	��ʼ������
		hInst ʵ����� 
	*/
	SpriteView(HINSTANCE hInst);

	/**��ʼ������
		hInst		ʵ�����
		GifPath		GIFͼƬ��·��
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath);

	/**��ʼ������
		hInst		ʵ�����
		GifPath		GIFͼƬ��·��
		pCallBack	�ص�������ַ
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath, SpriteCallBack pCallBack);

	/**��ʼ������
		hInst		ʵ�����
		GifPath		GIFͼƬ��·��
		pCallBack	�ص�������ַ
		PosX		����X����
		PosY		����Y����
		Width		���ڿ�
		Height		���ڸ�
	*/
	SpriteView(HINSTANCE hInst, LPWSTR GifPath, SpriteCallBack pCallBack, int PosX, int PosY, int Width, int Height);

	/**��������*/
	~SpriteView();

	/**�����Ƿ�����ֶ��ƶ�*/
	void SetMoveEnable(BOOL);

	/**�����Ƿ��ö�*/
	void SetTopMost(BOOL);

	/**���ûص�������ַ*/
	void SetCallBack(SpriteCallBack);

	/**���ñ���ͼƬ�ĵ�ַ*/
	BOOL SetImageBack(LPWSTR);

	/**����GIFͼƬ�ĵ�ַ*/
	BOOL SetImage(LPWSTR);

	/**��ȡGIFͼƬ�ж����ĸ���*/
	UINT GetDimensionsCount();

	/**��ʼ��ʾ Ĭ����ʾ��һ��*/
	BOOL Start();

	/**��ʼ��ʾ
		Index ָ����ʾ�Ķ��������� ��0��ʼ
	*/
	BOOL Start(UINT Index);

	/**ֹͣ��ʾ*/
	void Stop();

	/**�������һ�β��ź������һ�εĲ��ż��
		Interval ���ʱ�䵥λ��1ms -1 ��ʾ��ͣ����
	*/
	void SetPlayInterval(DWORD Interval);

	/**���ô���λ��
		PosX	x����
		PosY	y����
	*/
	void SetPos(int PosX, int PosY);

	/**���ô��ڴ�С
		Width	���ڿ�
		Height	���ڸ�
	*/
	void SetSize(int Width, int Height);

	/**���ô���λ�úʹ�С
		PosX	x����
		PosY	y����
		Width	���ڿ�
		Height	���ڸ�
	*/
	void SetWndPos(int PosX, int PosY, int Width, int Height);

	/**���ٲ���
		Qucik ����ϵ�� 1 ��ʾ��������С��1 �Ǽӿ죬����1 �Ǽ���
	*/
	void SetQuick(float Qucik);

	/**���ô����Ƿ�ɼ�
		enable TRUEΪ�ɼ�
	*/
	void SetVisable(BOOL enable);

	// ���ڵ�λ�úʹ�С
	int m_PosX, m_PosY, m_Width, m_Height;

	// ���ٱ���
	float m_Qucik;

	// �Ƿ�����ֶ��ƶ�
	BOOL m_bMoveEnbale;

	// ���ھ��
	HWND m_Hwnd;

	// �Ƿ����ö�
	BOOL m_bTopMost;

protected:
	void Init(HINSTANCE, LPWSTR, SpriteCallBack, int, int, int, int);				// ��ʼ������
	void UnInit();

	SpriteCallBack m_CallBack;	// �ص�֪ͨ
	BOOL m_bStart;				// �Ƿ��Ѿ���ʼ
	HINSTANCE m_Hinst;			// ʵ�����
	DWORD m_PlayInterval;		// ���ż��
	BOOL m_MouseIn;				// ����Ƿ��ڷ�Χ��
	BOOL m_bMoving;				// �ƶ���
	BOOL m_bButtonDown;			// ��ť�Ƿ���

	Image* m_pImage;			// ͼ��ʵ��
	Image* m_pImageBack;		// ����ͼƬ
	UINT m_DimensionsCount;		// ��������
	GUID *m_pDimensionIDs;		// ����������GUID
	UINT m_FrameCount;			// ָ��������֡��
	PropertyItem* m_pFrameTime;	// ��¼��ǰ������ÿ֡��ʱ��

	BOOL CreateView();			// �����߳�
	static DWORD WINAPI JobThread(PVOID);	// �̺߳�������Ҫ��ע�ᴰ�����������Ϣѭ��
	HANDLE m_hJobThread;					// �ֳɾ��
	HANDLE m_hJobEvent;						// ͬ�����

	// ���̺���
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProcSelf(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


	UINT m_DrawDimensionIndex;				// ��ǰ�滭�Ķ���
	UINT m_DrawFrameIndex;					// ��ǰ�滭����֡������

	void DrawFrame(HWND, UINT, UINT, BOOL);		// ���ƺ���
	BOOL DrawCtrl(DWORD&);					// ���ƿ��ƺ���

protected:
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wparam, LPARAM lparam);
	VOID CALLBACK MouseProcSelf(int nCode, WPARAM wparam, LPARAM lparam);
	static VOID InitialFunc(HINSTANCE hInstance);
	static VOID UnInitialFunc(VOID);
};