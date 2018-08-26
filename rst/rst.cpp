// rst.cpp : 定义 DLL 的初始化例程。
//

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

#include "stdafx.h"
#include "rst.h"

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CrstApp

BEGIN_MESSAGE_MAP(CrstApp, CWinApp)
END_MESSAGE_MAP()


// CrstApp 构造

CrstApp::CrstApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CrstApp 对象

CrstApp theApp;


// CrstApp 初始化

BOOL CrstApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}



Pic GetImg(unsigned long imgwidth, unsigned long imgheight)
{
	CWnd cwnd;
	CDC *pdeskdc = cwnd.GetDesktopWindow()->GetDC();
	CRect re;
	//获取窗口的大小  
	cwnd.GetDesktopWindow()->GetClientRect(&re);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());
	//创建一个兼容的内存画板  
	CDC memorydc;
	memorydc.CreateCompatibleDC(pdeskdc);

	//选中画笔  
	CBitmap *pold = memorydc.SelectObject(&bmp);

	//绘制图像  
	memorydc.BitBlt(0, 0, re.Width(), re.Height(), pdeskdc, 0, 0, SRCCOPY);

	//获取鼠标位置，然后添加鼠标图像  
	CPoint po;
	GetCursorPos(&po);
	HICON hinco = (HICON)GetCursor();
	memorydc.DrawIcon(po.x - 10, po.y - 10, hinco);
	//选中原来的画笔  
	memorydc.SelectObject(pold);
	BITMAP bit;
	bmp.GetBitmap(&bit);
	//  DWORD size = bit.bmWidth * bit.bmHeight ;  

	//定义 图像大小（单位：byte）  
	DWORD size = bit.bmWidthBytes * bit.bmHeight;
	LPSTR lpdata = (LPSTR)GlobalAlloc(GPTR, size);

	//后面是创建一个bmp文件的必须文件头，想要了解可以参考msdn  

	BITMAPINFOHEADER pbitinfo;
	pbitinfo.biBitCount = 24;
	pbitinfo.biClrImportant = 0;
	pbitinfo.biCompression = BI_RGB;
	pbitinfo.biHeight = bit.bmHeight;
	pbitinfo.biPlanes = 1;
	pbitinfo.biSize = sizeof(BITMAPINFOHEADER);
	pbitinfo.biSizeImage = size;
	pbitinfo.biWidth = bit.bmWidth;
	pbitinfo.biXPelsPerMeter = 0;
	pbitinfo.biYPelsPerMeter = 0;

	GetDIBits(pdeskdc->m_hDC, bmp, 0, pbitinfo.biHeight, lpdata,
		(BITMAPINFO*)&pbitinfo, DIB_RGB_COLORS);

	BITMAPFILEHEADER bfh;
	bfh.bfReserved1 = bfh.bfReserved2 = 0;
	bfh.bfType = ((WORD)('M' << 8) | 'B');
	bfh.bfSize = 54 + size;
	bfh.bfOffBits = 54;
	// 开始图像处理！
	BITMAPFILEHEADER HeadBuffer;
	BITMAPINFOHEADER InfoHeadBuffer;
	// IMG高度
	unsigned long MYDRAW_HEIGHT = imgheight;
	unsigned long MYDRAW_WIDTH = imgwidth;
	BITMAPFILEHEADER writebitHead;
	BITMAPINFOHEADER writebitInfoHead;
	writebitHead = bfh;
	writebitInfoHead = pbitinfo;
	writebitInfoHead.biHeight = MYDRAW_HEIGHT;//为截取文件重写位图高度    
	writebitInfoHead.biWidth = MYDRAW_WIDTH;//为截取文件重写位图宽度    
	int mywritewidth = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//BMP图像实际位图数据区的宽度为4byte的倍数，在此计算实际数据区宽度    
	writebitInfoHead.biSizeImage = mywritewidth*writebitInfoHead.biHeight;//计算位图实际数据区大小    

	writebitHead.bfSize = 54 + writebitInfoHead.biSizeImage;//位图文件头大小为位图数据区大小加上54byte
	HeadBuffer = writebitHead;
	InfoHeadBuffer = writebitInfoHead;

	int width = pbitinfo.biWidth;
	int height = pbitinfo.biHeight;
	//分配内存空间把源图存入内存       
	int l_width = WIDTHBYTES(width*pbitinfo.biBitCount);//计算位图的实际宽度并确保它为4byte的倍数    
	int write_width = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//计算写位图的实际宽度并确保它为4byte的倍数    

	unique_ptr<BYTE>pColorDataMid ((BYTE *)malloc(mywritewidth*MYDRAW_HEIGHT));//开辟内存空间存储图像处理之后数据    
	memset(&*pColorDataMid, 0, mywritewidth*MYDRAW_HEIGHT);

	long nData = height*l_width;
	long write_nData = mywritewidth*MYDRAW_HEIGHT;//截取的位图数据区长度定义
	BYTE* pColorData = (BYTE*)lpdata;
	for (int hnum = 0; hnum < MYDRAW_HEIGHT; hnum++)
		for (int wnum = 0; wnum < MYDRAW_WIDTH; wnum++)
		{
			double d_original_img_hnum = hnum*height / (double)MYDRAW_HEIGHT;
			double d_original_img_wnum = wnum*width / (double)MYDRAW_WIDTH;
			int i_original_img_hnum = d_original_img_hnum;
			int i_original_img_wnum = d_original_img_wnum;
			double distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//在原图像中与a点的水平距离    
			double distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//在原图像中与a点的垂直距离    

			int original_point_a = i_original_img_hnum*l_width + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点A      
			int original_point_b = i_original_img_hnum*l_width + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点B    
			int original_point_c = (i_original_img_hnum + 1)*l_width + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点C     
			int original_point_d = (i_original_img_hnum + 1)*l_width + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点D     
			if (i_original_img_hnum + 1 == MYDRAW_HEIGHT - 1)
			{
				original_point_c = original_point_a;
				original_point_d = original_point_b;
			}
			if (i_original_img_wnum + 1 == MYDRAW_WIDTH - 1)
			{
				original_point_b = original_point_a;
				original_point_d = original_point_c;
			}

			int pixel_point = hnum*write_width + wnum * 3;//映射尺度变换图像数组位置偏移量    
			(&*pColorDataMid)[pixel_point] =
				pColorData[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c] * distance_to_a_y*distance_to_a_x;
			(&*pColorDataMid)[pixel_point + 1] =
				pColorData[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c + 1] * distance_to_a_y*distance_to_a_x;
			(&*pColorDataMid)[pixel_point + 2] =
				pColorData[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c + 2] * distance_to_a_y*distance_to_a_x;

		}
	/*******************双线性插值******************/
	/*******************图像处理部分******************/
	//释放内存
	GlobalFree(lpdata);
	Pic NxData;
	NxData.imagelong = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + write_nData;
	shared_ptr<char> Bmp(new char[sizeof(BITMAPFILEHEADER)+ sizeof(BITMAPINFOHEADER)+ write_nData]);
	memcpy(&*Bmp, &HeadBuffer, sizeof(BITMAPFILEHEADER));
	memcpy(&*Bmp + sizeof(BITMAPFILEHEADER), &InfoHeadBuffer, sizeof(BITMAPINFOHEADER));
	memcpy(&*Bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), &*pColorDataMid, write_nData);
	NxData.imagedata = Bmp;
	return move(NxData);
}

