// autorunctrl.cpp : 实现文件
//

#include "stdafx.h"
#include "hisfoo.h"
#include "autorunctrl.h"
#include "afxdialogex.h"


// autorunctrl 对话框

IMPLEMENT_DYNAMIC(autorunctrl, CDialogEx)

autorunctrl::autorunctrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG5, pParent)
{

}

autorunctrl::~autorunctrl()
{
}

void autorunctrl::UseFunction(unsigned long long Mode)
{
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
	SzCDATA.Method = Auto_Run;
	shared_ptr<byte>_pUmode(new byte[sizeof(unsigned long long)]);
	memcpy(&*_pUmode, (char*)&Mode, sizeof(unsigned long long));
	SzCDATA.Data.push_back(_pUmode);
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
	CallBackEnv->MessagePcs("成功添加键盘相关命令...");
	return;
}

void autorunctrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(autorunctrl, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &autorunctrl::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &autorunctrl::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &autorunctrl::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &autorunctrl::OnBnClickedButton4)
END_MESSAGE_MAP()


// autorunctrl 消息处理程序


void autorunctrl::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UseFunction(1ULL);
}


void autorunctrl::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UseFunction(2ULL);
}


void autorunctrl::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	UseFunction(3ULL);
}


void autorunctrl::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBoxA("使用说明:\n"\
"如果客户端从未安装过，需要先安装。\n"\
"如果安装过，但是未启动（首次安装后会自动启动）则重新启动服务\n"\
"如果不再需要自启动服务，则卸载服务。", "膜法传奇", 0U);
}
