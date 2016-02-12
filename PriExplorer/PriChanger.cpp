#include "stdafx.h"
#include "PriChanger.h"

long getFileSize(FILE* f) {
	long now = ftell(f), ret = 0L;
	fseek(f, 0L, SEEK_END);
	ret = ftell(f);
	fseek(f, now, SEEK_SET);
	return ret;
}

PriChanger::PriChanger(PriEntry* pEntry) : mEntry(pEntry)
{
}

PriChanger::~PriChanger()
{
	mEntry = NULL;
}

void PriChanger::changeBackGroundJpg(const CommonChar* jpgPath) {
	if (!mEntry || !jpgPath)
		return;

	FILE* fp = commonOpenFile(jpgPath, CommonCharMap("rb"));
	if (!fp) return;

	int fileSize = (int) getFileSize(fp);
	if (fileSize <= 0) {
		fclose(fp);
		return;
	}

	unsigned char* pJpgData = new unsigned char[fileSize];
	fileSize = fread(pJpgData, sizeof(unsigned char), fileSize, fp);
	fclose(fp);

	mEntry->changeBackGroundJpg(pJpgData, fileSize);
	delete[] pJpgData; pJpgData = NULL;
}

bool PriChanger::saveAs(const CommonChar* path) {
	if (!mEntry || !path)
		return false;

	FILE* fp = commonOpenFile(path, CommonCharMap("wb"));
	if (!fp) return false;

	unsigned int length = 0;
	unsigned char* pBuffer = mEntry->getMapData(&length);
	fwrite(pBuffer, sizeof(unsigned char), length, fp);
	fclose(fp);
	return true;
}