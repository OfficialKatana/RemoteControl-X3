// KeyBGeko.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "KeyBGeko.h"


// 这是导出变量的一个示例
KEYBGEKO_API int nKeyBGeko=0;

// 这是导出函数的一个示例。
KEYBGEKO_API int fnKeyBGeko(void)
{
    return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 KeyBGeko.h
CKeyBGeko::CKeyBGeko()
{
    return;
}
