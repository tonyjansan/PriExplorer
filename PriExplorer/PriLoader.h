#ifndef __PRI_LOADER_H__
#define __PRI_LOADER_H__

#include "PriEntry.h"

class PriLoader
{
public:
	PriLoader() {}
	virtual ~PriLoader() = 0 {};
	PriEntry* getPriEntry() { return &mEntry; }
	unsigned int getDataByIndex(unsigned char* buffer, unsigned int length, const unsigned int index);
	unsigned int dumpDataFiles(const CommonChar* path, const unsigned short tag);
protected:
	PriEntry mEntry;
};

class PriFileLoader : public PriLoader
{
public:
	PriFileLoader(const CommonChar* path);
	virtual ~PriFileLoader();
};

#endif // __PRI_LOADER_H__