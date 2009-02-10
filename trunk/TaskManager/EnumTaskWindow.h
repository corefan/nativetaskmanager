#ifndef __ENUM_TASK_WINDOW_H
#define __ENUM_TASK_WINDOW_H

#include <windows.h>
#include <vector>
#include <string>

using namespace std;

// UNICODE ����
#ifdef _UNICODE
#define string wstring
#else
#define string string
#endif


// Task Window ��ϸ��Ϣ
typedef struct _tagTaskWndInfo
{
	HWND		m_hWnd;			// ���
	DWORD		m_dwID;			// ����ID
	HICON		m_hIcon;		// ����ͼ��
	string		m_strText;		// ����
}TaskWndInfo;


class CEnumTaskWindow
{
public:
	CEnumTaskWindow(void);
	virtual ~CEnumTaskWindow(void);

public:
	void EnumTaskWindow(HWND hWnd);							// �ӿ�--ָ��������
	void Clear();											// ����ڴ�

	vector<TaskWndInfo> &GetTaskWndInfo();					// ���Task Info

private:
	static BOOL CALLBACK EnumTaskWindowProc(HWND hWnd,LPARAM lParam);// �ص�����,ö������������
	HICON GetWindowIcon(HWND hWnd/*DWORD dwID*/);			// ��ȡ����ͼ��

private:
	//TaskWndInfo	m_TaskWndInfo;							
	vector<TaskWndInfo>		m_vecTask;						// ��Ա

};


#endif