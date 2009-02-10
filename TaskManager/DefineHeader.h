#ifndef __DEFINE_HEADER_H
#define __DEFINE_HEADER_H


#define WM_DELETETASK			WM_USER+100			// 删除任务栏窗口
#define WM_ADDTASK				WM_USER+101			// 添加任务栏窗口

#define WM_UPDATE_APP_LIST		WM_USER+102			// 更新Application ListCtrl

#define WM_FLUSH_FILE			WM_USER+103			// 发送Process File ListCtrl更新通知
#define WM_UPDATE_FILE_INFO		WM_USER+104			// 更新Process File ListCtrl

#define WM_UPDATE_PROCESS_LIST	WM_USER+105			// 更新Process ListCtrl
#define WM_UPDATE_MODULE_LIST	WM_USER+106			// 更新Module ListCtrl

#define WM_FLUSH_MODULE			WM_USER+116			// 发送Process Module ListCtrl更新通知
#define WM_UPDATE_MODULE_INFO	WM_USER+117			// 更新Process Module ListCtrl

#define WM_FLUSH_THREAD			WM_USER+118			// 发送Process Thread ListCtrl更新通知	
#define WM_UPDATE_THREAD_INFO	WM_USER+119			// 更新Process Thread ListCtrl

#define WM_FLUSH_MEMORY			WM_USER+120			// 发送Process Memory ListCtrl更新通知	
#define WM_UPDATE_MEMORY_INFO	WM_USER+121			// 更新Process Memory ListCtrl

#define WM_FLUSH_PORT			WM_USER+122			// 发送Process Port ListCtrl更新通知	
#define WM_UPDATE_PORT_INFO		WM_USER+123			// 更新Process Port ListCtrl

#define WM_FLUSH_ENV			WM_USER+124			// 发送Process Environment ListCtrl更新通知	
#define WM_UPDATE_ENV_INFO		WM_USER+125			// 更新Process Environment ListCtrl

#define WM_FLUSH_HANDLE			WM_USER+126			// 发送Process Handle ListCtrl更新通知	
#define WM_UPDATE_HANDLE_INFO	WM_USER+127			// 更新Process Handle ListCtrl

#define WM_UPDATE_DISK_INFO		WM_USER+128			// 更新Disk界面

#define WM_UPDATE_SERVICE_IFNO	WM_USER+129			// 更新Service界面

#define WM_TAB_SEL_CHANGED		WM_USER+200			// tab项改变消息
#define WM_SET_PAUSE			WM_USER+201			// 暂停线程

typedef struct tagTabInfo
{
	CDialog	*pDlg;								// Tab关联句柄
	CString strName;							// Tab关联名称
}TabInfo;


#endif