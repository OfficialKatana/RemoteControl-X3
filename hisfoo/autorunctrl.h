#pragma once


// autorunctrl 对话框
#include "moreopt.h"

class autorunctrl : public CDialogEx
{
	DECLARE_DYNAMIC(autorunctrl)

public:
	autorunctrl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~autorunctrl();
	ChisfooDlg* CallBackEnv = NULL;
	void UseFunction(unsigned long long Mode);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
