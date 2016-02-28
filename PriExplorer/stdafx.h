// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commdlg.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
#include <ShlObj.h>
#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define CommonChar TCHAR
#define CommonCharMap(str) _T(str)

#define CommonStringLength _tcslen
#define CommonStringNLength _tcsnlen
#define CommonStringCopy _tcscpy_s
#define CommonStringPrintf _stprintf_s

FILE* commonOpenFile(const CommonChar* path, const CommonChar* flag);