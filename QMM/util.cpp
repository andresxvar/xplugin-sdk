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

/* $Id: util.cpp,v 1.13 2006/03/04 22:33:29 cybermind Exp $ */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "osdef.h"
#include "CLinkList.h"
#include "qmm.h"
#include "util.h"
#include "../jkasdk/game/g_local.h"

static CLinkList<byte> s_plugins;

const char* get_modulename() {
	static char name[MAX_PATH] = "";
	if (name[0])
		return name;

	const char* pname = name;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulename, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT || !MBI.AllocationBase)
		return NULL;

	if(!GetModuleFileName((HMODULE)MBI.AllocationBase, name, sizeof(name)) || !name[0])
		return NULL;

	name[sizeof(name)-1] = '\0';

	int slen = strlen(name);
	for (int i = slen; i >= 0; --i) {
		if (name[i] == '\\') {
			pname = &name[i + 1];
			break;
		}
	}
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulename, &dli))
		return NULL;

	int slen = strlen(dli.dli_fname);
	for (int i = slen; i >= 0; --i) {
		if (dli.dli_fname[i] == '/') {
			++i;
			for (int j = 0; i+j <= slen; ++j)
				name[j] = dli.dli_fname[i+j];
			break;
		}
	}

	if (!name[0])
		strncpy(name, dli.dli_fname, sizeof(name));
#endif
	return pname;
}

const char* get_modulepath() {
	static char name[MAX_PATH] = "";
	if (name[0])
		return name;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulename, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT || !MBI.AllocationBase)
		return NULL;

	if(!GetModuleFileName((HMODULE)MBI.AllocationBase, name, sizeof(name)) || !name[0])
		return NULL;

	name[sizeof(name)-1] = '\0';

	int slen = strlen(name);
	for (int i = slen; i >= 0; --i) {
		if (name[i] == '\\') {
			name[i] = '\0';
			break;
		}
	}
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulename, &dli))
		return NULL;

	int slen = strlen(dli.dli_fname);
	for (int i = slen; i >= 0; --i) {
		if (dli.dli_fname[i] == '/') {
			strncpy(name, dli.dli_fname, i);
			name[i] = '\0';
			break;
		}
	}

	if (!name[0])
		strncpy(name, dli.dli_fname, sizeof(name));
#endif
	return name;
}

void* get_modulehandle() {
	static void* handle = NULL;
	if (handle)
		return handle;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulehandle, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT)
		return NULL;

	handle = (void*)MBI.AllocationBase;
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulehandle, &dli))
		return NULL;

	handle = dli.dli_fbase;
#endif
	return handle;
}

int ismoduleloaded(void* handle) {
	CLinkNode<byte>* p = s_plugins.first();
	while (p) {
		if (p->data() == (byte*)handle)
			return 1;
		p = p->next();
	}
	return 0;
}

void setmoduleloaded(void* handle) {
	if (!handle || ismoduleloaded(handle))
		return;

	s_plugins.add((byte*)handle,0);
}

void setmoduleunloaded(void* handle) {
	CLinkNode<byte>* p = s_plugins.first();
	while (p) {
		if (p->data() == (byte*)handle) {
			s_plugins.del(p);
			return;
		}
		p = p->next();
	}
}

