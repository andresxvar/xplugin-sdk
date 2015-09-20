/*
This file includes codes used for interfacing with lugormod.
*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "utils.h"
#include "interface.h"
#include "qmmapi.h"
#include "hook.h"

#pragma region Lugormod API

LugormodAPI LmdApi;

// Initialize Api
qboolean LugormodAPI::Initialize()
{
	HMODULE lugormodModule = GetModuleHandleA("lmd_jampgamex86.dll");

	if (lugormodModule == NULL)
	{
		// invalid modulue dll file name
		isInitiliazed = qfalse;
		return qfalse;
	}

	LmdApi_Get_f getApiProc = NULL;

	getApiProc = (LmdApi_Get_f)GetProcAddress(lugormodModule,
		"LmdApi_Get_Accounts");
	if (getApiProc)
		accounts = LmdApi_GetCurrent_Accounts(getApiProc);

	getApiProc = (LmdApi_Get_f)GetProcAddress(lugormodModule,
		"LmdApi_Get_Entities");
	if (getApiProc)
		entities = LmdApi_GetCurrent_Entities(getApiProc);

	getApiProc = (LmdApi_Get_f)GetProcAddress(lugormodModule,
		"LmdApi_Get_Commands_Auths");
	if (getApiProc)
		cmdAuths = LmdApi_GetCurrent_Commands_Auths(getApiProc);

	getApiProc = (LmdApi_Get_f)GetProcAddress(lugormodModule,
		"LmdApi_Get_Professions");
	if (getApiProc)
		professions = LmdApi_GetCurrent_Professions(getApiProc);

	isInitiliazed = qtrue;
	return qtrue;;
}

#pragma endregion

#pragma region External Functions
// Lmd Account Data Callbacks for red, write, and free
EFUNC(qboolean, Lmd_Data_AutoFieldCallback_Parse, (char *key, char *value, void *target, void *args));
EFUNC(DataWriteResult_t, Lmd_Data_AutoFieldCallback_Write, (void *target, char key[], int keySize, char value[], int valueSize, void **writeState, void *args));
EFUNC(void, Lmd_Data_AutoFieldCallback_Free, (void *state, void *args));
EFUNC(void, AngleVectors, (const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up));
EFUNC(gentity_t*, NPC_SpawnType, (gentity_t *ent, char *npc_type, char *targetname, qboolean isVehicle));
EFUNC(void, NPC_Begin, (gentity_t *ent));
EFUNC(char*, G_NewString, (const char *string ));
EFUNC(void, G_SetAnim, (gentity_t *ent, int setAnimParts, int anim, int setAnimFlags, int blendTime));
EFUNC(void, G_TestLine, (vec3_t start, vec3_t end, int color, int time));
EFUNC(int, G_FindConfigstringIndex, (const char *name, int start, int maxCS, qboolean create));
EFUNC(gentity_t*, G_PlayEffectID, (int fxID, vec3_t origin, vec3_t direction));
EFUNC(void, G_Damage, (gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod ));
EFUNC(qboolean, G_RadiusDamage, (vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, gentity_t *missile, int mod));
EFUNC(void, BlowUpEntity, (gentity_t *ent));
EFUNC(qboolean, isBuddy, (gentity_t *player, gentity_t *target));
EFUNC(qboolean, PlayerUseableCheck, (gentity_t *ent, gentity_t *player));
EFUNC(qboolean, PlayerUsableGetKeys, (gentity_t *ent));
EFUNC(gentity_t*, trySpawn, (const char *spawnstring));
EFUNC(gentity_t*, spawnEntity, (gentity_t* ent, SpawnData_t *data));
EFUNC(gentity_t*, Lmd_Entities_setSpawnstringKey, (SpawnData_t *data, const char *value, const char *key));
EFUNC(SpawnData_t*, cloneSpawnstring, (SpawnData_t *data));
EFUNC(void, removeSpawnstring, (struct SpawnData_s *));
EFUNC(void, Info_SetValueForKey, (char *s,const char *key, const char *value));
EFUNC(void, SP_PAS, (gentity_t *ent));
EFUNC(int, Professions_ChooseProf, (struct gentity_s *, int));
EFUNC(void*, Accounts_Prof_GetFieldData, (struct Account_s *));
level_locals_t* g_level = (level_locals_t*)level_Pos;
#pragma endregion

#pragma region Hooks

// Target print is hooke to add formatting of name and health
Hook targetPrint(SP_target_print_Pos, (unsigned int)Lmdp_target_print);
void Lmdp_target_print_use(gentity_t *self, gentity_t *other, gentity_t *activator)
{
	char buf[MAX_STRING_CHARS] = {0};
	Q_strncpyz(buf, self->message, MAX_STRING_CHARS);

	if (activator && activator->client) {
		int clientNum = NUM_FROM_ENT(activator);

		if (clientNum < g_maxclients) {
			char *replace =  strstr( buf, "$n" );

			while(replace) {
				*replace = '\0';
				Q_strncpyz(buf,
					QMM_VARARGS("%s%s%s",
					buf, activator->client->pers.netname, replace + 2),
					MAX_STRING_CHARS);
				replace = strstr(replace +
					strlen(activator->client->pers.netname), "$n");
			}

			replace = strstr(buf, "$h"); // health
			while (replace) {
				*replace = '\0';
				Q_strncpyz(buf,
					QMM_VARARGS("%s%d%s",
					buf, activator->health, replace + 2), MAX_STRING_CHARS);
				replace = strstr(replace, "$h");
			}

			replace = strstr(buf, "$a"); // shield
			while (replace) {
				*replace = '\0';
				Q_strncpyz(buf,
					QMM_VARARGS("%s%d%s",
					buf, activator->client->ps.stats[STAT_ARMOR], replace + 2),
					MAX_STRING_CHARS);
				replace = strstr(replace, "$a");
			}
		}

		if (!(self->spawnflags & 4))
			clientNum = -1;

		if (self->spawnflags & 16) {
			char *p = buf;
			char *line = buf;

			for (p; *p; ++p) {
				if (*p == '\n')  {
					*p = '\0';
					LMDP_CHATF(clientNum, "%s", line);
					++p;
					line = p;
				}
			}
			if (*line)
				LMDP_CHATF(clientNum, "%s", line);
		}
		else if (self->spawnflags & 8) 
			LMDP_PRINTF(clientNum, "%s\n", buf);
		else
			LMDP_CPF(clientNum, "%s", buf);
	}
}
void Lmdp_target_print(gentity_t *self)
{
	if (self->spawnflags & 128)
		self->use = Lmdp_target_print_use;
	else
	{
		targetPrint.RemovePatch();
		((void (__cdecl*)(gentity_t *))targetPrint.GetAddress())(self);
		targetPrint.Patch();
	}
}

#pragma endregion

// PatchGame
// Write memomry hooks
void PatchGame()
{
	DWORD dwOldProtect = NULL;

	// patch hooked functions
	targetPrint.Patch();
}

// UnpatchGame
// Restore original patched memomry
void UnpatchGame()
{
	DWORD dwOldProtect = NULL;

	// restore original functions
	targetPrint.RemovePatch();
}