/*
QMM - Q3 MultiMod
Copyright QMM Team 2005
http://www.q3mm.org/

Licensing:
QMM is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

QMM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QMM; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Created By:
Kevin Masterson a.k.a. CyberMind <kevinm@planetquake.com>

*/

/* $Id: main.cpp,v 1.24 2006/03/04 22:33:29 cybermind Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "CEngineMgr.h"
#include "CModMgr.h"
#include "CLinkList.h"
#include "CConfigMgr.h"
#include "osdef.h"
#include "game_api.h"
#include "qmmapi.h"
#include "qmm.h"
#include "util.h"
#include "version.h"
#include "../jkasdk/game/g_local.h"

static unsigned int s_shutdown = 0;

CEngineMgr* g_EngineMgr = NULL;
CModMgr* g_ModMgr = NULL;
CPluginMgr* g_PluginMgr = NULL;
CConfigMgr* g_ConfigMgr = NULL;

//this is a much different dllEntry() from the last QMM version
//this time, we load the pdb config file, get various load-time settings,
//auto-detect the game as neccesary, and save all the crap to the Engine
//and Game managers
C_DLLEXPORT void dllEntry(eng_syscall_t syscall) {
	//this is some pretty convoluted shit
	//in windows, the game and custom files are always installed to the same dir

	//however, in linux, the game can typically be installed to one place but the
	//users are left without write access to it, so they place custom files in
	//a special directory in their home dir, such as /home/cybermind/.q3a/
	//the engine loads stuff from this special homepath just fine, including the
	//qmm binary, but qmm loads from "./" which actually boils down to the install
	//path. this means qmm may not always load files correctly.

	//basically what this bit of code does is check to see if qmm was loaded from
	//this special homepath. if it was, we assume that every other custom file will
	//be in the special homepath. as such, qmm prepends the homepath to every file
	//that it loads, except for mods. VM mods are loaded with the engine functions
	//that will automatically check the homepath, and the DLL mod might be located
	//in either directory. in this case, both locations are checked (homepath first)

#ifdef WIN32
	static char* homepath = "";
#else
	static char homepath[MAX_PATH] = "";
	char* tmp;
	char* envhome = getenv("HOME");

	tmp = vaf("%s%s", envhome, g_GameInfo.althomepath);

	if (!strncmp(get_modulepath(), tmp, strlen(tmp))) {
		strncpy(homepath, tmp, sizeof(homepath));
		printf("[QMM] Detected load from homepath \"%s\"\n", homepath);
	}
#endif
	
	//initialize the config manager and load pdb lib
	//use the homepath determined above to load the lib
	g_ConfigMgr = CConfigMgr::GetInstance();
	g_ConfigMgr->LoadLib(vaf("%s%s", homepath, PDB_LIB));
	//attempt to load the config file using various names
	//use the homepath determined above to load the config file
	if (g_ConfigMgr->IsLibLoaded()) {
		g_ConfigMgr->LoadConf(vaf("%sqmm.ini", homepath));
		if (!g_ConfigMgr->IsConfLoaded())
			g_ConfigMgr->LoadConf("qmm.ini");
	}
	
	// initialize engine manager and initialize mod manager
	const char* gamedllname = get_modulename();
	g_EngineMgr = CEngineMgr::GetInstance(syscall, gamedllname, g_GameInfo.basedir, g_GameInfo.eng_msg_names, homepath);
	g_ModMgr = CModMgr::GetInstance(QMM_syscall, g_GameInfo.mod_msg_names);

	//if unable to determine game, we can't do anything involving the engine
	//so generate a general OS error message and exit
	if (!g_EngineMgr) {
#ifdef WIN32
		//MessageBox(NULL, "QMM was unable to determine the game.\nPlease set the \"game\" option in qmm.ini.\nRefer to the documentation for more information.", "Critical QMM Error", 0);
#else
		//printf("\n\n=========\nCritical QMM Error:\nQMM was unable to determine the game.\nPlease set the \"game\" option in qmm.ini.\nRefer to the documentation for more information.\n=========\n");
#endif

		//forgot this before, now would be a good time to unload the config manager
		delete g_ConfigMgr;

		//temporarily store the syscall here, this is checked in vmMain
		//normal shutdown mode sets this to 1 specifically
		s_shutdown = (unsigned int)syscall;
		return;

		/* this error method has been removed in favor of the method above
		//i would prefer the standard G_ERROR, but since the engine manager wasn't loaded
		//we don't know what engine to use to get the G_ERROR message value, plus when
		//vmMain gets called with GAME_SHUTDOWN as a result of G_ERROR, it assumes the
		//engine manager is loaded

		exit(0);
		//comedy *(int*)0; option
		*/
	}

	//moved this down here so we don't have to worry about unloading it and such if the
	//engine manager fails to load
	g_PluginMgr = CPluginMgr::GetInstance();
}

