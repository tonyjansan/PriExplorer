#ifndef __PRI_H__
#define __PRI_H__

typedef struct tagPriBlob {
	unsigned int offset;
	unsigned int length;
	unsigned short tag;
} PriBlob, *PriBlobPointer;

typedef struct tagPriDataInfo {
	unsigned int mTotalLength;

	// Header Structure
	unsigned int mHeaderLength;
	unsigned int mDataItemOffset;
	unsigned int mDataItemLength; // 8 bytes aligned

	// DataItem Structure
	unsigned short mStringCount;
	unsigned short mBlobCount;
	unsigned int mDataLength;

	unsigned int mBlobOffset;
	unsigned int mDataOffset;

	// Blob Structure
	PriBlobPointer mPriBlobs;
} PriDataInfo, *PriDataInfoPointer;

#endif // __PRI_H__
