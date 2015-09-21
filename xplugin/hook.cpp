#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "hook.h"

Hook::Hook(unsigned int oldPtr, unsigned int newPtr)
{
	mOldPtr = oldPtr; // old function address
	mNewPtr = newPtr; // new function address

	memcpy( mOriginalBytes, (void *)oldPtr, 6 ); // store original bytes
}

// Patch
// write new bytes to add hook
qboolean Hook::Patch()
{
	unsigned int calcAdr;	// function body address
	unsigned char newBytes[6]; // bytes to write
	unsigned long dwOldProtect = 0;

	if (VirtualProtect((void*)mOldPtr, 6,
		PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		newBytes[0] = 0xE9; // jmp
		calcAdr = mNewPtr - (mOldPtr + 5);
		memcpy(&newBytes[1], &calcAdr, 4);
		newBytes[5] = 0xC3; // retn
		memcpy((void*)mOldPtr, &newBytes, 6); // write new bytes
		VirtualProtect((void*)mOldPtr, 6, dwOldProtect, &dwOldProtect);
		return qtrue;
	}
	else
		return qfalse; // failed
}

// RemovePatch
// write original bytes to remove hook
qboolean Hook::RemovePatch()
{
	unsigned long dwOldProtect = NULL;

	if (VirtualProtect((void*)mOldPtr, 6,
		PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		memcpy((void *)mOldPtr, mOriginalBytes, 6 ); // write original bytes
		VirtualProtect((void*)mOldPtr, 6, dwOldProtect, &dwOldProtect);
		return qtrue;
	}
	else
		return qfalse; // failed
}