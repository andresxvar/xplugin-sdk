#pragma once
#pragma warning( disable : 91 )
#include "g_local.h"

#define LMDP_PRINTF(ID, MSG, ...) g_syscall(G_SEND_SERVER_COMMAND, ID, QMM_VARARGS("print \""##MSG##"\n\"", ##__VA_ARGS__))
#define LMDP_PRINT(ID, MSG) g_syscall(G_SEND_SERVER_COMMAND, ID, "print \""##MSG##"\n\"")
#define LMDP_CHATF(ID, MSG, ...) g_syscall(G_SEND_SERVER_COMMAND, ID, QMM_VARARGS("chat \""##MSG##"\"", ##__VA_ARGS__))
#define LMDP_CPF(ID, MSG, ...) g_syscall(G_SEND_SERVER_COMMAND, ID, QMM_VARARGS("cp \""##MSG##"\"", ##__VA_ARGS__))

// macros to handle base entities
#define ENT_FROM_NUM(index)		(g_gents + index) // TODO: this doesn't work on logical entities therefore it may be better to remove these macros
#define NUM_FROM_ENT(ent)		ent->s.number //ent-g_gents
#define CLIENT_FROM_NUM(index)	(g_clients + index)

extern int g_survivalhp;
extern int g_maxclients;
extern gentity_t *g_gents;
extern gclient_t *g_clients;
extern level_locals_t *g_level;

typedef enum PW2_POWERUPS
{
	PW2_NONE = 0,
	PW2_WEAPONS_PLUS,
	PW2_TEST,

	PW2_NUM_POWERUPS
};

typedef struct Lmdp_Player_s
{
	int authrank;
	int cmdTime; // time of last command
	int msgCount;
	int powerups[PW2_NUM_POWERUPS];
	vec3_t		mark;
} Lmdp_Player_t;
extern Lmdp_Player_t Lmdp_Players[];