#include "../hisfans/ykdef.h"

int sendimg(SOCKET client, shared_ptr<char> _Data, unsigned long long DataLen)
{
	char fBuffer[Pkg_Leng];
	unsigned long long latestpkg = 0ULL;
	for (unsigned long long DataPcs = 0; DataPcs < DataLen;)
	{
		if ((DataPcs + Pkg_Leng) > DataLen)
			latestpkg = DataLen - DataPcs;
		else latestpkg = Pkg_Leng;
		memcpy(fBuffer, &*_Data + DataPcs, latestpkg);
		send(client, &*_Data, latestpkg, 0);
		recv(client, (char*)&latestpkg, sizeof(unsigned long long), 0);
		DataPcs += Pkg_Leng;
	}
	return 1;
}

string Get_Domain_IP()
{
	hostent *phst = gethostbyname(Server_Host);
	in_addr * iddr = (in_addr*)phst->h_addr;
	unsigned long IPUL = iddr->s_addr;
	return move(string(inet_ntoa(*iddr)));
}

extern "C" _declspec (dllexport) BOOL NxRms()
{
	//初始化网络环境  
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup failed\n");
		return 0;
	}
	// 初始化完成，创建一个TCP的socket  
	SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sServer == INVALID_SOCKET)
	{
		printf("socket failed\n");
		return 0;
	}
	//指定连接的服务端信息  
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(S_Port - 1);
	//客户端只需要连接指定的服务器地址，127.0.0.1是本机的回环地址  
	addrServ.sin_addr.S_un.S_addr = inet_addr((Get_Domain_IP()).c_str());

	// 服务器Bind 客户端是进行连接  
	int ret = connect(sServer, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));//开始连接  
	if (SOCKET_ERROR == ret)
	{
		printf("socket connect failed\n");
		WSACleanup();
		closesocket(sServer);
		return 0;
	}
	unsigned long long uxBuffer = 1;
	// YK Start Screen Monitoring
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	unsigned long Failed_Counter;
	unsigned long long SleepTime = 0ULL;
	recv(sServer, (char*)&SleepTime, sizeof(unsigned long long), 0);
	while (uxBuffer)
	{
		auto FxImg = GetImg(480UL, 320UL);
		uxBuffer = FxImg.imagelong;
		if (send(sServer, &*(FxImg.imagedata), uxBuffer, 0) < 0)
		{
			Failed_Counter++;
			if (Failed_Counter > 10UL)break;
			continue;
		}
		if (sendimg(sServer, (FxImg.imagedata), uxBuffer) < 0)
		{
			Failed_Counter++;
			if (Failed_Counter > 10UL)break;
			continue;
		}
		else
		{
			Failed_Counter = 0UL;

			recv(sServer, (char*)&uxBuffer, sizeof(unsigned long long), 0);
			if (uxBuffer == 2ULL)
			{
				recv(sServer, (char*)&uxBuffer, sizeof(unsigned long long), 0);
				if (uxBuffer == Mou_Move)
				{
					recv(sServer, (char*)&uxBuffer, sizeof(unsigned long long), 0);
					double fW = double(cx) / Max_WSc * uxBuffer;
					recv(sServer, (char*)&uxBuffer, sizeof(unsigned long long), 0);
					double fH = cy / Max_HSc *uxBuffer;
					SetCursorPos(int(fW), int(fH));
				}
				else if (uxBuffer == Mou_L)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}
				else if (uxBuffer == Mou_R)
				{
					mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				}
				else if (uxBuffer == Mou_DbC)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}
				else if (uxBuffer)
				{
					keybd_event(uxBuffer, 0, 0, 0);
					keybd_event(uxBuffer, 0, KEYEVENTF_KEYUP, 0);
				}
			}
		}
		Sleep(DWORD(SleepTime));
	}
	closesocket(sServer);
	WSACleanup();
	return 1;
}
