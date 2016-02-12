#include "stdafx.h"
#include "PriLoader.h"

#define DUMP_PREFIX CommonCharMap("%d.jpg")

unsigned int PriLoader::getDataByIndex(unsigned char* buffer, unsigned int length, const unsigned int index)
{
	if (!mEntry.getMapFile() || !buffer || !length)
		return 0;

	unsigned int blobSize = 0;
	const PriBlob* pBlobs = mEntry.getBlobs(&blobSize);
	if (index >= blobSize)
		return 0;

	return mEntry.getDataByBlob(buffer, length, pBlobs + index);
}

unsigned int PriLoader::dumpDataFiles(const CommonChar* path, const unsigned short tag)
{
	if (!mEntry.getMapFile() || !path)
		return 0;

	CommonChar filepath[256], *name;
	CommonStringCopy(filepath, path);
	const unsigned int nLen = CommonStringNLength(filepath, 255);
	name = filepath + nLen;

	unsigned int ret = 0, blobSize = 0;
	const PriBlob* pBlobs = mEntry.getBlobs(&blobSize);
	for (unsigned int i = 0; i < blobSize; i++) {
		if (pBlobs[i].tag != tag)
			continue;

		CommonStringPrintf(name, nLen, DUMP_PREFIX, i);
		FILE* dumpFile = commonOpenFile(filepath, CommonCharMap("wb"));
		if (!dumpFile)
			continue;

		unsigned char* buffer = new unsigned char[pBlobs[i].length];
		int count = 0;
		if ((count = mEntry.getDataByBlob(buffer, pBlobs[i].length, pBlobs + i)) > 0) {
			fwrite(buffer, sizeof(unsigned char), count, dumpFile);
			delete[] buffer;
			ret++;
		}
		fclose(dumpFile);
	}
	return ret;
}

PriFileLoader::PriFileLoader(const CommonChar* path) : PriLoader()
{
	mEntry.setMapFile(commonOpenFile(path, CommonCharMap("rb")));
}

PriFileLoader::~PriFileLoader()
{
	if (mEntry.getMapFile())
		fclose(mEntry.getMapFile());
}