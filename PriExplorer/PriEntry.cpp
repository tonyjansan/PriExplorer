#include "stdafx.h"
#include "PriEntry.h"

#define alignSize(size, align) ((size + align - 1) / align * align)

const char* PriEntry::sTag = "mrm_pri2";
const unsigned int PriEntry::sFileLengthTag = 0xDEFFFADE;
const unsigned int PriEntry::sDataItemLengthTag = 0xDEF5FADE;

PriEntry::PriEntry()
{
	mMapData = new unsigned char[MAP_BUFFER_SIZE];
	memset(mMapData, 0, MAP_BUFFER_SIZE);
}

PriEntry::~PriEntry()
{
	if (mDataInfo.mPriBlobs) {
		delete[] mDataInfo.mPriBlobs;
		mDataInfo.mPriBlobs = NULL;
	}
	delete[] mMapData;
	mMapData = NULL;
}

unsigned char* PriEntry::getMapData(unsigned int* length)
{
	*length = mMapData ? mDataInfo.mTotalLength : 0;
	return mMapData;
}
PriBlob* PriEntry::getBlobs(unsigned int* count)
{
	*count = mDataInfo.mBlobCount;
	return mDataInfo.mPriBlobs;
}

unsigned int PriEntry::getDataByBlob(unsigned char* buffer, unsigned int length, const PriBlob* pBlob)
{
	if (!mMapData || !buffer || length <= 0 || !pBlob)
		return 0;

	unsigned int size = length < pBlob->length ? length : pBlob->length;
	memcpy(buffer, (unsigned char*)mMapData + mDataInfo.mDataOffset + pBlob->offset, size);
	return size;
}

void PriEntry::setMapFile(FILE* file)
{
	mMapFile = file;
	if (!mMapFile || !mMapData)
		return;

	fseek(mMapFile, 0x0CL, SEEK_SET);
	fread(&mDataInfo.mTotalLength, sizeof(unsigned int), 1, mMapFile);
	fseek(mMapFile, 0L, SEEK_SET);
	fread(mMapData, sizeof(unsigned char), mDataInfo.mTotalLength, mMapFile);

	memcpy(&mDataInfo.mTotalLength, mMapData + 0x0C, sizeof(unsigned int));

	// Read Header Structure
	memcpy(&mDataInfo.mHeaderLength, mMapData + 0x14, sizeof(unsigned int));
	memcpy(&mDataInfo.mDataItemOffset, mMapData + 0xB8, sizeof(unsigned int));
	memcpy(&mDataInfo.mDataItemLength, mMapData + 0xBC, sizeof(unsigned int));

	// Read DataItem Structure
	int position = mDataInfo.mHeaderLength + mDataInfo.mDataItemOffset + 0x24;
	memcpy(&mDataInfo.mStringCount, mMapData + position, sizeof(unsigned short));
	position += sizeof(unsigned short);
	memcpy(&mDataInfo.mBlobCount, mMapData + position, sizeof(unsigned short));
	position += sizeof(unsigned short);
	memcpy(&mDataInfo.mDataLength, mMapData + position, sizeof(unsigned int));

	mDataInfo.mBlobOffset = mDataInfo.mHeaderLength + mDataInfo.mDataItemOffset
		+ 0x2C + mDataInfo.mStringCount * sizeof(unsigned int);
	mDataInfo.mDataOffset = mDataInfo.mBlobOffset + mDataInfo.mBlobCount * sizeof(unsigned int) * 2;

	// Read Blob Structure
	mDataInfo.mPriBlobs = NULL;
	if (mDataInfo.mBlobCount > 0) {
		mDataInfo.mPriBlobs = new PriBlob[mDataInfo.mBlobCount];
		for (int i = 0; i < mDataInfo.mBlobCount; i++) {
			position = mDataInfo.mBlobOffset + i * sizeof(unsigned int) * 2;
			memcpy(&mDataInfo.mPriBlobs[i].offset, mMapData + position, sizeof(unsigned int));
			position += sizeof(unsigned int);
			memcpy(&mDataInfo.mPriBlobs[i].length, mMapData + position, sizeof(unsigned int));
			position = mDataInfo.mDataOffset + mDataInfo.mPriBlobs[i].offset;
			memcpy(&mDataInfo.mPriBlobs[i].tag, mMapData + position, sizeof(unsigned short));
		}
	}
}

void PriEntry::changeBackGroundJpg(unsigned char* jpgData, unsigned int length) {
	if (!mMapData || !jpgData)
		return;

	unsigned int lengthAlign = alignSize(length, 8);
	unsigned int jpgOffset = mDataInfo.mDataLength;
	unsigned int pos = mDataInfo.mHeaderLength + mDataInfo.mDataItemOffset + 0x18;

	// Change Header Structure
	mDataInfo.mTotalLength += lengthAlign;
	memcpy(mMapData + 0x0C, &mDataInfo.mTotalLength, sizeof(unsigned int));
	mDataInfo.mDataItemLength += lengthAlign;
	memcpy(mMapData + 0xBC, &mDataInfo.mDataItemLength, sizeof(unsigned int));

	// Change DataItem Structure
	memcpy(mMapData + pos, &mDataInfo.mDataItemLength, sizeof(unsigned int));
	mDataInfo.mDataLength += lengthAlign;
	memcpy(mMapData + pos + 0x10, &mDataInfo.mDataLength, sizeof(unsigned int));

	// Change Blob Structure
	for (unsigned int i = 0; i < mDataInfo.mBlobCount; i++) {
		if (mDataInfo.mPriBlobs[i].tag != 0xD8FF)
			continue;

		mDataInfo.mPriBlobs[i].offset = jpgOffset;
		mDataInfo.mPriBlobs[i].length = length;
		pos = mDataInfo.mBlobOffset + i * sizeof(unsigned int) * 2;
		memcpy(mMapData + pos, &jpgOffset, sizeof(unsigned int));
		memcpy(mMapData + pos + sizeof(unsigned int), &length, sizeof(unsigned int));
	}

	// Append Jpg Data
	pos = mDataInfo.mDataOffset + jpgOffset;
	memcpy(mMapData + pos, jpgData, length);

	// Rebuild Footer Structure
	pos += lengthAlign;
	memcpy(mMapData + pos, &sDataItemLengthTag, sizeof(unsigned int));
	pos += sizeof(unsigned int);
	memcpy(mMapData + pos, &mDataInfo.mDataItemLength, sizeof(unsigned int));
	pos += sizeof(unsigned int);
	memcpy(mMapData + pos, &sFileLengthTag, sizeof(unsigned int));
	pos += sizeof(unsigned int);
	memcpy(mMapData + pos, &mDataInfo.mTotalLength, sizeof(unsigned int));
	pos += sizeof(unsigned int);
	memcpy(mMapData + pos, sTag, 8);
}