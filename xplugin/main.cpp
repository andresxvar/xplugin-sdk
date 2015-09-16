#include "main.h"
#include "cmds.h"
#include "utils.h"
#include "qmmapi.h"

#pragma region Variables
// gamadata variables
gentity_t* g_gents		= NULL;
gclient_t* g_clients	= NULL;
int	g_gentsize			= sizeof(gentity_t);
int g_clientsize		= sizeof(gclient_t);

// qmm variables
eng_syscall_t	g_syscall = NULL;
mod_vmMain_t	g_vmMain = NULL;
pluginfuncs_t*	g_pluginfuncs = NULL;
pluginres_t*	g_result = NULL;

// plugin information
plugininfo_t g_plugininfo =
{
	PLUGIN_INFO_NAME,
	PLUGIN_INFO_VERSION,
	PLUGIN_INFO_DESCRIPTION,
	PLUGIN_INFO_AUTHOR,
	PLUGIN_INFO_CONTACT,							    
	0,                              //can this plugin be paused?
	0,                              //can this plugin be loaded via cmd
	0,                              //can this plugin be unloaded via cmd
	QMM_PIFV_MAJOR,                 //plugin interface version major
	QMM_PIFV_MINOR                  //plugin interface version minor
};

// plugin variables
int g_survivalhp = 0;
int g_maxclients = MAX_CLIENTS;
vec3_t vec3_origin = {0,0,0};
vec3_t axisDefault[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
Lmdp_Player_t Lmdp_Players[MAX_CLIENTS];

#pragma endregion


C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo)
{
	QMM_GIVE_PINFO();
}

C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc,
						   pluginres_t* presult, pluginfuncs_t* pluginfuncs,
						   int iscmd)
{
	QMM_SAVE_VARS();
	iscmd = 0;
	
	if (LmdApi.Initialize()) {
		PatchGame();
		return 1;
	}
	else
		return 0;	// lmd_jampgamex86.dll was not loaded
}

C_DLLEXPORT void QMM_Detach(int iscmd)
{
	if (LmdApi.isInitiliazed)
		UnpatchGame();
	iscmd = 0;
}

C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3,
						   int arg4, int arg5, int arg6, int arg7, int arg8,
						   int arg9, int arg10, int arg11)
{
	gameExport_t vmcmd = (gameExport_t)cmd;

	if (cmd == GAME_CLIENT_COMMAND) {		
		return ClientCommand(arg0);
	}
	else if (cmd == GAME_CONSOLE_COMMAND) {
		return ConsoleCommand();
	}
	else if(cmd == GAME_CLIENT_DISCONNECT) {
		// clear Lmdp_Player data for the disconnected player
		memset(&Lmdp_Players[arg0], 0, sizeof(Lmdp_Players[arg0]));
	}
	

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3,
							int arg4, int arg5, int arg6, int arg7, int arg8,
							int arg9, int arg10, int arg11, int arg12)
{
	if (cmd == G_LOCATE_GAME_DATA) {
		// this is no longer needed but some older code may be using it
		g_gents = (gentity_t*)arg0;
		g_gentsize = arg2;
		g_clients = (gclient_t*)arg3;
		g_clientsize = arg4;

		// store maxclients in global variable
		if (g_level->maxclients < MAX_CLIENTS)
			g_maxclients = g_level->maxclients;
	}
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2,
								int arg3, int arg4, int arg5, int arg6,
								int arg7, int arg8, int arg9, int arg10,
								int arg11)
{

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2,
								 int arg3, int arg4, int arg5, int arg6,
								 int arg7, int arg8, int arg9, int arg10,
								 int arg11, int arg12)
{
	QMM_RET_IGNORED(1);
}