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

/* $Id: CModMgr.cpp,v 1.14 2005/10/12 22:57:57 cybermind Exp $ */

#include <stddef.h>
#include <string.h>
#include "osdef.h"
#include "CConfigMgr.h"
#include "CEngineMgr.h"
#include "CModMgr.h"
#include "CDLLMod.h"
#include "game_api.h"
#include "util.h"
#include "qmm.h"
#include "../jkasdk/game/g_local.h"

CModMgr::CModMgr(eng_syscall_t qmm_syscall, msgname_t msgnames) {
	this->qmm_syscall = qmm_syscall;
	this->msgnames = msgnames;

	this->mod = NULL;
}

CModMgr::~CModMgr() {
	this->UnloadMod();
}

// - file is the path relative to the mod directory
//this uses the engine functions to reliably open the file regardless of homepath crap
CDLLMod* CModMgr::newmod(const char* file) {
	CDLLMod* ret = new CDLLMod;
	return ret;
}

//attempts to load a mod in the following search order:
// - a mod file specified in the config file
//	- dll mod is loaded from homepath then install dir
// - a dll/so named qmm_<modfilename> in the homepath
// - a dll/so named qmm_<modfilename> in the install dir
// - a qvm named vm/<modqvmname>
int CModMgr::LoadMod() {
	//load mod file setting from config file
	//this should be relative to mod directory
	char* cfg_mod = g_ConfigMgr->GetStr(vaf("%s/mod", g_EngineMgr->GetModDir()));

	if (cfg_mod && *cfg_mod) {
		ENG_SYSCALL(G_PRINT, vaf("[QMM] CModMgr::LoadMod(): Mod file specified in configuration file: \"%s\"\n", cfg_mod));

		//detect mod type
		this->mod = this->newmod(cfg_mod);

		//if a type was detected
		if (this->mod) {
			//load with homepath first
			if (this->mod->LoadMod(vaf("%s%s/%s", g_EngineMgr->GetHomepath(), g_EngineMgr->GetModDir(), cfg_mod)))
				return 1;

			//if a homepath exists, and the above load failed, load from install dir
			if (g_EngineMgr->GetHomepath()[0]) {
				ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in homepath, checking install directory\n", cfg_mod));
				if (this->mod->LoadMod(vaf("%s/%s", g_EngineMgr->GetModDir(), cfg_mod)))
					return 1;

				//load failed
				ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in install directory\n", cfg_mod));
			}

			//attempt to load dll mod using default filename
			ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\", attempting to load default DLL mod file \"qmm_%s\"\n", cfg_mod, g_EngineMgr->GetDLLName()));
			//mod type wasn't detected
		}
		else
		{
			ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to determine mod type of file \"%s\"\n", cfg_mod));
		}
	}
	else
	{
		ENG_SYSCALL(G_PRINT, vaf("[QMM] WARNING: CModMgr::LoadMod(): Unable to detect mod file setting from configuration file, attempting to load default DLL mod file \"qmm_%s\"\n", g_EngineMgr->GetDLLName()));
	}

	//attempt to load <prefix>_<dllname>
	cfg_mod = vaf("%s_%s", g_EngineMgr->GetDLLPrefix(), g_EngineMgr->GetDLLName());

	//make dll mod object
	this->mod = new CDLLMod;

	//load with homepath first
	if (this->mod->LoadMod(vaf("%s%s/%s", g_EngineMgr->GetHomepath(), g_EngineMgr->GetModDir(), cfg_mod)))
		return 1;

	//if a homepath exists, and the above load failed, load from install dir
	if (g_EngineMgr->GetHomepath()[0]) {
		ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in homepath, checking install directory\n", cfg_mod));
		if (this->mod->LoadMod(vaf("%s/%s", g_EngineMgr->GetModDir(), cfg_mod)))
			return 1;

		//load failed
		ENG_SYSCALL(G_PRINT, vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in install directory\n", cfg_mod));
	}

	//delete mod object since we failed
	delete this->mod;

	ENG_SYSCALL(G_ERROR, "[QMM] FATAL ERROR: Unable to load mod file\n");

	return 0;
}

void CModMgr::UnloadMod() {
	if (this->mod)
		delete this->mod;
}

eng_syscall_t CModMgr::QMM_SysCall() {
	return this->qmm_syscall;
}

CDLLMod* CModMgr::Mod() {
	return this->mod;
}

const char* CModMgr::GetMsgName(int msg) {
	return this->msgnames(msg);
}

CModMgr* CModMgr::GetInstance(eng_syscall_t qmm_syscall, msgname_t msgnames) {
	if (!CModMgr::instance)
		CModMgr::instance = new CModMgr(qmm_syscall, msgnames);

	return CModMgr::instance;
}

CModMgr* CModMgr::instance = NULL;