#pragma once
#include "resource.h"		// 主符号
#include "moreopt.h"
class moreopt;
class ChisfooDlg;

// kbpanel 对话框

class kbpanel : public CDialogEx
{
	DECLARE_DYNAMIC(kbpanel)

public:
	kbpanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~kbpanel();
	ChisfooDlg* CallBackEnv = NULL;
	void SetupFunc(unsigned long long Mode);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
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
