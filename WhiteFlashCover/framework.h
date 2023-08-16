// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers


#define TESTBUTTONS_OFFSET 400
#define OFFSET_FLASHCOVER 90
#define TB_OFFSET 320
#define BTN_SPACING_Y 20
#define FLASH_SPACING_Y 20
#define TB_SPACING_Y 35




#include "targetver.h"
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>



#include <typeinfo>
#include <iostream>
#include <oleacc.h>	// AccessibleObjectFromEvent
#include <oaidl.h>	// VARIANT
#include <string>
//#include <string_view>
#include <atlstr.h>
#include <signal.h>
#include <thread>
#include <vector>
//#include <unistd.h>
//#include <format>
#include <map>

#include <stdio.h>
#include <stdlib.h>

#include <sstream>

#include <wingdi.h>	// SetDCBrushColor, GetStockObject

#include <shellapi.h>	// NOTIFYICONDATA

#pragma comment (lib, "Dwmapi")
#include "dwmapi.h"
#include <Gdiplus.h>

#include <strsafe.h>
#include <shlwapi.h>

using namespace std;
using std::ostringstream;
using std::ostream;
