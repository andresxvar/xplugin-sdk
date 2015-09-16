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

/* $Id: CEngineMgr.cpp,v 1.9 2005/10/13 02:02:53 cybermind Exp $ */

#include <stddef.h>		//for NULL
#include <string.h>		//for strncpy
#include "CEngineMgr.h"
#include "game_api.h"
#include "qmmapi.h"
#include "osdef.h"
#include "../jkasdk/game/g_local.h"

CEngineMgr::CEngineMgr(eng_syscall_t syscall, const char* dllname, const char* basedir, msgname_t msgnames, const char* homepath) {
	this->pfnsyscall = syscall;
	this->dllname = dllname;
	this->basedir = basedir;
	this->msgnames = msgnames;
	this->homepath = homepath;
}

CEngineMgr::~CEngineMgr() {
}

eng_syscall_t CEngineMgr::SysCall() {
	return this->pfnsyscall;
}

const char* CEngineMgr::GetDLLName() {
	return this->dllname;
}

const char* CEngineMgr::GetModDir() {
	static char dir[MAX_PATH] = "";
	if (dir[0]) return dir;

	this->pfnsyscall(G_CVAR_VARIABLE_STRING_BUFFER, "fs_game", dir, sizeof(dir));
	if (!dir[0])
		strncpy(dir, this->basedir, sizeof(dir));

	return dir;
}

const char* CEngineMgr::GetDLLPrefix() {
	static char prefix[MAX_PATH] = "";

	this->pfnsyscall(G_CVAR_VARIABLE_STRING_BUFFER, "fs_dllprefix", prefix, sizeof(prefix));
	if (!prefix[0])
		strncpy(prefix, "qmm", sizeof(prefix));

	return prefix;
}

const char* CEngineMgr::GetBaseDir() {
	return this->basedir;
}

const char* CEngineMgr::GetMsgName(int msg) {
	return this->msgnames(msg);
}

const char* CEngineMgr::GetHomepath(){
	return this->homepath;
}

CEngineMgr* CEngineMgr::GetInstance(eng_syscall_t syscall, const char* dllname, const char* basedir, msgname_t msgnames, const char* homepath) {
	if (!CEngineMgr::instance)
		CEngineMgr::instance = new CEngineMgr(syscall, dllname, basedir, msgnames, homepath);

	return CEngineMgr::instance;
}

CEngineMgr* CEngineMgr::instance = NULL;