//vmMain flow for all mods:
//	engine calls vmMain (thinks it is the mod's syscall)
//	call passed to plugins
//	call passed to mod
C_DLLEXPORT int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	//if the syscall was stored temporarily, it means the loading failed and we have
	//to exit. G_ERROR and GAME_SHUTDOWN are 1 in every game. we can't send G_ERROR
	//in GAME_SHUTDOWN since that would just cause recursion errors
	if (s_shutdown > 1) {
		if (cmd != QMM_FAIL_GAME_SHUTDOWN)
			((eng_syscall_t)s_shutdown)(QMM_FAIL_G_ERROR, "\n\n=========\nCritical QMM Error:\nQMM was unable to determine the game.\nPlease set the \"game\" option in qmm.ini.\nRefer to the documentation for more information.\n=========\n");
		return 0;
	}

	//if we are in shutdown mode, just route the call and check for shutdown
	if (s_shutdown == 1) {
		int ret = MOD_VMMAIN(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
		if (cmd == GAME_SHUTDOWN) {
			delete g_ModMgr;
			ENG_SYSCALL(G_PRINT, "[QMM] Shutting down\n");
			delete g_EngineMgr;
		}
		return ret;
	}

	if (cmd == GAME_INIT) {
		//get the shutdown cvar to see if we shutdown on a previous map
		s_shutdown = get_int_cvar("qmm_shutdown") ? 1 : 0;

		if (!g_ConfigMgr->IsLibLoaded())
			ENG_SYSCALL(G_PRINT, "[QMM] WARNING: ::vmMain(): Unable to load pdb library, all settings will use default values\n");
		else if (!g_ConfigMgr->IsConfLoaded())
			ENG_SYSCALL(G_PRINT, "[QMM] WARNING: ::vmMain(): Unable to load config file, all settings will use default values\n");

		ENG_SYSCALL(G_PRINT, "[QMM] QMM v" QMM_VERSION " (" QMM_OS ") loaded\n");
		ENG_SYSCALL(G_PRINT, vaf("[QMM] Mod: %s\n", g_EngineMgr->GetModDir()));
		ENG_SYSCALL(G_PRINT, "[QMM] Built: " QMM_COMPILE " by " QMM_BUILDER "\n");
		ENG_SYSCALL(G_PRINT, "[QMM] URL: http://qmm.cvs.sourceforge.net\n");

		if (!s_shutdown) {
			ENG_SYSCALL(G_PRINT, "[QMM] Registering CVARs\n");

			//make version cvar
			ENG_SYSCALL(G_CVAR_REGISTER, NULL, "qmm_version", QMM_VERSION, CVAR_ROM );//| ENG_MSG(QMM_CVAR_SERVERINFO));
			ENG_SYSCALL(G_CVAR_SET, "qmm_version", QMM_VERSION);
		}

		ENG_SYSCALL(G_PRINT, "[QMM] Attempting to load mod\n");

		if (!g_ModMgr->LoadMod()) {
			ENG_SYSCALL(G_ERROR, "[QMM] FATAL ERROR: Unable to load mod\n");
			return 0;
		}

		ENG_SYSCALL(G_PRINT, vaf("[QMM] Successfully loaded dll mod \"%s\"\n", g_ModMgr->Mod()->File()));

		if (!s_shutdown) {
			//load plugins
			//ENG_SYSCALL(G_PRINT, "[QMM] Attempting to load plugins\n");
			ENG_SYSCALL(G_PRINT, vaf("[QMM] Floating point support (Pi=%3.2f)\n", 3.14159));
			ENG_SYSCALL(G_PRINT, vaf("[QMM] Successfully loaded %d plugin(s)\n", g_PluginMgr->LoadPlugins()));

			//attempt to exec the qmmexec cfg
			char* cfg_execcfg = g_ConfigMgr->GetStr(vaf("%s/execcfg", g_EngineMgr->GetModDir()));

			if (!cfg_execcfg || (cfg_execcfg && !*cfg_execcfg))
				cfg_execcfg = "qmmaddons/qmm/qmmexec.cfg";

			ENG_SYSCALL(G_PRINT, vaf("[QMM] Executing config file \"%s\"\n", cfg_execcfg));
			ENG_SYSCALL(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, vaf("exec %s\n", cfg_execcfg));

			//we're done
			ENG_SYSCALL(G_PRINT, "[QMM] Startup successful, proceeding to mod startup\n");
		} else {
			//we're done with the config at this point, so unload it
			g_ConfigMgr->UnloadConf();
			g_ConfigMgr->UnloadLib();
			ENG_SYSCALL(G_PRINT, "[QMM] Successfully loaded in Shutdown mode, proceeding to mod startup\n");
		}
	}

	else if (cmd == GAME_CONSOLE_COMMAND) {
		char buf[5], arg1[14], arg2[MAX_PATH];
		ENG_SYSCALL(G_ARGV, 0, buf, sizeof(buf));
		buf[4] = '\0';
		int argc = ENG_SYSCALL(G_ARGC);

		if (!strcasecmp("qmm", buf)) {
			if (argc > 1)
				ENG_SYSCALL(G_ARGV, 1, arg1, sizeof(arg1));
			if (argc > 2)
				ENG_SYSCALL(G_ARGV, 2, arg2, sizeof(arg2));

			if (argc == 1) {
				ENG_SYSCALL(G_PRINT, "[QMM] Usage: qmm <command> [params]\n");
				ENG_SYSCALL(G_PRINT, "[QMM] Available sub commands:\n");
				ENG_SYSCALL(G_PRINT, "[QMM] status - displays information about QMM\n");
				ENG_SYSCALL(G_PRINT, "[QMM] list - displays information about loaded QMM plugins\n");
				ENG_SYSCALL(G_PRINT, "[QMM] load <file> - loads a new plugin\n");
				ENG_SYSCALL(G_PRINT, "[QMM] info <id/str> - outputs info on plugin with id\n");
				ENG_SYSCALL(G_PRINT, "[QMM] pause <id/str> - pauses plugin with id\n");
				ENG_SYSCALL(G_PRINT, "[QMM] unpause <id/str> - unpauses plugin with id\n");
				ENG_SYSCALL(G_PRINT, "[QMM] unload <id/str> - unloads plugin with id\n");
				ENG_SYSCALL(G_PRINT, "[QMM] force_unload <id/str> - forcefully unloads plugin with id\n");
				ENG_SYSCALL(G_PRINT, "[QMM] shutdown - permanantly unloads QMM except for neccesary functions\n");
				return 1;
			} else if (!strcasecmp("status", arg1)) {
				ENG_SYSCALL(G_PRINT, "[QMM] QMM v" QMM_VERSION " (" QMM_OS ") loaded\n");
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Mod: %s\n", g_EngineMgr->GetModDir()));
				ENG_SYSCALL(G_PRINT, "[QMM] Built: " QMM_COMPILE " by " QMM_BUILDER "\n");
				ENG_SYSCALL(G_PRINT, "[QMM] URL: http://qmm.cvs.sourceforge.net\n");
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Loaded mod file: %s\n", g_ModMgr->Mod()->File()));
				//g_ModMgr->Mod()->Status();
			} else if (!strcasecmp("list", arg1)) {
				g_PluginMgr->ListPlugins();
			} else if (!strcasecmp("load", arg1)) {
				if (argc == 2)
					ENG_SYSCALL(G_PRINT, "[QMM] load <file> - loads a new plugin\n");
				else
					g_PluginMgr->LoadPlugin(arg2, 1);
			} else if (!strcasecmp("info", arg1)) {
				if (argc == 2) {
					ENG_SYSCALL(G_PRINT, "[QMM] info <id/str> - outputs info on plugin with id\n");
					return 1;
				}
				CPlugin* plugin = g_PluginMgr->FindPlugin(arg2);
				if (!plugin) {
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Unable to find unique plugin match for \"%s\"\n", arg2));
					return 1;
				}
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin Info for \"%s\":\n", arg2));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Name: \"%s\"\n", plugin->PluginInfo()->name));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Version: \"%s\"\n", plugin->PluginInfo()->version));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] URL: \"%s\"\n", plugin->PluginInfo()->url));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Author: \"%s\"\n", plugin->PluginInfo()->author));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Desc: \"%s\"\n", plugin->PluginInfo()->desc));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Pausable: %s\n", plugin->PluginInfo()->canpause ? "yes" : "no"));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Cmd Loadable: %s\n", plugin->PluginInfo()->loadcmd ? "yes" : "no"));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Cmd Unloadable: %s\n", plugin->PluginInfo()->unloadcmd ? "yes" : "no"));
				ENG_SYSCALL(G_PRINT, vaf("[QMM] Loaded from: %s\n", plugin->IsCmd() ? "command" : "config"));
			} else if (!strcasecmp("pause", arg1)) {
				if (argc == 2) {
					ENG_SYSCALL(G_PRINT, "[QMM] pause <id/str> - pauses plugin with id\n");
					return 1;
				}
				CPlugin* plugin = g_PluginMgr->FindPlugin(arg2);
				if (!plugin) {
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Unable to find unique plugin match for \"%s\"\n", arg2));
					return 1;
				}
				if (plugin->Pause())
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" paused successfully\n", plugin->PluginInfo()->name));
				else
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" not paused: not allowed or already paused\n", plugin->PluginInfo()->name));
			} else if (!strcasecmp("unpause", arg1)) {
				if (argc == 2) {
					ENG_SYSCALL(G_PRINT, "[QMM] unpause <id/str> - unpauses plugin with id\n");
					return 1;
				}
				CPlugin* plugin = g_PluginMgr->FindPlugin(arg2);
				if (!plugin) {
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Unable to find unique plugin match for \"%s\"\n", arg2));
					return 1;
				}
				if (plugin->Unpause())
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" unpaused successfully\n", plugin->PluginInfo()->name));
				else
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" already unpaused\n", plugin->PluginInfo()->name));
			} else if (!strcasecmp("unload", arg1)) {
				if (argc == 2) {
					ENG_SYSCALL(G_PRINT, "[QMM] unload <id/str> - unloads plugin with id\n");
					return 1;
				}
				CPlugin* plugin = g_PluginMgr->FindPlugin(arg2);
				if (!plugin) {
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Unable to find unique plugin match for \"%s\"\n", arg2));
					return 1;
				}
				char temp[MAX_PATH];
				strncpy(temp, plugin->PluginInfo()->name, sizeof(temp));
				if (g_PluginMgr->UnloadPlugin(plugin, 1))
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" unloaded successfully\n", temp));
				else
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" not unloaded: not allowed\n", plugin->PluginInfo()->name));
			} else if (!strcasecmp("force_unload", arg1)) {
				if (argc == 2) {
					ENG_SYSCALL(G_PRINT, "[QMM] force_unload <id/str> - forcefully unloads plugin with id\n");
					return 1;
				}
				CPlugin* plugin = g_PluginMgr->FindPlugin(arg2);
				if (!plugin) {
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Unable to find unique plugin match for \"%s\"\n", arg2));
					return 1;
				}
				char temp[MAX_PATH];
				strncpy(temp, plugin->PluginInfo()->name, sizeof(temp));
				if (g_PluginMgr->UnloadPlugin(plugin, 0))
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" forcefully unloaded successfully\n", temp));
				else
					ENG_SYSCALL(G_PRINT, vaf("[QMM] Plugin \"%s\" not forcefully unloaded: unknown reason\n", plugin->PluginInfo()->name));
			} else if (!strcasecmp("shutdown", arg1)) {
				s_shutdown = 1;
				ENG_SYSCALL(G_PRINT, "[QMM] Preparing QMM Shutdown mode\n");
				log_write("[QMM] Preparing QMM Shutdown mode\n");
				if (g_PluginMgr) {
					ENG_SYSCALL(G_PRINT, "[QMM] Shutting down plugins\n");
					log_write("[QMM] Shutting down plugins\n");
					delete g_PluginMgr;
				}
				if (g_ConfigMgr->IsLibLoaded()) {
					ENG_SYSCALL(G_PRINT, "[QMM] Unloading pdb system\n");
					log_write("[QMM] Unloading pdb system\n");
					delete g_ConfigMgr;
				}
				ENG_SYSCALL(G_CVAR_REGISTER, NULL, "qmm_shutdown", "1", CVAR_ROM);
				ENG_SYSCALL(G_PRINT, "[QMM] Entering QMM Shutdown mode\n");
				log_write("[QMM] Entering QMM Shutdown mode\n");
			}

			return 1;
		}
	}

	//pass vmMain call to plugins, allow them to halt
	int ret = g_PluginMgr->CallvmMain(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

	//if user is connecting for the first time, user is not a bot, and "nogreeting" option is not set
	if (cmd == GAME_CLIENT_CONNECT && arg1 && !arg2 && !g_ConfigMgr->GetInt("nogreeting")) {
		ENG_SYSCALL(G_SEND_SERVER_COMMAND, arg0, "print \"^5This server is running ^3QMM JKA v" QMM_VERSION "\n\"");
		ENG_SYSCALL(G_SEND_SERVER_COMMAND, arg0, "print \"^5URL: ^3http://www.lugormod.com/^7\n\"");
	}
	else if (cmd == GAME_SHUTDOWN) {
		ENG_SYSCALL(G_PRINT, "[QMM] Shutting down plugins\n");
		delete g_PluginMgr;

		//this is after plugin unload, so plugins can call mod's vmMain while shutting down
		ENG_SYSCALL(G_PRINT, "[QMM] Shutting down mod\n");
		delete g_ModMgr;

		if (g_ConfigMgr->IsLibLoaded())
			ENG_SYSCALL(G_PRINT, "[QMM] Unloading pdb system\n");
		delete g_ConfigMgr;

		ENG_SYSCALL(G_PRINT, "[QMM] Finished shutting down, prepared for unload.\n");
		delete g_EngineMgr;
	}

	return ret;
}

