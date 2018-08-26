#pragma once
#include "resource.h"		// 主符号
#include "moreopt.h"
#include <list>
#include "afxwin.h"
class moreopt;
class ChisfooDlg;

// filepcs 对话框

class filepcs : public CDialogEx
{
	DECLARE_DYNAMIC(filepcs)

public:
	filepcs(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~filepcs();
	void Change_File_Data(vector<string> FileList, vector<string> DirList);
	ChisfooDlg* CallBackEnv = NULL;
	//void Change_File_Data(list<string> DirList, list<string> FileList);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	// 文件夹路径
	CListBox folderpathlist;
	// 文件列表
	CListBox filelistbox;
	afx_msg void OnLbnDblclkList2();
	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnLbnDblclkList3();
	void _GoShell(unsigned long long CID, string Shell);
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton7();
};
