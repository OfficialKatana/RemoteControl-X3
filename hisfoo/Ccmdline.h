#pragma once


#include "hisfooDlg.h"
#include "afxwin.h"

// Ccmdline 对话框
class ChisfooDlg;


class Ccmdline : public CDialogEx
{
	DECLARE_DYNAMIC(Ccmdline)

public:
	Ccmdline(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Ccmdline();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIddDialog1();
	afx_msg void OnUpdateIddDialog1(CCmdUI *pCmdUI);
	void AddStr(string _str);
	shared_ptr<unsigned int> is_ok;
	ChisfooDlg* tDlg;
	string szStr = "Valid Member";
	unsigned long long CID;
	afx_msg void OnBnClickedButton1();
	CEdit pedit;
};
