#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "cmds.h"
#include "utils.h"
#include "qmmapi.h"

#pragma region Client Commands

// gethere <entity number>
int Cmd_gethere_f(gentity_t *player)
{
	char *msg = NULL;
	int playerNum = NUM_FROM_ENT(player);

	if (g_syscall(G_ARGC) > 1) {
		char args[MAX_STRING_CHARS] = {0};

		g_syscall(G_ARGV, 1, args, sizeof(args));
		int targetNum = atoi(args);

		if (targetNum >= g_maxclients) {
			gentity_t *target = LmdApi.entities->getEntity(targetNum);
			if (target && target->inuse) {
				vec3_t newOrigin = {0};

				if (target->r.bmodel) {
					// bmodel's constant origin
					VectorSet(newOrigin,
						(target->r.maxs[0]+target->r.mins[0])/2,
						(target->r.maxs[1]+target->r.mins[1])/2,
						(target->r.maxs[2]+target->r.mins[2])/2);

					// angled bmodels have a rotated origin
					if (target->s.angles[0] || target->s.angles[1]
					|| target->s.angles[2])
					{
						// rotate constant origin around map's origin
						RotatePointAroundVector(newOrigin, axisDefault[0],
							newOrigin, target->r.currentAngles[2]);
						RotatePointAroundVector(newOrigin, axisDefault[1],
							newOrigin, target->r.currentAngles[0]);
						RotatePointAroundVector(newOrigin, axisDefault[2],
							newOrigin, target->r.currentAngles[1]);
					}
					// final nudged origin for spawnstring
					VectorSubtract(player->r.currentOrigin, newOrigin,
						newOrigin);
				}
				else
				{
					// for non bmodels set new origin as player's origin
					VectorCopy(player->r.currentOrigin, newOrigin);
				}

				if (Lmdp_EditEntity(target, "origin",
					QMM_VARARGS("%i %i %i",
					(int)newOrigin[0],
					(int)newOrigin[1],
					(int)newOrigin[2])))
				{
					msg = "^3Entity modified successfuly";
				}
				else
					msg = "^3Entity failed to respawn";
			}
			else
				msg = "^3Target not found";

			LMDP_PRINTF(playerNum, "%s.", msg);

			QMM_RET_SUPERCEDE(1);
		}
	}
	QMM_RET_IGNORED(1);
}

// goto <entity number>
int Cmd_goto_f(gentity_t *player)
{
	if (g_syscall(G_ARGC) > 1) {
		int playerNum = NUM_FROM_ENT(player);
		char args[MAX_STRING_CHARS] = {0};

		g_syscall(G_ARGV, 1, args, sizeof(args));
		int targetNum = atoi(args);

		if (targetNum >= g_maxclients) {
			gentity_t *target = LmdApi.entities->getEntity(targetNum);
			if (target && target->inuse) {
				vec3_t newOrigin = {0};

				if (target->r.bmodel)
				{
					// bmodel's constant origin
					VectorSet(newOrigin,
						(target->r.maxs[0]+target->r.mins[0])/2,
						(target->r.maxs[1]+target->r.mins[1])/2,
						(target->r.maxs[2]+target->r.mins[2])/2);

					// angled bmodels have a rotated origin
					if (target->s.angles[0] || target->s.angles[1]
					|| target->s.angles[2])
					{
						// rotate constant origin around map's origin
						RotatePointAroundVector(newOrigin, axisDefault[0],
							newOrigin, target->r.currentAngles[2]);
						RotatePointAroundVector(newOrigin, axisDefault[1],
							newOrigin, target->r.currentAngles[0]);
						RotatePointAroundVector(newOrigin, axisDefault[2],
							newOrigin, target->r.currentAngles[1]);
					}
					VectorAdd(newOrigin, target->s.origin, newOrigin);
				}
				else
					VectorCopy(target->r.currentOrigin, newOrigin);

				player->client->ps.eFlags ^= EF_TELEPORT_BIT;
				VectorCopy(newOrigin, player->client->ps.origin);
			}
			else
			{
				g_syscall(G_SEND_SERVER_COMMAND, playerNum,
					"print \"^3Target not found.\n\"");
			}
			QMM_RET_SUPERCEDE(1);
		}
	}
	QMM_RET_IGNORED(1);
}

