#pragma once
#include "local.h"
#include "API/Lmd_API.h"
#include "API/Lmd_API_Entities.h"
#include "API/Lmd_API_Accounts.h"
#include "API/Lmd_API_Commands_Auths.h"
#include "API/Lmd_API_Professions.h"

#define BUILD_MOD

// Hook Replacement Functions
void Lmdp_target_print(gentity_t *self);

//
// Lugormod API
//
class LugormodAPI
{
public:
	LugormodAPI() 
		: isInitiliazed(qfalse), accounts(NULL), cmdAuths(NULL), 
		entities(NULL),	professions(NULL) { }
	
	// Initialize is called to setup the LugormodAPI object
	qboolean Initialize();

	// API containers
	LmdApi_Accounts_t *accounts;
	LmdApi_Commands_Auths_t *cmdAuths;
	LmdApi_Entities_t *entities;
	LmdApi_Professions_t *professions;

	qboolean isInitiliazed;	// qfalse if the LugormodAPI failed to initialize
};
extern LugormodAPI LmdApi; // lugormod API container

//
// lugormod
//
typedef struct SpawnData_s{
	qboolean canSave;			// saved to lmd file
	gentity_t *spawned;			// entity spawned by the spawnData
	struct KeyPairSet_s{
		unsigned int count;		// number of key,value pairs
		struct KeyPair_s{
			char *key;
			char *value;
		} *pairs;				// array of key value pairs
	} keys;
}SpawnData_t;

// Lugormod dummy Data Structure for reversing

enum accountData_t {
	ACCOUNT_DATA_UNK1,
	ACCOUNT_DATA_UNK2,
	ACCOUNT_DATA_UNK3,
	ACCOUNT_DATA_STATS,
	ACCOUNT_DATA_AUTH,
	ACCOUNT_DATA_INVENTORY,
	ACCOUNT_DATA_PROFESSION,
};

// Lugormod Profession Data Structure
typedef struct professionData_s
{
	int prof;
	int level;
	int* skills;
} professionData_t;

// Lugormod Auth Data Structure
typedef struct
{
	unsigned int count;
	struct authList_s
	{
		authFile_t *auth;
	} *list;							// list of authfiles
} authData_t;

// Lugormod Stats Data Structure
#define ACCOUNT_DATA_COUNT_BASE 7
typedef struct statData_s{
	int kills;
	int deaths;
	int shots;
	int hits;
	int duels;
	int duelsWon;
	int stashes;
} statData_t;

typedef struct AccountDataCategory_s AccountDataCategory_t;

typedef struct Account_s{
	char *username;
	char *name;
	unsigned int pwChksum;
	char *secCode;

	int id;
	int logins;
	unsigned int lastLogin;
	IP_t lastIP;

	int time;
	int score;
	int credits;
	int flags;
	struct AccountData_s
	{
		unsigned int count;
		AccountDataCategory_t **data;
	} accountData;
	int modifiedTime;
} Account_t;

// profession stuff
enum Profession_t
{
	PROFESSION_NONE,
	PROFESSION_GOD,
	PROFESSION_BOT,
	PROFESSION_FORCE,
	PROFESSION_MERC,

	PROFESSION_MAX
};

