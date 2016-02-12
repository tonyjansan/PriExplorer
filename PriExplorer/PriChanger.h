#ifndef __PRI_CHANGER_H__
#define __PRI_CHANGER_H__

#include "PriEntry.h"

class PriChanger
{
public:
	PriChanger(PriEntry* pEntry);
	~PriChanger();
	void changeBackGroundJpg(const CommonChar* jpgPath);
	bool saveAs(const CommonChar* path);
private:
	PriEntry* mEntry;
};

#endif // __PRI_CHANGER_H__