// slapother <player>
int Cmd_slapother_f(gentity_t *ent)
{
	int targetNum = -1;

	if (g_syscall(G_ARGC) > 1) {
		char args[MAX_STRING_CHARS] = {0};
		g_syscall(G_ARGV, 1, args, sizeof(args));
		// get targetNum from command
		targetNum = Lmdp_ClientIdFromString(ent, args);
	}
	else
	{
		// get targetNum from trace
		targetNum = Lmdp_Trace(ent, Q3_INFINITE, qtrue);
	}

	if (targetNum > -1) {
		playerState_s *ps = &CLIENT_FROM_NUM(targetNum)->ps;
		ps->velocity[2] = 375;
		ps->forceHandExtend = HANDEXTEND_KNOCKDOWN;
		ps->forceDodgeAnim = 0;
		ps->forceHandExtendTime = g_level->time + 700;
	}
	QMM_RET_SUPERCEDE(1);
}

// allskills (documented style)
#define CMD_ALLSKILLS_COST 20; // the cost of using allskills command
int forcePowerSide[]
{
	//FP_FIRST = 0,//marker
	FORCE_LIGHTSIDE,//FP_HEAL = 0,//instant
	0,//FP_LEVITATION,//hold/duration
	0,//FP_SPEED,//duration
	0,//FP_PUSH,//hold/duration
	0,//FP_PULL,//hold/duration
	FORCE_LIGHTSIDE,//FP_TELEPATHY,//instant
	
	FORCE_DARKSIDE,//FP_GRIP,//hold/duration
	FORCE_DARKSIDE,//FP_LIGHTNING,//hold/duration
	FORCE_DARKSIDE,//FP_RAGE,//duration
	
	FORCE_LIGHTSIDE,//FP_PROTECT,
	FORCE_LIGHTSIDE,//FP_ABSORB,
	FORCE_LIGHTSIDE,//FP_TEAM_HEAL,
	
	FORCE_DARKSIDE,//FP_TEAM_FORCE,
	FORCE_DARKSIDE,//FP_DRAIN,
	0,//FP_SEE,

	0,//FP_SABER_OFFENSE,
	0,//FP_SABER_DEFENSE,
	0,//FP_SABERTHROW,
	//NUM_FORCE_POWERS
};

int Cmd_allskills_f(gentity_t *player)
{
	int clientId = player->s.number; // the player's ID
	Account_t *acc = player->client->pers.Lmd.account; // pointer to account data
	int argumentsCount = g_syscall(G_ARGC); // number of arguments with command

	/* Checking for a valid account is already done on ClientCommand function
	if (!acc)
	{
		// the player does not have an account
		// so the plugin should ignore this command
		QMM_RET_IGNORED(1);// returns to lugormod unknown command message
	}
	*/

	// check the argument count
	if (argumentsCount == 1)
	{
		// player used /allskills without additional arguments
		// some information about the proper ussage is shown to the player
		LMDP_PRINT(clientId, "Use /allskills {jedi}{sith}{merc} to get all profession skills");

		QMM_RET_SUPERCEDE(1); // return to lugormod
	}

	// player used /allskills with some additional arguments
	// check balance
	int creditBalance = acc->credits - CMD_ALLSKILLS_COST;
	if (creditBalance < 0)
	{
		LMDP_PRINTF(clientId, "You need CR%i more to get all skills", -creditBalance);
		QMM_RET_SUPERCEDE(1); // return to lugormod
	}

	// acquire command argument
	char arg[MAX_STRING_CHARS] = { 0 }; // a string of characters to hold an argument
	g_syscall(G_ARGV, 1, arg, sizeof(arg)); // copies the 1st argument to 'args'
	int arglen = strlen(arg); // store the length of the argument

	int professionNum = 0; // the profession number
	int forceSide = 0;
	if (!Q_stricmpn(arg, "jedi", arglen))
	{
		professionNum = PROFESSION_FORCE;
		forceSide = FORCE_LIGHTSIDE;
	}
	else if (!Q_stricmpn(arg, "sith", arglen))
	{
		professionNum = PROFESSION_FORCE;
		forceSide = FORCE_DARKSIDE;
	}
	else if (!Q_stricmpn(arg, "merc", arglen))
	{
		professionNum = PROFESSION_MERC;
	}
	else
	{
		// unknown profession specified
		// display help to the player
		LMDP_PRINT(clientId, "Use /allskills {jedi}{sith}{merc} to get all profession skills");
		QMM_RET_SUPERCEDE(1); // return to lugormod
	}
	
	// apply profession
	Professions_ChooseProf(player, professionNum); // use lugormod's function to choose the right profession

	if (professionNum == PROFESSION_FORCE)
	{
		// upgrade force skills
		for (int i = FP_FIRST; i < NUM_FORCE_POWERS; ++i)
		{
			if (forcePowerSide[i] == 0 || forcePowerSide[i] == forceSide)
			{
				// upgrade it
				*((DWORD*)Accounts_Prof_GetFieldData(acc) + i) = 5;
			}
		}
	}
	else if (professionNum == PROFESSION_MERC)
	{
		for (int i = 0 /*MELEE*/; i < 8 /*YSALIMARI*/; ++i)
		{
			// upgrade it
			*((DWORD*)Accounts_Prof_GetFieldData(acc) + i) = 5;				
		}
	}

	LmdApi.accounts->setAccountCredits(acc, creditBalance); // apply credit balance

	QMM_RET_SUPERCEDE(1); // return to lugormod
}

