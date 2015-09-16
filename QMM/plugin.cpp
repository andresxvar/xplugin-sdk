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

/* $Id: plugin.cpp,v 1.9 2006/01/29 22:45:37 cybermind Exp $ */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "CModMgr.h"
#include "qmm.h"
#include "qmmapi.h"
#include "util.h"
#include "CLinkList.h"

static int plugin_WriteGameLog(const char* text) {
	return log_write(text);
}
static int plugin_WriteQMMLog(const char* text) {
	return log_write(text);
}

static char* plugin_VarArgs(char* format, ...) {
	va_list	argptr;
	static char str[8][1024];
	static int index = 0;
	int i = index;

	va_start(argptr, format);
	vsnprintf(str[i], sizeof(str[i]), format, argptr);
	va_end(argptr);

	index = (index + 1) & 7;
	return str[i];
}

static const char* plugin_EngMsgName(int x) {
	return ENG_MSGNAME(x);
}

static const char* plugin_ModMsgName(int x) {
	return MOD_MSGNAME(x);
}

static int plugin_GetIntCvar(const char* cvar) {
	return get_int_cvar(cvar);
}

static const char* plugin_GetStrCvar(const char* cvar) {
	return get_str_cvar(cvar);
}

static int plugin_GetLogHandle() {
	return gamelog_get();
}

static const char* plugin_GetHomePath() {
	return g_EngineMgr->GetHomepath();
}

int plugin_GetPluginID(char* name) {
	return g_PluginMgr->GetPluginID((const char*)name);
}

const char* plugin_GetPluginInfo(int num) {
	return g_PluginMgr->GetPluginInfo(num);
}

int plugin_CallPlugin(int num, int cmd, ...) {
	if(num<0){
		return 0;
	}
	CPlugin* plug=g_PluginMgr->FindPlugin(num);
	if(!plug){
		return 0;
	}
	va_list arglist;
	int args[13];
	va_start(arglist, cmd);
	for (int i = 0; i < (sizeof(args)/sizeof(args[0])); ++i){
		args[i] = va_arg(arglist, int);
	}
	va_end(arglist);
	plugin_interface plugincallfunc = plug->plugin_Call();
	if(plugincallfunc == NULL){
		return 0;
	}
	return plugincallfunc(cmd, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12]);
}

static pluginfuncs_t s_pluginfuncs = {
	&plugin_WriteGameLog,
	&plugin_WriteQMMLog,
	&plugin_VarArgs,
	&plugin_EngMsgName,
	&plugin_ModMsgName,
	&plugin_GetIntCvar,
	&plugin_GetStrCvar,
	&plugin_GetLogHandle,
	&plugin_GetHomePath,
	&plugin_GetPluginID,
	&plugin_GetPluginInfo,
	&plugin_CallPlugin
};

pluginfuncs_t* get_pluginfuncs() {
	return &s_pluginfuncs;
}