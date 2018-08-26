// moreopt.cpp : 实现文件
//

#include "stdafx.h"
#include "hisfoo.h"
#include "moreopt.h"
#include "afxdialogex.h"

/*
*                    GNU GENERAL PUBLIC LICENSE
*                       Version 3, 29 June 2007
*
* Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
* Everyone is permitted to copy and distribute verbatim copies
* of this license document, but changing it is not allowed.
* 
* Saurik Remote Control Software V3 is under a GPLv3 Licence. You can share or change those code under licence.
*/

// moreopt 对话框

IMPLEMENT_DYNAMIC(moreopt, CDialogEx)

moreopt::moreopt(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

moreopt::~moreopt()
{
}

void moreopt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(moreopt, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &moreopt::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &moreopt::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &moreopt::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &moreopt::OnBnClickedButton4)
END_MESSAGE_MAP()


// moreopt 消息处理程序


void moreopt::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	shared_ptr<filepcs> _FilePcsD(new filepcs);
	_FilePcsD->CallBackEnv = CallBackEnv;
	CallBackEnv->_file_trs_dlg = _FilePcsD;
	thread(&filepcs::DoModal, _FilePcsD).detach();
}


void moreopt::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	shared_ptr<kbpanel> _kbPcsD(new kbpanel);
	_kbPcsD->CallBackEnv = CallBackEnv;
	thread(&kbpanel::DoModal, _kbPcsD).detach();
}


void moreopt::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	shared_ptr<autorunctrl> _AutoRunPcsD(new autorunctrl);
	_AutoRunPcsD->CallBackEnv = CallBackEnv;
	thread(&autorunctrl::DoModal, _AutoRunPcsD).detach();
}


void moreopt::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	shared_ptr<ykshow> _Rmc(new ykshow);
	_Rmc->CallBackEnv = CallBackEnv;
	thread(&autorunctrl::DoModal, _Rmc).detach();
}