// sskills
int Cmd_sskills_f(gentity_t *ent)
{
	QMM_RET_SUPERCEDE(1);
}

int Cmd_help_f(gentity_t *ent);

// cmds
// list of commands must be arranged alphabetically
cmd_t cmds[] =
{
	// command				function			requirement					description
	{ "allskills",			Cmd_allskills_f,	CMD_ACCOUNT | CMD_MESSAGE,	"level up all skills"},
	{ "help",				Cmd_help_f,			0,							NULL },	// list plugin commands
	{ "sskills",			Cmd_sskills_f,		CMD_ACCOUNT,				"special skills" },
};
static const size_t numCmds = ARRAY_LEN(cmds);

// adminCmds
// list of admin commands must be arranged alphabetically
cmd_t adminCmds[] =
{
	// command				function		requirement		description
	{ "get",				Cmd_gethere_f,		10,			"teleport entity"},
	{ "goto",				Cmd_goto_f,			10,			"teleport to entity"},
	{ "slapother",			Cmd_slapother_f,	10,			"slap a client"},
};
static const size_t numAdminCmds = ARRAY_LEN(adminCmds);

// help plugin
int Cmd_help_f(gentity_t *ent)
{
	int playerNum = NUM_FROM_ENT(ent);
	char msg[MAX_STRING_CHARS] = {0};
	char *msgp = NULL;

	if (g_syscall(G_ARGC) < 2)	{ 
		// using help command without arguments
		
		// show plugin information
		g_syscall(G_SEND_SERVER_COMMAND, playerNum,
			"print \"^2Plugin -- ^3" __TIME__ " " __DATE__  "\n\"");
		
		// use lugormod's help command
		g_vmMain(GAME_CLIENT_COMMAND, playerNum, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		
		// append a line for plugin section
		g_syscall(G_SEND_SERVER_COMMAND, playerNum,
			"print \"^5Plugin                    ^3Plugin commands\n\"");
		QMM_RET_SUPERCEDE(1);
	}
	else
	{
		char args[32] = {0};

		g_syscall(G_ARGV, 1, args, sizeof(args));

		if (!Q_stricmpn(args, "plugin", strlen(args))) {
			// Player use /help plugin

			// List plugin player commands
			for(int i = 0; i < numCmds; ++i) {
				if ( cmds[i].cmdInfo ) {
					if ((cmds[i].requirement & CMD_ACCOUNT)
						&& !ent->client->pers.Lmd.account)
						continue;

					LMDP_PRINTF(playerNum, "^5%-25s ^3%s",
						cmds[i].cmdName, cmds[i].cmdInfo);
				}
			}

			// List lmdplus admin commands
			int authRank = LmdApi.cmdAuths->playerAuthRank(ent);
			if (authRank < Q3_INFINITE)	{
				for(int i = 0; i < numAdminCmds; ++i) {
					if (adminCmds[i].cmdInfo
						&& adminCmds[i].requirement >= authRank)
					{
						LMDP_PRINTF(playerNum, "^6%-25s ^3%s",
							adminCmds[i].cmdName, adminCmds[i].cmdInfo);
					}
				}
			}
			QMM_RET_SUPERCEDE(1);
		}
	}
	QMM_RET_IGNORED(1);
}

// MessageTimeIsValid
// return true if the player has not reached
// CMD_MAX_MESSAGES within CMD_MAX_MESSAGES_TIME
qboolean MessageTimeIsValid(Lmdp_Player_t *player, int cmdTime)
{
	if (cmdTime + CMD_MAX_INTERVAL < g_level->time)
		player->msgCount = 0;
	else if (player->msgCount > CMD_MAX_MESSAGES)
		return qfalse;
	else
		player->msgCount++;

	return qtrue;
}

// cmdcmp
// Utility function to compare command name
// for a binary search
int cmdcmp( const void *a, const void *b )
{
	return Q_stricmpn((const char *)a,((dummyCmd_t *)b)->name, strlen((char*)a));
}

// ClientCommand
int ClientCommand(int playerNum)
{
	gentity_t  *player = ENT_FROM_NUM(playerNum);

	if( player->client
		&& (player->client->pers.connected == CON_CONNECTED)
		&& playerNum < g_maxclients)
	{
		char	cmd[MAX_TOKEN_CHARS] = {0};
		cmd_t	*command = NULL;
		Lmdp_Player_t *p_player = Lmdp_Players + playerNum;

		g_syscall(G_ARGV, 0, cmd, sizeof(cmd));

		int cmdTime = p_player->cmdTime + 1000;
		p_player->cmdTime = g_level->time;

		if (g_level->time > cmdTime || LmdApi.cmdAuths->playerHasAuthFlag(player, 9)) {
			// search client commands
			command = (cmd_t*)bsearch( cmd, cmds, numCmds, sizeof( cmds[ 0 ] ), cmdcmp );
			if(!command) {
				// search admin commands
				p_player->authrank = LmdApi.cmdAuths->playerAuthRank(player);
				if (p_player->authrank < Q3_INFINITE) {
					command = (cmd_t*)bsearch(cmd, adminCmds, numAdminCmds, sizeof( adminCmds[0] ), cmdcmp );
					if (command && (p_player->authrank <= command->requirement || !command->requirement) )
						return command->cmdHandler( player );
				}
			}
			else if (command->requirement & CMD_DISABLE);
			else if ((command->requirement & CMD_ACCOUNT) && !player->client->pers.Lmd.account);
			else if ((command->requirement & CMD_MESSAGE) && !MessageTimeIsValid(p_player, cmdTime)) {
				g_syscall(G_SEND_SERVER_COMMAND, playerNum, "cp \"^1Spam Protection");
				QMM_RET_SUPERCEDE(1);
			}
			else
				return command->cmdHandler( player );

			// command not available from plugin
			QMM_RET_IGNORED(1);
		}
	}
	QMM_RET_SUPERCEDE(1);
}
#pragma endregion

#pragma region Console Commands

// Console commands
// SvCmd_sayas_f
// Ussage: /sayas [match name or id] [message]
int SvCmd_sayas_f()
{
	int clientId; 
	if (g_syscall(G_ARGC) == 3) {
		char args[MAX_STRING_CHARS] = { 0 };
		g_syscall(G_ARGV, 1, args, sizeof(args));
		clientId = Lmdp_ClientIdFromString(NULL, args);

		if (clientId > -1){
			g_syscall(G_ARGV, 2, args, sizeof(args));
			LMDP_CHATF(-1, "%s: %s", CLIENT_FROM_NUM(clientId)->pers.netname, args);
		}
	}
	QMM_RET_SUPERCEDE(1);
}

// List of console commands
// These commands work via rcon or from the server console
svcmd_t consoleCmds[ ] =
{
	// command				function
	{ "sayas",				SvCmd_sayas_f		},
};
static size_t numConsoleCmds = ARRAY_LEN(consoleCmds);

// ConsoleCommand
// Handle commands from console or rcon
int ConsoleCommand( void )
{
	char cmd[MAX_TOKEN_CHARS] = {0};

	g_syscall(G_ARGV, 0, cmd, sizeof(cmd));
	
	// search console commands
	svcmd_t *command = (svcmd_t*)bsearch( cmd, consoleCmds, numConsoleCmds, sizeof( consoleCmds[ 0 ] ), cmdcmp );
	if (command)
		return command->cmdHandler();

	// not a plugin command
	QMM_RET_IGNORED(1);
}

#pragma endregion