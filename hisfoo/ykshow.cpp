// ykshow.cpp : 实现文件
//

#include "stdafx.h"
#include "hisfoo.h"
#include "ykshow.h"
#include "afxdialogex.h"


// ykshow 对话框

IMPLEMENT_DYNAMIC(ykshow, CDialogEx)

ykshow::ykshow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG6, pParent)
{

}

ykshow::~ykshow()
{
	if (ShowFlag)
		*ShowFlag = 0UL;
	
}

int recvimg(SOCKET server, shared_ptr<char> _Data, unsigned long long DataLen)
{
	char fBuffer[Pkg_Leng];
	unsigned long long latestpkg = 0ULL;
	unsigned long long Failed_Counter = 0ULL;
	for (unsigned long long DataPcs = 0; DataPcs < DataLen;)
	{
		send(server, fBuffer, sizeof(unsigned long long), 0);
		if ((DataPcs + Pkg_Leng) > DataLen)
			latestpkg = DataLen - DataPcs - 1;
		else latestpkg = Pkg_Leng;
		if (recv(server, fBuffer, latestpkg, 0) < 0)
		{
			Failed_Counter++;
			if (Failed_Counter > 100ULL) return -1;
		}
		else
		{
			Failed_Counter = 0ULL;
		}
		memcpy(&*_Data + DataPcs, fBuffer, latestpkg);
		DataPcs += Pkg_Leng;
	}
	return 1;
}

void ykshow::ShowThread()
{
	Working = 1UL;
	CDC* pDC = GetDlgItem(IDC_PicCtrl)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	//初始化WSA    
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		MessageBoxA("Startup error", 0, 0);
		Working = 0UL;
		return;
	}

	//创建套接字    
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		MessageBoxA("socket error !",0,0);
		Working = 0UL;
		return;
	}

	//绑定IP和端口    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(S_Port - 1);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		MessageBoxA("bind error !", 0, 0);
		Working = 0UL;
		return;
	}

	//开始监听    
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		MessageBoxA("listen error !", 0, 0);
		Working = 0UL;
		return;
	}

	//循环接收数据    
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
	if (sClient == INVALID_SOCKET)
	{
		MessageBoxA("accept error !",0,0);
		Working = 0UL;
		return;
	}
	unsigned long long mcBuffer = 0ULL;
	unsigned long Error_Count = 0UL;
	char bufferArea[255];
	shared_ptr<char> _PicShow;

	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_PicCtrl);//IDC_picture为picture控件ID
	pWnd->GetClientRect(&rect);//rc为控件的大小。
	unsigned PicScy = rect.Height();
	unsigned PicScx = rect.Width();
	CString CSTRBuf;
	GetDlgItemTextA(IDC_EDIT1, CSTRBuf);
	string SStr = CSTRBuf.GetString();
	if (SStr != string(""))
	{
		stringstream NxSS;
		NxSS << SStr;
		NxSS >> mcBuffer;
	}

	send(sClient, (char*)&mcBuffer, sizeof(unsigned long long), 0);

	while (*ShowFlag)
	{
		if (recv(sClient, (char*)&mcBuffer, sizeof(unsigned long long), 0) < 0);
		_PicShow.reset(new char[mcBuffer]);
		PicSize = mcBuffer;
		if (recvimg(sClient, _PicShow, mcBuffer) < 0);
		if (DoBuffer)
		{
			mcBuffer = 2ULL;
			send(sClient, (char*)&mcBuffer, sizeof(unsigned long long),0);
			if (DoBuffer == Mou_Move)
			{
				send(sClient, (char*)&DoBuffer, sizeof(unsigned long long), 0);
				send(sClient, (char*)&Moux, sizeof(unsigned long long), 0);
				send(sClient, (char*)&Mouy, sizeof(unsigned long long), 0);
			}
			else
			{
				send(sClient, (char*)&DoBuffer, sizeof(unsigned long long), 0);
			}
			DoBuffer = 0UL;
		}
		else
		{
			if (*ShowFlag)
				mcBuffer = 1ULL;
			else
				mcBuffer = 0ULL;
			send(sClient, (char*)&mcBuffer, sizeof(unsigned long long), 0);
		}
		if (GetPicShot)
		{
			GetPicShot = 0UL;
			ofstream shotpath("screenshot.bmp", ios::out | ios::binary);
			if (shotpath)
			{
				shotpath.write((char*)&*_PicShow, PicSize);
				shotpath.close();
			}
		}
		try {
			ShowDib(hDC, (BITMAPFILEHEADER *)(&*_PicShow), PicScx, PicScy, SHOW_STRETCH);
		}
		catch (...)
		{

		}
	}
	
	closesocket(sClient);
	WSACleanup();
	Working = 0UL;
	return;
}


