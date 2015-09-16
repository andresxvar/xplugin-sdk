#pragma once
#include "local.h"

typedef struct
{
	char *cmdName;
	int (*cmdHandler)();
} svcmd_t;

typedef struct
{
	const char *name;
} dummyCmd_t;

typedef struct
{
	char *cmdName;
	int (*cmdHandler)(gentity_t *ent);
	int requirement;
	char *cmdInfo;
} cmd_t;

#define CMD_ACCOUNT		0x0001
#define CMD_MESSAGE		0x0002
#define CMD_DISABLE		0x0004
#define CMD_MAX_MESSAGES 5
#define CMD_MAX_INTERVAL 2000

int ClientCommand(int);
int ConsoleCommand(void);
int cmdcmp( const void *a, const void *b );
void Commands_Init();