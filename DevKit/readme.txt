The API is implemented using functions that take a version number, and
return a struct populated by functions.

The API is far from complete, and more features may be added in the future.

Currently supported APIs:


LmdApi_Get_Memory : Allocate and free memory managed by the mod.

All memory that will be touched by the mod should be created using this API, otherwise crashes or heap corruption will occur.

Most of the objects in the game dll were never designed to free memory once it was allocated, and as a result most objects use a mix of dynamically allocated memory and static data.  The solution to this was to keep track of all allocated memory and to write a safe G_Free function that will only free memory allocated by G_Alloc.

Giving the mod an object with memory allocated with alloc() will cause a memory leak if the mod tries to free that object.  calling free() on memory given by the mod will cause a heap corruption when the game later tries to free the object, or finds newly allocated memory at the same position.



LmdApi_Get_Accounts : Manipulate accounts, and register account data objects.

You can register your own data structures to be managed alongside accounts with this API.  The mod will handle loading and saving the data using the parser described in Lmd_Data_Public.h



LmdApi_Get_Professions : Early-stage access to professions.

The profession API is far from complete.  At this stage, it gives you access to the data structures of Lugormod that define the professions.

The profession data is entirely skill tree based.  You can find the skill you want to fetch by walking the tree, and calling the getValue/canSetValue/setValue functions defined for that skill.  Note that these functions may be NULL, if the skill is a category for other skills, and cannot be leveled on its own.

You may modify this structure, but doing so is unsupported.
Changing the functions pointed to by getValue/canSetValue/setValue may or may not change the skill's behavior inside the mod, depending on the skill, but they will always change the behavior of the "/skills" command.



LmdApi_Get_Entities : Access to entity manipulation.

The entity system in lugormod has gone through extensive replacements to support editing and logical entities.  Because of this, many assumptions the base game made no longer apply.  In order to ensure compatibility with the mod, the plugin should make use of the API functions getEntity and iterateEntities when fetching or listing entities.

At the moment, there is no API spawning entities that will save with the map.  This will come later.  You can create non-saving entities, however.
Attempting to manipulate the fields in gentity_t to create a savable entity will not cause that entity to save, and will probably cause crashes and other undesirable behavior.  The entity data is stored in a different location, and the fields in gentity_t are intended to point to it.

You can define your own entities, or replace existing entities, using registerSpawnableEntity.  These entities will be used by "/place", and will be able to save and load from map files.  To ensure the entities can load, you should register all entities before GAME_INIT is called for the mod dll.
When inside your entity spawn function, use reportSpawnFailure to inform the creator of the entity when the entity cannot spawn due to invalid keys/values.

As the entity structure is too large to version properly, this API will have its version changed on every change to the gentity_t or gclient_t structure, or any structure they contain.



LmdApi_Get_Commands_Auths : Check command authentication, query / add / remove authfiles.

This API lets you check if an account or player is an admin, get their rank, and add/remove authfiles from them.

Modifying the authfiles is not supported nor tested.

This API provides no means of creating your own commands or adding them to the authfile or help system.  Such an API will come later.




Usage:

The best way to connect to the API functions is to query the memory address of the lugormod dll using GetModuleHandle, then get the address to the function using GetProcAddress.

HMODULE lugormodModule = GetModuleHandle("jampgamex86.dll");
if (!lugormodModule) {
	// Error
	return;
}

// Important: LmdApi_Get_Accounts will get the accounts API, not a list of accounts.
FARPROC getAccountApiProc = GetProcAddress(lugormodModule, "LmdApi_Get_Accounts");
if (!getAccountApiProc) {
	// Error
	return;
}

Note: QMM may give you the handle to the game dll without having to use GetModuleHandle.
Note: Careful when using proxy mods such as JASS, as Lugormod's game dll may be using a different name.

Once an API function is obtained, it should be called by passing the relevant LMDAPI_X_VERSION_V macro into it, which is defined in the relevant API header 
file.  The most recent version is reflected by the macro LMDAPI_X_VERSION_CURRENT.

This method of versioning the API allows the mod to recognize what version your plugin was designed for, and give you a function list that mantains compatibility with the current version of the mod.  If backwards compatibility cannot be mantained, the API function will return NULL for that version.

The API function will return a void pointer, which you must then cast into the correct function struct.  The structs are named LmdApi_X_v0_t, where X is the API name and 0 is the version.  The most recent struct will be represented to by the macro LmdApi_Accounts_t

These steps are combined into macros called "LmdApi_GetCurrent_X(address)" which will fetch the most recent known API in your copy of the header file.

~
#include "Lmd_API_Accounts.h"
~

LmdApi_Accounts_t accountsApi = LmdApi_GetCurrent_Accounts(getAccountApiProc);

Once you have the API structure, you can call its functions.  Currently, one copy of the function struct is kept in memory and shared on all plugins.  If you change the content of a function pointer, other plugins using the same API version will use the new pointer, but most of the functions are proxy functions inside the mod, and will not be used by the mod itself.  Because of this, the API structure is not a good way of overriding the mod behavior.

You can find detailed information on each API surface by reading the comments inside the header files.  All API header files will attempt to include any other header files they need to be used, so be sure to copy over the contents of CoreTypes and LugormodHeaders to where your plugin can access it.


Samples:

In addition to some sample code, the samples directory contains some components that might be helpful.  Given time, they will be cleaned up and a standard lmd library of functions will be created, but for now you are free to use them in your Lugormod plugins.



Reference:

GetModuleHandle:
http://msdn.microsoft.com/en-us/library/ms683199(VS.85).aspx

GetProcAddress:
http://msdn.microsoft.com/en-us/library/ms683212(v=vs.85).aspx