BOOL ykshow::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作

	//AdjustWindowsSize();    //调整窗口大小与桌面工作区相同
	//ReSize();                //调整当前窗口中所有控件大小，以适应当前屏幕分辨率
	SetDlgItemText(IDC_EDIT1, "1000");
	return 1;
}

void ykshow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ykshow, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ykshow::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ykshow::OnBnClickedButton2)
	ON_STN_CLICKED(IDC_PicCtrl, &ykshow::OnStnClickedPicctrl)
	ON_BN_CLICKED(IDC_BUTTON3, &ykshow::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &ykshow::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &ykshow::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON8, &ykshow::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON6, &ykshow::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &ykshow::OnBnClickedButton7)
END_MESSAGE_MAP()


// ykshow 消息处理程序


void ykshow::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowFlag.reset(new unsigned long);
	*ShowFlag = 1UL;
	NxThread.push(thread(&ykshow::ShowThread, this));
	NxThread.top().detach();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	return;
}


void ykshow::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	auto Fxs = [&]() {
		if (ShowFlag)
			*ShowFlag = 0UL;
		auto Time_Ofs = chrono::system_clock::now();
		while (Working && ((chrono::system_clock::now() - Time_Ofs) < chrono::seconds(10)))
			Sleep(100);
		while (!NxThread.empty())NxThread.pop();
		closesocket(slisten);
		BOOL  bDontLinger = FALSE;
		setsockopt(slisten, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
		WSACleanup();
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BUTTON1, "监听");
	};
	SetDlgItemText(IDC_BUTTON1, "等待");
	thread(Fxs).detach();
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
}


BOOL ykshow::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (KeyBoardHook)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			DoBuffer = pMsg->wParam;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void ykshow::OnStnClickedPicctrl()
{
	// TODO: 在此添加控件通知处理程序代码
	CRect rect_ctr;
	(this->GetDlgItem(IDC_PicCtrl))->GetWindowRect(&rect_ctr);   //获取IDC_STATIC控件相对屏幕左上角的坐标，/存储到rect_ctr中  
	//point.x += rect_ctr.left; //在屏幕上的绝对坐标  
	//point.y += rect_ctr.top; //  
	CRect rect;
	GetDlgItem(IDC_PicCtrl)->GetClientRect(&rect);//rc为控件的大小。
	int PicScy = rect.Height();
	int PicScx = rect.Width();
	CPoint   point;
	GetCursorPos(&point);
	double analX = point.x - rect_ctr.left;
	double analY = point.y - rect_ctr.top;
	analX *= Max_WSc;
	analY *= Max_HSc;
	analX /= double(PicScx);
	analY /= double(PicScy);
	Moux = unsigned long long(analX);
	Mouy = unsigned long long(analY);
	DoBuffer = Mou_Move;
	return;
}


void ykshow::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	DoBuffer = Mou_L;
}


void ykshow::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	DoBuffer = Mou_DbC;
}


void ykshow::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	DoBuffer = Mou_R;
}


void ykshow::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!Working)
	{
		MessageBoxA("请先开启监听。", 0, 0U);
		return;
	}
	CString CSTRBuf;
	CallBackEnv->GetDlgItemTextA(IDC_EDIT1, CSTRBuf);
	string SStr = CSTRBuf.GetString();
	unsigned long long SzNID = 1ULL;
	if (SStr != string(""))
	{
		stringstream NxSS;
		NxSS << SStr;
		NxSS >> SzNID;
	}
	else
	{
		MessageBox("出现了微小的问题\n请指派红军执行命令！", "不符合基本法", 0U);
		return;
	}
	auto Pcs_Com = [&](unsigned long long UID, ChisfooDlg::CLData userData)
	{
		if (CallBackEnv->WorkMap.find(UID) == CallBackEnv->WorkMap.end())
		{
			CallBackEnv->WorkMap.insert(make_pair(UID, list<ChisfooDlg::CLData>()));
		}
		CallBackEnv->WorkMap[UID].push_back(userData);
	};
	ChisfooDlg::CLData SzCDATA;
	SzCDATA.Method = Remote_Screen;
	if (SzNID == 0ULL)
	{
		for (auto UListTor = CallBackEnv->userList.begin(); UListTor != CallBackEnv->userList.end(); UListTor++)
		{
			Pcs_Com(UListTor->CID, SzCDATA);
		}
	}
	else
	{
		Pcs_Com(SzNID, SzCDATA);
	}
	CallBackEnv->MessagePcs("成功添加远程屏幕相关命令...");
	return;
}


void ykshow::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	if (KeyBoardHook)
	{
		SetDlgItemText(IDC_BUTTON6, "传输键盘");
		KeyBoardHook = 0UL;
	}
	else
	{
		SetDlgItemText(IDC_BUTTON6, "取消捕获");
		KeyBoardHook = 1UL;
	}
}


void ykshow::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	GetPicShot = 1UL;
}
