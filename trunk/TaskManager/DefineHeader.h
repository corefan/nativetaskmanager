#ifndef __DEFINE_HEADER_H
#define __DEFINE_HEADER_H


#define WM_DELETETASK			WM_USER+100			// ɾ������������
#define WM_ADDTASK				WM_USER+101			// �������������

#define WM_UPDATE_APP_LIST		WM_USER+102			// ����Application ListCtrl

#define WM_FLUSH_FILE			WM_USER+103			// ����Process File ListCtrl����֪ͨ
#define WM_UPDATE_FILE_INFO		WM_USER+104			// ����Process File ListCtrl

#define WM_UPDATE_PROCESS_LIST	WM_USER+105			// ����Process ListCtrl
#define WM_UPDATE_MODULE_LIST	WM_USER+106			// ����Module ListCtrl

#define WM_FLUSH_MODULE			WM_USER+116			// ����Process Module ListCtrl����֪ͨ
#define WM_UPDATE_MODULE_INFO	WM_USER+117			// ����Process Module ListCtrl

#define WM_FLUSH_THREAD			WM_USER+118			// ����Process Thread ListCtrl����֪ͨ	
#define WM_UPDATE_THREAD_INFO	WM_USER+119			// ����Process Thread ListCtrl

#define WM_FLUSH_MEMORY			WM_USER+120			// ����Process Memory ListCtrl����֪ͨ	
#define WM_UPDATE_MEMORY_INFO	WM_USER+121			// ����Process Memory ListCtrl

#define WM_FLUSH_PORT			WM_USER+122			// ����Process Port ListCtrl����֪ͨ	
#define WM_UPDATE_PORT_INFO		WM_USER+123			// ����Process Port ListCtrl

#define WM_FLUSH_ENV			WM_USER+124			// ����Process Environment ListCtrl����֪ͨ	
#define WM_UPDATE_ENV_INFO		WM_USER+125			// ����Process Environment ListCtrl

#define WM_FLUSH_HANDLE			WM_USER+126			// ����Process Handle ListCtrl����֪ͨ	
#define WM_UPDATE_HANDLE_INFO	WM_USER+127			// ����Process Handle ListCtrl

#define WM_UPDATE_DISK_INFO		WM_USER+128			// ����Disk����

#define WM_UPDATE_SERVICE_IFNO	WM_USER+129			// ����Service����

#define WM_TAB_SEL_CHANGED		WM_USER+200			// tab��ı���Ϣ
#define WM_SET_PAUSE			WM_USER+201			// ��ͣ�߳�

typedef struct tagTabInfo
{
	CDialog	*pDlg;								// Tab�������
	CString strName;							// Tab��������
}TabInfo;


#endif