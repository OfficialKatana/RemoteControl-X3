// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

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

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <winsock2.h>    

#pragma comment(lib,"ws2_32.lib")    

// TODO: 在此处引用程序需要的其他头文件
#include <iostream>
using namespace std;
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <future>
#include <cstdlib>
#include <chrono>
#include <memory>
#include <list>
#include <vector>
#include <functional>