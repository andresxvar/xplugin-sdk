#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "hook.h"

Hook::Hook(unsigned int oldPtr, unsigned int newPtr)
{
	mOldPtr = oldPtr;
	mNewPtr = newPtr;

	memcpy( mOriginalBytes, (void *)oldPtr, 6 );
}

qboolean Hook::Patch()
{
	unsigned int calcAdr;
	unsigned char newBytes[6];
	unsigned long dwOldProtect = 0;

	if (VirtualProtect((void*)mOldPtr, 6,
		PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		newBytes[0] = 0xE9; // jmp
		calcAdr = mNewPtr - (mOldPtr + 5);
		memcpy(&newBytes[1], &calcAdr, 4);
		newBytes[5] = 0xC3; // retn
		memcpy((void*)mOldPtr, &newBytes, 6);
		VirtualProtect((void*)mOldPtr, 6, dwOldProtect, &dwOldProtect);
		return qtrue;
	}
	else
		return qfalse;
}

qboolean Hook::RemovePatch()
{
	unsigned long dwOldProtect = NULL;

	if (VirtualProtect((void*)mOldPtr, 6,
		PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		memcpy((void *)mOldPtr, mOriginalBytes, 6 );
		VirtualProtect((void*)mOldPtr, 6, dwOldProtect, &dwOldProtect);
		return qtrue;
	}
	else
		return qfalse;
}