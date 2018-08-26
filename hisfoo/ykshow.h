#pragma once

#include "resource.h"		// 主符号
#include "hisfooDlg.h"
#include "filepcs.h"
#include "kbpanel.h"
#include "autorunctrl.h"
#include <stack>
class kbpanel;
class filepcs;
class ChisfooDlg;
class autorunctrl;
#define SHOW_NORMAL            1
#define SHOW_CENTER            2
#define SHOW_STRETCH        3
#define SHOW_ISOSTRETCH        4

// ykshow 对话框

class ykshow : public CDialogEx
{
	DECLARE_DYNAMIC(ykshow)

public:
	ykshow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ykshow();
	// Main Window
	ChisfooDlg* CallBackEnv = NULL;
	int  ShowDib(HDC hdc, BITMAPFILEHEADER * pbmfh, int cxClient, int cyClient, int wShow)
	{
		if (NULL == pbmfh)
			return 0;
		return ShowDib(hdc, (BITMAPINFO*)(pbmfh + 1), (BYTE*)pbmfh + (pbmfh->bfOffBits), cxClient, cyClient, wShow);
	}
	int ShowDib(HDC hdc, BITMAPINFO * pbmi, BYTE * pBits, int cxClient, int cyClient, int wShow)
	{
		if (NULL == hdc || NULL == pbmi || NULL == pBits || wShow<SHOW_NORMAL || wShow>SHOW_ISOSTRETCH)
			return 0;
		int cxDib, cyDib;
		int BltMode, ret;
		if (pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
		{
			cxDib = ((BITMAPCOREHEADER *)pbmi)->bcWidth;
			cyDib = ((BITMAPCOREHEADER *)pbmi)->bcHeight;
		}
		else
		{
			cxDib = pbmi->bmiHeader.biWidth;
			cyDib = abs(pbmi->bmiHeader.biHeight);
		}
		switch (wShow)
		{
		case SHOW_NORMAL:
			return SetDIBitsToDevice(hdc, 0, 0, cxDib, cyDib, 0, 0,
				0, cyDib, pBits, pbmi, DIB_RGB_COLORS);

		case SHOW_CENTER:
			return SetDIBitsToDevice(hdc, (cxClient - cxDib) / 2, (cyClient - cyDib) / 2,
				cxDib, cyDib, 0, 0, 0, cyDib, pBits, pbmi, DIB_RGB_COLORS);

		case SHOW_STRETCH:
			BltMode = SetStretchBltMode(hdc, COLORONCOLOR);
			ret = StretchDIBits(hdc, 0, 0, cxClient, cyClient,
				0, 0, cxDib, cyDib,
				pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
			SetStretchBltMode(hdc, BltMode);
			return ret;

		case SHOW_ISOSTRETCH:
			BltMode = SetStretchBltMode(hdc, COLORONCOLOR);
			if (cxDib*cyClient > cyDib*cxClient)//图更宽
				ret = StretchDIBits(hdc, 0,
				(cyClient - (int)((float)cyDib / (float)cxDib*(float)cxClient)) / 2,
					cxClient, (int)((float)cyDib / (float)cxDib*(float)cxClient),
					0, 0, cxDib, cyDib, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
			else
				ret = StretchDIBits(hdc,
				(cxClient - (int)((float)cxDib / (float)cyDib*(float)cyClient)) / 2, 0,
					(int)((float)cxDib / (float)cyDib*(float)cyClient), cyClient,
					0, 0, cxDib, cyDib, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
			SetStretchBltMode(hdc, BltMode);
			return ret;
		}
		return 0;
	}
	shared_ptr<unsigned long> ShowFlag;
	unsigned long long DoBuffer = 0ULL;
	unsigned long long Moux = 0ULL;
	unsigned long long Mouy = 0ULL;
	unsigned long KeyBoardHook = 0;
	void ShowThread();
	unsigned long Working = 0UL;
	unsigned long GetPicShot = 0UL;
	unsigned long long PicSize = 0ULL;
	stack<thread> NxThread;
	bool InitNetEnv()
	{
		// 进行网络环境的初始化操作  
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("WSAStartup failed\n");
			return false;
		}
		return true;
	}
	BOOL OnInitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
	SOCKET slisten;
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

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnStnClickedPicctrl();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
};

#define INVALID_MSG      -1   // 无效的消息标识  
#define MSG_FILENAME     1   // 文件的名称  
#define MSG_FILELENGTH     2   // 传送文件的长度  
#define MSG_CLIENT_READY    3   // 客户端准备接收文件  
#define MSG_FILE      4   // 传送文件  
#define MSG_SENDFILESUCCESS    5   // 传送文件成功  
#define MSG_OPENFILE_ERROR    10   // 打开文件失败,可能是文件路径错误找不到文件等原因  
#define MSG_FILEALREADYEXIT_ERROR 11   // 要保存的文件已经存在了
class CCSDef
{
public:
#pragma pack(1)      // 使结构体的数据按照1字节来对齐,省空间  
	// 消息头  
	struct TMSG_HEADER
	{
		char    cMsgID;    // 消息标识  
		TMSG_HEADER(char MsgID = INVALID_MSG)
			: cMsgID(MsgID)
		{
		}
	};
	// 请求传送的文件名  
	// 客户端传给服务器端的是全路径名称  
	// 服务器传回给客户端的是文件名  
	struct TMSG_FILENAME : public TMSG_HEADER
	{
		char szFileName[256];   // 保存文件名的字符数组  
		TMSG_FILENAME()
			: TMSG_HEADER(MSG_FILENAME)
		{
		}
	};
	// 传送文件长度  
	struct TMSG_FILELENGTH : public TMSG_HEADER
	{
		long lLength;
		TMSG_FILELENGTH(long length)
			: TMSG_HEADER(MSG_FILELENGTH), lLength(length)
		{
		}
	};
	// Client端已经准备好了,要求Server端开始传送文件  
	struct TMSG_CLIENT_READY : public TMSG_HEADER
	{
		TMSG_CLIENT_READY()
			: TMSG_HEADER(MSG_CLIENT_READY)
		{
		}
	};
	// 传送文件  
	struct TMSG_FILE : public TMSG_HEADER
	{
		union     // 采用union保证了数据包的大小不大于MAX_PACKET_SIZE * sizeof(char)  
		{
			char szBuff[Pkg_Leng];
			struct
			{
				int nStart;
				int nSize;
				char szBuff[Pkg_Leng - 2 * sizeof(int)];
			}tFile;
		};
		TMSG_FILE()
			: TMSG_HEADER(MSG_FILE)
		{
		}
	};
	// 传送文件成功  
	struct TMSG_SENDFILESUCCESS : public TMSG_HEADER
	{
		TMSG_SENDFILESUCCESS()
			: TMSG_HEADER(MSG_SENDFILESUCCESS)
		{
		}
	};
	// 传送出错信息,包括:  
	// MSG_OPENFILE_ERROR:打开文件失败  
	// MSG_FILEALREADYEXIT_ERROR:要保存的文件已经存在了  
	struct TMSG_ERROR_MSG : public TMSG_HEADER
	{
		TMSG_ERROR_MSG(char cErrorMsg)
			: TMSG_HEADER(cErrorMsg)
		{
		}
	}; 
#pragma pack()
};