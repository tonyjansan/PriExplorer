// stdafx.cpp : source file that includes just the standard includes
// LoginInterface.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
FILE* commonOpenFile(const CommonChar* path, const CommonChar* flag) {
	FILE* ret = NULL;
	_tfopen_s(&ret, path, flag);
	return ret;
}