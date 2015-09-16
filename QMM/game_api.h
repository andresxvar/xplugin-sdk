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

/* $Id: game_api.h,v 1.8 2006/01/29 22:45:37 cybermind Exp $ */

#ifndef __GAME_API_H__
#define __GAME_API_H__

typedef const char* (*msgname_t)(int);

//some information for each game engine supported by QMM
typedef struct gameinfo_s {
	const char* dllname;		//default dll mod filename
	const char* basedir;		//default moddir name
	msgname_t eng_msg_names;	//pointer to a function that returns a string for a given engine message
	msgname_t mod_msg_names;	//pointer to a function that returns a string for a given mod message
	const char* althomepath;	//alternate homepath for linux only (win = NULL)
} gameinfo_t;

extern gameinfo_t g_GameInfo;

const char* JKA_eng_msg_names(int);
const char* JKA_mod_msg_names(int);

#endif //__GAME_API_H__
