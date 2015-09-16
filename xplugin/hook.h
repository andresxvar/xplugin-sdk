#pragma once
#include "local.h"

// Hooking library class
class Hook
{
public:
	Hook(unsigned int oldPtr, unsigned int newPtr);
	unsigned int GetAddress(){return mOldPtr;};
	qboolean Patch();
	qboolean RemovePatch();

private:
	unsigned int	mOldPtr;	//	The code we're patching
	unsigned int	mNewPtr;	//	Function to direct the control flow into
	unsigned char	mOriginalBytes[6];
};