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

/* $Id: CModMgr.h,v 1.6 2005/10/12 22:57:57 cybermind Exp $ */

#ifndef __CMODMGR_H__
#define __CMODMGR_H__

#include "osdef.h"
#include "CDLLMod.h"
#include "game_api.h"
#include "qmmapi.h"

class CModMgr {
public:
	CModMgr(eng_syscall_t qmm_syscall, msgname_t msgnames);
	~CModMgr();

	int LoadMod();
	void UnloadMod();

	eng_syscall_t QMM_SysCall();

	CDLLMod* Mod();

	const char* GetMsgName(int msg);

	static CModMgr* GetInstance(eng_syscall_t qmm_syscall, msgname_t msgnames);

private:
	CDLLMod* mod;
	msgname_t msgnames;
	eng_syscall_t qmm_syscall;
	CDLLMod* newmod(const char*);

	static CModMgr* instance;
};

#endif //__CMODMGR_H__
