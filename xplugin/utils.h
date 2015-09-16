#pragma once
#include "local.h"
#include "interface.h"

#define ARRAY_LEN(x) (sizeof(x)/sizeof(*(x)))

// utils.cpp
void Lmdp_Init();
void Lmdp_RemoveNewLine(char *s);
int	Lmdp_Trace(gentity_t* ent, int range=Q3_INFINITE, qboolean clientOnly=qfalse);
gentity_t* Lmdp_Find(gentity_t *from, int fieldOfs, const char *match);
gentity_t *Lmdp_TempEntity(vec3_t origin, int event);
int Lmdp_ClientIdFromString(gentity_t* to, const char *s);
AccountDataCategory_t *Lmdp_GetAccountData(Account_t *acc, int DataCategoryOfs);
int Lmdp_GetAccountLevel(Account_t *acc);
int Lmdp_GetAccountProfession(Account_t *acc);
void Lmdp_ObjectDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath);
qboolean Lmdp_EditEntity(gentity_t *ent, const char *key, const char *value);

qboolean StringIsInteger(const char *s);
char *ConcatArgs(int);
void ScorePlum(int clientId, vec3_t origin, int score);
void PlaySound(vec3_t origin,int channel, int soundIndex);
void vectoangles(const vec3_t value1, vec3_t angles);
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);