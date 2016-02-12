#ifndef __PRI_ENTRY_H__
#define __PRI_ENTRY_H__

#include <stdio.h>
#include "Pri.h"

#define MAP_BUFFER_SIZE 8 * 1024 * 1024

class PriEntry
{
public:
	PriEntry();
	~PriEntry();

	PriDataInfo* getDataInfo() { return &mDataInfo; }
	FILE* getMapFile() { return mMapFile; }
	unsigned char* getMapData(unsigned int* length);
	PriBlob* getBlobs(unsigned int* count);
	unsigned int getDataByBlob(unsigned char* buffer, unsigned int length, const PriBlob* pBlob);

	void setMapFile(FILE* file);
	// TODO: void changeBlobData(unsigned char* buffer, unsigned int length, const PriBlob* pBlob);
	void changeBackGroundJpg(unsigned char* jpgData, unsigned int length);

private:
	static const char* sTag;
	static const unsigned int sFileLengthTag;
	static const unsigned int sDataItemLengthTag;

	PriDataInfo mDataInfo;
	FILE* mMapFile;
	unsigned char* mMapData;
};

#endif // __PRI_ENTRY_H__