#ifndef __ENUM_TASK_WINDOW_H
#define __ENUM_TASK_WINDOW_H

#include <windows.h>
#include <vector>
#include <string>

using namespace std;

// UNICODE 设置
#ifdef _UNICODE
#define string wstring
#else
#define string string
#endif


// Task Window 详细信息
typedef struct _tagTaskWndInfo
{
	HWND		m_hWnd;			// 句柄
	DWORD		m_dwID;			// 进程ID
	HICON		m_hIcon;		// 关联图标
	string		m_strText;		// 名称
}TaskWndInfo;


class CEnumTaskWindow
{
public:
	CEnumTaskWindow(void);
	virtual ~CEnumTaskWindow(void);

public:
	void EnumTaskWindow(HWND hWnd);							// 接口--指定父窗口
	void Clear();											// 清除内存

	vector<TaskWndInfo> &GetTaskWndInfo();					// 获得Task Info

private:
	static BOOL CALLBACK EnumTaskWindowProc(HWND hWnd,LPARAM lParam);// 回调函数,枚举任务栏窗口
	HICON GetWindowIcon(HWND hWnd/*DWORD dwID*/);			// 获取窗口图标

private:
	//TaskWndInfo	m_TaskWndInfo;							
	vector<TaskWndInfo>		m_vecTask;						// 成员

};


#endif