//Offsets 2.4.8.3
#define	G_TestLine_Pos			0x2002fc40	// void __cdecl G_TestLine(float * const,float * const,int,int) ai_wpnav.obj
#define	G_Damage_Pos			0x200c58e0	// void __cdecl G_Damage(struct gentity_s *,struct gentity_s *,struct gentity_s *,float * const,float * const,int,int,int) g_combat.obj
#define	G_RadiusDamage_Pos			0x200cf560	// int __cdecl G_RadiusDamage(float * const,struct gentity_s *,float,float,struct gentity_s *,struct gentity_s *,int) g_combat.obj
#define	player_die_Pos			0x200d15f0	// void __cdecl player_die(struct gentity_s *,struct gentity_s *,struct gentity_s *,int,int) g_combat.obj
#define	SP_PAS_Pos			0x200e8450	// void __cdecl SP_PAS(struct gentity_s *) g_items.obj
#define	G_NewString_Pos			0x2012cfd0	// char * __cdecl G_NewString(char const *) g_spawn.obj
#define	SP_target_print_Pos			0x20136d30	// void __cdecl SP_target_print(struct gentity_s *) g_target.obj
#define	G_FindConfigstringIndex_Pos			0x2014e150	// int __cdecl G_FindConfigstringIndex(char const *,int,int,int) g_utils.obj
#define	G_PlayEffectID_Pos			0x2014f110	// struct gentity_s * __cdecl G_PlayEffectID(int,float * const,float * const) g_utils.obj
#define	G_SetAnim_Pos			0x2014fcd0	// void __cdecl G_SetAnim(struct gentity_s *,int,int,int,int) g_utils.obj
#define	isBuddy_Pos			0x20180270	// int __cdecl isBuddy(struct gentity_s *,struct gentity_s *) Lmd_Commands.obj
#define	Lmd_Data_AutoFieldCallback_Free_Pos			0x2018b8a0	// void __cdecl Lmd_Data_AutoFieldCallback_Free(void *,void *) Lmd_Data.obj
#define	Lmd_Data_AutoFieldCallback_Parse_Pos			0x2018b8e0	// int __cdecl Lmd_Data_AutoFieldCallback_Parse(char *,char *,void *,void *) Lmd_Data.obj
#define	Lmd_Data_AutoFieldCallback_Write_Pos			0x2018b950	// enum DataWriteResult_t __cdecl Lmd_Data_AutoFieldCallback_Write(void *,char * const,int,char * const,int,void * *,void *) Lmd_Data.obj
#define	BlowUpEntity_Pos			0x20191950	// void __cdecl BlowUpEntity(struct gentity_s *) Lmd_Entities_Commands.obj
#define	PlayerUsableGetKeys_Pos			0x201946b0	// void __cdecl PlayerUsableGetKeys(struct gentity_s *) Lmd_Entities_Ents.obj
#define	PlayerUseableCheck_Pos			0x20194820	// int __cdecl PlayerUseableCheck(struct gentity_s *,struct gentity_s *) Lmd_Entities_Ents.obj
#define	Lmd_Entities_setSpawnstringKey_Pos			0x201a04b0	// void __cdecl Lmd_Entities_setSpawnstringKey(struct SpawnData_s *,char *,char *) Lmd_Entities_Spawner.obj
#define	cloneSpawnstring_Pos			0x201a0ea0	// struct SpawnData_s * __cdecl cloneSpawnstring(struct SpawnData_s *) Lmd_Entities_Spawner.obj
#define	removeSpawnstring_Pos			0x201a1130	// void __cdecl removeSpawnstring(struct SpawnData_s *) Lmd_Entities_Spawner.obj
#define	spawnEntity_Pos			0x201a1260	// struct gentity_s * __cdecl spawnEntity(struct gentity_s *,struct SpawnData_s *) Lmd_Entities_Spawner.obj
#define	trySpawn_Pos			0x201a1550	// struct gentity_s * __cdecl trySpawn(char *) Lmd_Entities_Spawner.obj
#define	Professions_ChooseProf_Pos			0x201be210	// int __cdecl Professions_ChooseProf(struct gentity_s *,int) Lmd_Prof_Core.obj
#define	NPC_Begin_Pos			0x2022f0b0	// void __cdecl NPC_Begin(struct gentity_s *) NPC_spawn.obj
#define	NPC_SpawnType_Pos			0x20231f00	// struct gentity_s * __cdecl NPC_SpawnType(struct gentity_s *,char *,char *,int) NPC_spawn.obj
#define	AngleVectors_Pos			0x20243900	// void __cdecl AngleVectors(float const * const,float * const,float * const,float * const) q_math.obj
#define	Info_SetValueForKey_Pos			0x2024a980	// void __cdecl Info_SetValueForKey(char *,char const *,char const *) q_shared.obj
#define	level_Pos			0x20ae90b8	// struct level_locals_t level g_main.obj
#define	Accounts_Prof_GetFieldData_Pos			0x201bc990	// void * __cdecl Accounts_Prof_GetFieldData(struct Account_s *) Lmd_Prof_Core.obj

#define EFUNC( type, name, args ) type (*name) args = (type (*)args)name##_Pos

void PatchGame();
void UnpatchGame();

extern qboolean(*Lmd_Data_AutoFieldCallback_Parse)(char *key, char *value, void *target, void *args);
extern DataWriteResult_t(*Lmd_Data_AutoFieldCallback_Write)(void *target, char key[], int keySize, char value[], int valueSize, void **writeState, void *args);
extern void(*Lmd_Data_AutoFieldCallback_Free)(void *state, void *args);
extern gentity_t* (*Lmd_Entities_setSpawnstringKey)(SpawnData_t *data, const char *value, const char *key);
extern void (*SP_PAS)(gentity_t *ent);
extern void (*AngleVectors)(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
extern gentity_t* (*G_PlayEffectID)(const int fxID, vec3_t origin, vec3_t direction);
extern void (*G_AddEvent)( gentity_t *ent, int event, int eventParm );
extern qboolean (*G_RadiusDamage)( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, gentity_t *missile, int mod);
extern void (*G_Damage)(gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod );
extern void (*G_TestLine)(vec3_t start, vec3_t end, int color, int time);
extern int (*G_FindConfigstringIndex)(const char *name, int start, int maxCS, qboolean create);
extern void (*Info_SetValueForKey)(char *s, const char *key, const char *value);
extern void (*ClientUserinfoChanged)(int clientNum);
extern gentity_t* (*NPC_SpawnType)(gentity_t *ent, char *npc_type, char *targetname, qboolean isVehicle);
extern void (*BlowUpEntity)(gentity_t *ent);
extern gentity_t* (*spawnEntity)(gentity_t* ent, SpawnData_t *data);
extern gentity_t* (*trySpawn)(const char *spawnstring);
extern qboolean (*PlayerUseableCheck)(gentity_t *ent, gentity_t *activator);
extern char* (*G_NewString)(const char *string );
extern char* (*Accounts_Custom_GetValue)(Account_t *acc, char *skill);
extern void (*Accounts_Custom_SetValue)(Account_t *acc, char *skill, char *value);
extern qboolean (*isBuddy)(gentity_t *player, gentity_t *target);
extern void (*NPC_Begin)(gentity_t *ent);
extern void (*changeSiegeClass)(gentity_t *player, char *siegeClass);
extern SpawnData_t* (*cloneSpawnstring)(SpawnData_t *);
extern void (*removeSpawnstring)(SpawnData_t *);
extern int (*Professions_ChooseProf)(struct gentity_s *, int);
extern void* (*Accounts_Prof_GetFieldData)(struct Account_s *);