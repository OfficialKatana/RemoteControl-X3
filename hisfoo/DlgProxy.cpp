
// DlgProxy.cpp : 实现文件
//

#include "stdafx.h"
#include "hisfoo.h"
#include "DlgProxy.h"
#include "hisfooDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChisfooDlgAutoProxy

IMPLEMENT_DYNCREATE(ChisfooDlgAutoProxy, CCmdTarget)

ChisfooDlgAutoProxy::ChisfooDlgAutoProxy()
{
	EnableAutomation();
	
	// 为使应用程序在自动化对象处于活动状态时一直保持 
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(ChisfooDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(ChisfooDlg)))
		{
			m_pDialog = reinterpret_cast<ChisfooDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

ChisfooDlgAutoProxy::~ChisfooDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void ChisfooDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(ChisfooDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(ChisfooDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_Ihisfoo 的支持
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {04A76836-E4B7-4BB8-8FF2-AEC5DD5AA23C}
static const IID IID_Ihisfoo =
{ 0x4A76836, 0xE4B7, 0x4BB8, { 0x8F, 0xF2, 0xAE, 0xC5, 0xDD, 0x5A, 0xA2, 0x3C } };

BEGIN_INTERFACE_MAP(ChisfooDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(ChisfooDlgAutoProxy, IID_Ihisfoo, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {E15DCE55-7DFD-418F-8A09-960A02D1E97E}
IMPLEMENT_OLECREATE2(ChisfooDlgAutoProxy, "hisfoo.Application", 0xe15dce55, 0x7dfd, 0x418f, 0x8a, 0x9, 0x96, 0xa, 0x2, 0xd1, 0xe9, 0x7e)


// ChisfooDlgAutoProxy 消息处理程序