//this uses a cycling array of strings so the return value does not need to be stored locally
char* vaf(char* format, ...) {
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

int get_int_cvar(const char* cvar) {
	if (!cvar || !*cvar)
		return -1;

	return ENG_SYSCALL(G_CVAR_VARIABLE_INTEGER_VALUE, cvar);
}

//this uses a cycling array of strings so the return value does not need to be stored locally
#define MAX_CVAR_LEN	256
const char* get_str_cvar(const char* cvar) {
	if (!cvar || !*cvar)
		return NULL;

	static char temp[8][MAX_CVAR_LEN];
	static int index = 0;
	int i = index;

	ENG_SYSCALL(G_CVAR_VARIABLE_STRING_BUFFER, cvar, temp[i], sizeof(temp[i]));
	index = (index + 1) & 7;
	return temp[i];
}

static int s_fh = -1;
static int loghandle = -1;

void gamelog_set(int handle) {
	loghandle = handle;
}

int gamelog_get() {
	return loghandle;
}

void log_set(int fh) {
	s_fh = fh;
	if(s_fh<0||s_fh>2){
		s_fh=1;
	}
}

int log_get() {
	return s_fh;
}

char * filename;

void logfile_set(const char * logfile){
#ifdef WIN32
	char* homepath = "";
#else
	const char* homepath = g_EngineMgr->GetHomepath();
#endif
	const char * moddir=g_EngineMgr->GetModDir();
	char * name_temp = vaf("%s%s/%s",homepath,moddir,logfile);
	filename=new char[strlen(name_temp)+1];
	int i;
	for(i=0;i<strlen(name_temp);++i){
		filename[i]=name_temp[i];
	}
	filename[i]='\0';
}

const char * logfile_get(){
	return filename;
}

int log_write(const char* text, int len) {
	if (s_fh != -1 && text && *text) {
		if (len == -1)
			len = strlen(text);

		return ENG_SYSCALL(G_FS_WRITE, text, len, s_fh);
	}

	return -1;
}

const char* my_strcasestr(const char* s1, const char* s2) {
	if (!s1 || !*s1 || !s2 || !*s2)
		return NULL;

	//store string length
	int slen1 = strlen(s1);
	int slen2 = strlen(s2);

	//allocate space for lowercase versions of the strings
	char* temps1 = (char*)malloc(slen1+1);
	char* temps2 = (char*)malloc(slen2+1);

	//null terminate
	temps1[slen1] = '\0';
	temps2[slen2] = '\0';

	//copy strings
	int i;
	for (i = 0; i < slen1; ++i)
		temps1[i] = (char)tolower(s1[i]);

	for (i = 0; i < slen2; ++i)
		temps2[i] = (char)tolower(s2[i]);

	//search
	char* x = strstr(temps1, temps2);

	//free allocated space
	free(temps1);
	free(temps2);

	if (!x)
		return NULL;

	//determine the offset into the string that the match occurred and add to s1
	return s1+(x-temps1);
}

int isnumber(const char* str) {
	for (int i = 0; str[i]; ++i) {
		if (str[i] < '0' || str[i] > '9')
			return 0;
	}

	return 1;
}

int write_file(const char* file, const char* outfile) {
	outfile = vaf("%s/%s", g_EngineMgr->GetModDir(), outfile ? outfile : file);

	//check if the real file already exists
	FILE* ffile = fopen(outfile, "r");
	if (ffile) {
		fclose(ffile);
		return 0;
	}

	//open file from inside pk3
	int fpk3, fsize = ENG_SYSCALL(G_FS_FOPEN_FILE, file, &fpk3, FS_READ);
	if (fsize <= 0) {
		ENG_SYSCALL(G_FS_FCLOSE_FILE, fpk3);
		return 0;
	}

	//open output file
	ffile = fopen(outfile, "wb");
	if (!ffile) {
		ENG_SYSCALL(G_FS_FCLOSE_FILE, fpk3);
		return 0;
	}

	//read file in blocks of 512
	byte buf[512];
	int left = fsize;
	while (left >= sizeof(buf)) {
		ENG_SYSCALL(G_FS_READ, buf, sizeof(buf), fpk3);
		fwrite(buf, sizeof(buf), 1, ffile);
		left -= sizeof(buf);
	}
	if (left) {
		ENG_SYSCALL(G_FS_READ, buf, left, fpk3);
		fwrite(buf, left, 1, ffile);
	}

	//close file handles
	ENG_SYSCALL(G_FS_FCLOSE_FILE, fpk3);
	fclose(ffile);

	return fsize;
}