//syscall flow for all mods:
//	call passed to QMM_SysCall
//	call passed to plugins
//	call passed to engine
int QMM_syscall(int cmd, ...) {
	va_list arglist;
	int args[13];	//JK2 decided to fuck shit up and have a single cmd with 13 args
	va_start(arglist, cmd);
	for (int i = 0; i < (sizeof(args)/sizeof(args[0])); ++i)
		args[i] = va_arg(arglist, int);
	va_end(arglist);

	if (s_shutdown == 1)
		return ENG_SYSCALL(cmd, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12]);

	//if this is a call to close a file, check the handle to see if it matches our existing log handle
	if (cmd == G_FS_FCLOSE_FILE) {
		if (args[0] == gamelog_get()) {
			//we have it, output final line and clear log file handle
			ENG_SYSCALL(G_PRINT, "[QMM] Detected close operation on g_log file handle, unhooking...\n");
			log_write("[QMM] Detected close operation on g_log file handle, unhooking...\n\n");
			gamelog_set(-1);
		}
	}

	//pass syscall to plugins, allow them to halt
	int ret = g_PluginMgr->Callsyscall(cmd, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12]);

	//if this is a call to open a file for APPEND or APPEND_SYNC
	if (cmd == G_FS_FOPEN_FILE) {
		if (args[2] == FS_APPEND || args[2] == FS_APPEND_SYNC) {
			//compare filename against g_log cvar
			if (!strcasecmp(get_str_cvar("g_log"), (char*)(args[0]))) {
				//we have it, save log file handle
				gamelog_set(*(int*)(args[1]));
				ENG_SYSCALL(G_PRINT, "[QMM] Successfully hooked g_log file\n");
				log_write("[QMM] Successfully hooked g_log file\n");
				log_write("[QMM] QMM v" QMM_VERSION " (" QMM_OS ") loaded\n");
				log_write(vaf("[QMM] Mod: %s\n", g_EngineMgr->GetModDir()));
				log_write("[QMM] Built: " QMM_COMPILE " by " QMM_BUILDER "\n");
				log_write("[QMM] URL: http://sourceforge.net/projects/qmm\n");
			}
		}
	}

	return ret;
}