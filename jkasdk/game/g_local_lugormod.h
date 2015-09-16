// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_local.h -- local definitions for game module

#include "q_shared.h"
#include "bg_public.h"
#include "bg_vehicles.h"
#include "g_public.h"

#ifndef __LCC__
#define GAME_INLINE ID_INLINE
#else
#define GAME_INLINE //none
#endif

//npc stuff
#include "b_public.h"

extern int gPainMOD;
extern int gPainHitLoc;
extern vec3_t gPainPoint;

//==================================================================

// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"basejka_mod"

#define BODY_QUEUE_SIZE		8

#ifndef INFINITE
#define INFINITE			1000000
#endif

#define	FRAMETIME			100					// msec
#define	CARNAGE_REWARD_TIME	3000
#define REWARD_SPRITE_TIME	2000

#define	INTERMISSION_DELAY_TIME	1000
#define	SP_INTERMISSION_DELAY_TIME	5000

//primarily used by NPCs
#define	START_TIME_LINK_ENTS		FRAMETIME*1 // time-delay after map start at which all ents have been spawned, so can link them
#define	START_TIME_FIND_LINKS		FRAMETIME*2 // time-delay after map start at which you can find linked entities
#define	START_TIME_MOVERS_SPAWNED	FRAMETIME*2 // time-delay after map start at which all movers should be spawned
#define	START_TIME_REMOVE_ENTS		FRAMETIME*3 // time-delay after map start to remove temporary ents
#define	START_TIME_NAV_CALC			FRAMETIME*4 // time-delay after map start to connect waypoints and calc routes
#define	START_TIME_FIND_WAYPOINT	FRAMETIME*5 // time-delay after map start after which it's okay to try to find your best waypoint

// gentity->flags
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_DROPPED_ITEM			0x00001000
#define FL_NO_BOTS				0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS			0x00004000	// spawn point just for bots
#define FL_FORCE_GESTURE		0x00008000	// force gesture on client
#define FL_INACTIVE				0x00010000	// inactive
#define FL_NAVGOAL				0x00020000	// for npc nav stuff
#define	FL_DONT_SHOOT			0x00040000
#define FL_SHIELDED				0x00080000
#define FL_UNDYING				0x00100000	// takes damage down to 1, but never dies

//ex-eFlags -rww
#define	FL_BOUNCE				0x00100000		// for missiles
#define	FL_BOUNCE_HALF			0x00200000		// for missiles
#define	FL_BOUNCE_SHRAPNEL		0x00400000		// special shrapnel flag

//vehicle game-local stuff -rww
#define	FL_VEH_BOARDING			0x00800000		// special shrapnel flag

//breakable flags -rww
#define FL_DMG_BY_SABER_ONLY		0x01000000 //only take dmg from saber
#define FL_DMG_BY_HEAVY_WEAP_ONLY	0x02000000 //only take dmg from explosives

#define FL_BBRUSH					0x04000000 //I am a breakable brush

#ifndef FINAL_BUILD
#define DEBUG_SABER_BOX
#endif

#define	MAX_G_SHARED_BUFFER_SIZE		8192
extern char gSharedBuffer[MAX_G_SHARED_BUFFER_SIZE];

// movers are things like doors, plats, buttons, etc
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1
} moverState_t;

#define SP_PODIUM_MODEL		"models/mapobjects/podium/podium4.md3"

typedef enum
{
	HL_NONE = 0,
	HL_FOOT_RT,
	HL_FOOT_LT,
	HL_LEG_RT,
	HL_LEG_LT,
	HL_WAIST,
	HL_BACK_RT,
	HL_BACK_LT,
	HL_BACK,
	HL_CHEST_RT,
	HL_CHEST_LT,
	HL_CHEST,
	HL_ARM_RT,
	HL_ARM_LT,
	HL_HAND_RT,
	HL_HAND_LT,
	HL_HEAD,
	HL_GENERIC1,
	HL_GENERIC2,
	HL_GENERIC3,
	HL_GENERIC4,
	HL_GENERIC5,
	HL_GENERIC6,
	HL_MAX
};

//============================================================================
extern void *precachedKyle;
extern void *g2SaberInstance;

extern qboolean gEscaping;
extern int gEscapeTime;

// Lugormod Profession Data Structure
typedef struct
{
	int prof;
	int level;
} ProfessionData_t;

// Lugormod Inventory Data Structure
typedef struct
{
	unsigned int count;
	struct iObjectList_s
	{
		struct iObjectData_s
		{
			struct iObject_s
			{
				char	*type;			// iobject type (downcount, upcount, medpack ...)
			} *object;

			char	*data1;
			char	*name;				// iobject name
			qboolean noCombine;			// combine iobjects
			qboolean noDelete;			// destroy iobjects
			qboolean noAutoDelete;		// automatic deletion

			struct iObjectProp_s
			{
				char *propertyName;
				int max;
				int count;
				int uses;
			} *propertyInfo;
		} *objectData;
	} *list;							// list of inventory entries
} InventoryData_t;

// Lugormod AuthData Structure
typedef struct
{
	unsigned int count;
	struct authList_s
	{
		struct authData_s
		{
			char	*filename;
			int		auth1;
			int		rank;
			int		auth2;
			int		flags;
		} *auth;
	} *list;							// list of authfiles
} AuthData_t;

// Lugormod SpawnData Data Structure
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

typedef struct Account_s{
	char *username;
	char *name;
	unsigned int pwChksum;
	char *secCode;

	int id;
	int logins;
	unsigned int lastLogin;
	int lastIP;	// IP_t

	int time;
	int score;
	int credits;
	int flags;

	ProfessionData_t*	profession;
	AuthData_t*			auth;
	lmdDummy_t*			Accounts_StatData;
	lmdDummy_t*			Accounts_Property;
	InventoryData_t*	inventory;
	lmdDummy_t*			Accounts_Friends;
	lmdDummy_t*			Accounts_Custom;

	int modifiedTime;
} Account_t;

struct gentity_s {
	//rww - entstate must be first, to correspond with the bg shared entity structure
	entityState_t	s;				// communicated by server to clients
	playerState_t	*playerState;	//ptr to playerstate if applicable (for bg ents)
	Vehicle_t		*m_pVehicle; //vehicle data
	void			*ghoul2; //g2 instance
	int				localAnimIndex; //index locally (game/cgame) to anim data for this skel
	vec3_t			modelScale; //needed for g2 collision

	//From here up must be the same as centity_t/bgEntity_t

	entityShared_t	r;				// shared by both the server system and game

	//rww - these are shared icarus things. They must be in this order as well in relation to the entityshared structure.
	int				taskID[NUM_TIDS];
	parms_t			*parms;
	char			*behaviorSet[NUM_BSETS];
	char			*script_targetname;
	int				delayScriptTime;
	char			*fullName;

	//rww - targetname and classname are now shared as well. ICARUS needs access to them.
	char			*targetname;
	char			*classname;			// set in QuakeEd

	//rww - and yet more things to share. This is because the nav code is in the exe because it's all C++.
	int				waypoint;			//Set once per frame, if you've moved, and if someone asks
	int				lastWaypoint;		//To make sure you don't double-back
	int				lastValidWaypoint;	//ALWAYS valid -used for tracking someone you lost
	int				noWaypointTime;		//Debouncer - so don't keep checking every waypoint in existance every frame that you can't find one
	int				combatPoint;
	int				failedWaypoints[MAX_FAILED_NODES];
	int				failedWaypointCheckTime;

	int				next_roff_time; //rww - npc's need to know when they're getting roff'd

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s	*client;			// NULL if not a client

	gNPC_t		*NPC;//Only allocated if the entity becomes an NPC
	int			cantHitEnemyCounter;//HACK - Makes them look for another enemy on the same team if the one they're after can't be hit

	qboolean	noLumbar; //see note in cg_local.h

	qboolean	inuse;

	int			lockCount; //used by NPCs

	int			spawnflags;			// set in QuakeEd

	int			teamnodmg;			// damage will be ignored if it comes from this team

	char		*roffname;			// set in QuakeEd
	char		*rofftarget;		// set in QuakeEd

	char		*healingclass; //set in quakeed
	char		*healingsound; //set in quakeed
	int			healingrate; //set in quakeed
	int			healingDebounce; //debounce for generic object healing shiz

	char		*ownername;

	int			objective;
	int			side;

	int			passThroughNum;		// set to index to pass through (+1) for missiles

	int			aimDebounceTime;
	int			painDebounceTime;
	int			attackDebounceTime;
	int			alliedTeam;			// only useable by this team, never target this team

	int			roffid;				// if roffname != NULL then set on spawn

	qboolean	neverFree;			// if true, FreeEntity will only unlink
	// bodyque uses this

	int			flags;				// FL_* variables

	char		*model;
	char		*model2;
	int			freetime;			// level.time when the object was freed

	int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
	qboolean	freeAfterEvent;
	qboolean	unlinkAfterEvent;

	qboolean	physicsObject;		// if true, it can be pushed by movers and fall off edges
	// all game items are physicsObjects,
	float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce
	int			clipmask;			// brushes with this content value will be collided against
	// when moving.  items and corpses do not collide against
	// players, for instance

	//Only used by NPC_spawners
	char		*NPC_type;
	char		*NPC_targetname;
	char		*NPC_target;

	// movers
	moverState_t moverState;
	int			soundPos1;
	int			sound1to2;
	int			sound2to1;
	int			soundPos2;
	int			soundLoop;
	gentity_t	*parent;
	gentity_t	*nextTrain;
	gentity_t	*prevTrain;
	vec3_t		pos1, pos2;

	//for npc's
	vec3_t		pos3;

	char		*lmdField1;
	char		*lmdField2;
	char		*lmdField3;

	char		*message;	// 1096

	int			timestamp;		// body queue sinking, etc	1100

	float		angle;			// set in editor, -1 = up, -2 = down	1104
	char		*target;	// 1108
	char		*target2;	// 1112
	char		*target3;		//For multiple targets, not used for firing/triggering/using, though, only for path branches	1116
	char		*target4;		//For multiple targets, not used for firing/triggering/using, though, only for path branches	1120
	char		*target5;		//mainly added for siege items	1124
	char		*target6;		//mainly added for siege items	1128

	char		*team;	// 1132
	char		*targetShaderName;		// 1136
	char		*targetShaderNewName;	// 1140
	gentity_t	*target_ent;	// 1144

	char		*closetarget;	// 1148
	char		*opentarget;	// 1152
	char		*paintarget;	// 1156

	char		*goaltarget;	// 1160
	char		*idealclass;	// 1164

	float		radius;	// 1168

	int			maxHealth; //used as a base for crosshair health display 1172

	float		speed;		// 1176
	vec3_t		movedir;	// 1180 1184 1188
	float		mass;		// 1192
	int			setTime;	// 1196

	//Think Functions
	int			nextthink;	// 1200
	void		(*think)(gentity_t *self);	// 1204
	void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint 1208
	void		(*blocked)(gentity_t *self, gentity_t *other);	//  1212
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);	// 1216
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);	// 1220
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage);	// 1224
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod); // 1228

	qboolean ent_1232;	// payable
	int		ent_1236;
	int		ent_1240;
	int		ent_1244;
	char	*examine;

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel
	int			last_move_time;

	//Health and damage fields
	int			health;
	qboolean	takedamage;
	material_t	material;

	int			damage;
	int			dflags;
	int			splashDamage;	// quad will increase this without increasing radius
	int			splashRadius;
	int			methodOfDeath;
	int			splashMethodOfDeath;

	int			locationDamage[HL_MAX];		// Damage accumulated on different body locations

	int			count;
	int			bounceCount;
	qboolean	alt_fire;

	gentity_t	*chain;
	gentity_t	*enemy;
	gentity_t	*lastEnemy;
	gentity_t	*activator;		// 1416
	gentity_t	*teamchain;		// next entity in team	// 1420
	gentity_t	*teammaster;	// master of the team	// 1424

	int			watertype;		// 1428
	int			waterlevel;		// 1432

	int			noise_index;	// 1436

	// timing variables
	float		wait;	// 1440
	float		random;	// 1444
	int			delay;	// 1448

	//generic values used by various entities for different purposes.
	int			genericValue1;	// 1452
	int			genericValue2;	// 1456
	int			genericValue3;	// 1460
	int			genericValue4;	// 1464
	int			genericValue5;	// 1468
	int			genericValue6;	// 1472
	int			genericValue7;	// 1476
	int			genericValue8;	// 1480
	int			genericValue9;	// 1484
	int			genericValue10; // 1488
	int			genericValue11;
	int			genericValue12;
	int			genericValue13;
	int			genericValue14;
	int			genericValue15;

	char		*soundSet;

	qboolean	isSaberEntity;

	int			damageRedirect; //if entity takes damage, redirect to..
	int			damageRedirectTo; //this entity number	1440

	vec3_t		epVelocity;		// 1444
	float		epGravFactor;	// 1448

	gitem_t		*item;			// for bonus items 1452

	vec3_t offadelta;	// 1448 1452 1456
	char* ent_1560;
	char* ent_1564;
	char* ent_1568;

	char* GenericStrings[9]; // 1572
	//char* cmd;		// cmd, skin, zone, startdepomessage
	//char* cmd2;		// cmd2
	//char* cmd3;		// cmd3, pickupmessge
	//char* cmd4;		// cmd4, dropmessage
	//char* cmd5;		// cmd5, startdepomessage
	//char* cmd6;		// cmd6, depositmessage
	//char* message7;		// resetmessage
	//char* message8;		// usetarget, spawnmessage
	//char* globalTarget;
	char* ent_1608;
	char* ent_1612;
	char* ent_1616;
	char* ent_1620;
	char* ent_1624;
	char* ent_1628;
	char* ent_1632;
	char* ent_1636;
	char* ent_1640;
	char* ent_1644;
	char* ent_1648;
	vec3_t ent_1652;
	char* ent_1664;
	char* ent_1668;
	SpawnData_t *spawnData; // Info connecting the ent to loading / saving 1672
	qboolean logical; //if we are a logic entity. 1676
	char *group; // 1680
	struct UseReq_s{
		int profession; // 1684
		int level;
		int levelMax;
		int authLevel;
		int flags;
		struct customSkill_s {
			char *skill;
			char *value;
			int compare;
		}customSkill;
		char *prop;
		int credits;
	}UseReq;
	qboolean isAutoTargeted; //we were given a targetname automatically
};

#define DAMAGEREDIRECT_HEAD		1
#define DAMAGEREDIRECT_RLEG		2
#define DAMAGEREDIRECT_LLEG		3

typedef enum {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
};
typedef int clientConnected_t;

typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum {
	TEAM_BEGIN,		// Beginning a team game, spawn at base
	TEAM_ACTIVE		// Now actively playing
} playerTeamStateState_t;

typedef struct {
	playerTeamStateState_t	state;

	int			location;

	int			captures;
	int			basedefense;
	int			carrierdefense;
	int			flagrecovery;
	int			fragcarrier;
	int			assists;

	float		lasthurtcarrier;
	float		lastreturnedflag;
	float		flagsince;
	float		lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct {
	team_t		sessionTeam;
	int			spectatorTime;		// for determining next-in-line to play
	spectatorState_t	spectatorState;
	int			spectatorClient;	// for chasecam and follow mode
	int			wins, losses;		// tournament stats
	int			selectedFP;			// check against this, if doesn't match value in playerstate then update userinfo
	int			saberLevel;			// similar to above method, but for current saber attack level
	qboolean	setForce;			// set to true once player is given the chance to set force powers
	int			updateUITime;		// only update userinfo for FP/SL if < level.time
	qboolean	teamLeader;			// true when this client is a team leader
	char		siegeClass[64];
	char		saberType[64];
	char		saber2Type[64];
	int			duelTeam;
	int			siegeDesiredTeam;
	struct 	{
		int accountNumber;
		byte ipBytes[4];
		AuthData_t* tempAuthData;
		vec3_t sess_2024;
	}LMD;
} clientSession_t;

// playerstate mGameFlags
#define	PSG_VOTED				(1<<0)		// already cast a vote
#define PSG_TEAMVOTED			(1<<1)		// already cast a team vote

//
#define MAX_NETNAME			36
#define	MAX_VOTE_COUNT		3

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct {
	clientConnected_t	connected;
	usercmd_t	cmd;				// we would lose angles if not persistant
	qboolean	localClient;		// true if "ip" info key is "localhost"
	qboolean	initialSpawn;		// the first spawn should be at a cool location
	qboolean	predictItemPickup;	// based on cg_predictItems userinfo
	qboolean	pmoveFixed;			//
	char		netname[MAX_NETNAME];
	int			netnameTime;				// Last time the name was changed
	int			maxHealth;			// for handicapping
	int			enterTime;			// level.time the client entered the game
	playerTeamState_t teamState;	// status in teamplay games
	int			voteCount;			// to prevent people from constantly calling votes
	int			teamVoteCount;		// to prevent people from constantly calling votes
	qboolean	teamInfo;			// send team overlay updates?

	struct {
		int clientFlags;	// 1 = Shutup, 2 = Nocaps, 4 = Novote, 8 = King, 16 = ionlyduel, 32 = Hide Admin
		lmdDummy_t* lmd_1712;
		int teamTime;		// time team switched?
		lmdDummy_t* lmd_1720;
		lmdDummy_t* lmd_1724;
		int buddiesMask;
		lmdDummy_t* lmd_1732;
		lmdDummy_t* lmd_1736;
		lmdDummy_t* lmd_1740;
		lmdDummy_t* lmd_1744;
		Account_t* account;		// 1748
		lmdDummy_t* lmd_1752;	// actions
		lmdDummy_t* lmd_1756;	// actions
		int chatModeSay;	// none=0, all, team, admins, close, buddies, friends 1760
		int chatModeTeam;	// 1764
	} LMD;
} clientPersistant_t;

typedef struct renderInfo_s
{
	//In whole degrees, How far to let the different model parts yaw and pitch
	int		headYawRangeLeft;
	int		headYawRangeRight;
	int		headPitchRangeUp;
	int		headPitchRangeDown;

	int		torsoYawRangeLeft;
	int		torsoYawRangeRight;
	int		torsoPitchRangeUp;
	int		torsoPitchRangeDown;

	int		legsFrame;
	int		torsoFrame;

	float	legsFpsMod;
	float	torsoFpsMod;

	//Fields to apply to entire model set, individual model's equivalents will modify this value
	vec3_t	customRGB;//Red Green Blue, 0 = don't apply
	int		customAlpha;//Alpha to apply, 0 = none?

	//RF?
	int			renderFlags;

	//
	vec3_t		muzzlePoint;
	vec3_t		muzzleDir;
	vec3_t		muzzlePointOld;
	vec3_t		muzzleDirOld;
	//vec3_t		muzzlePointNext;	// Muzzle point one server frame in the future!
	//vec3_t		muzzleDirNext;
	int			mPCalcTime;//Last time muzzle point was calced

	//
	float		lockYaw;//

	//
	vec3_t		headPoint;//Where your tag_head is
	vec3_t		headAngles;//where the tag_head in the torso is pointing
	vec3_t		handRPoint;//where your right hand is
	vec3_t		handLPoint;//where your left hand is
	vec3_t		crotchPoint;//Where your crotch is
	vec3_t		footRPoint;//where your right hand is
	vec3_t		footLPoint;//where your left hand is
	vec3_t		torsoPoint;//Where your chest is
	vec3_t		torsoAngles;//Where the chest is pointing
	vec3_t		eyePoint;//Where your eyes are
	vec3_t		eyeAngles;//Where your eyes face
	int			lookTarget;//Which ent to look at with lookAngles
	lookMode_t	lookMode;
	int			lookTargetClearTime;//Time to clear the lookTarget
	int			lastVoiceVolume;//Last frame's voice volume
	vec3_t		lastHeadAngles;//Last headAngles, NOT actual facing of head model
	vec3_t		headBobAngles;//headAngle offsets
	vec3_t		targetHeadBobAngles;//head bob angles will try to get to targetHeadBobAngles
	int			lookingDebounceTime;//When we can stop using head looking angle behavior
	float		legsYaw;//yaw angle your legs are actually rendering at

	//for tracking legitimate bolt indecies
	void		*lastG2; //if it doesn't match ent->ghoul2, the bolts are considered invalid.
	int			headBolt;
	int			handRBolt;
	int			handLBolt;
	int			torsoBolt;
	int			crotchBolt;
	int			footRBolt;
	int			footLBolt;
	int			motionBolt;

	int			boltValidityTime;
} renderInfo_t;

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t	ps;				// communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;

	saberInfo_t	saber[MAX_SABERS];
	void		*weaponGhoul2[MAX_SABERS];

	int			tossableItemDebounce;

	int			bodyGrabTime;
	int			bodyGrabIndex;

	int			pushEffectTime;

	int			invulnerableTimer;

	int			saberCycleQueue;

	int			legsAnimExecute;
	int			torsoAnimExecute;
	qboolean	legsLastFlip;
	qboolean	torsoLastFlip;

	qboolean	readyToExit;		// wishes to leave the intermission

	qboolean	noclip;

	int			lastCmdTime;		// level.time of last usercmd_t, for EF_CONNECTION
	// we can't just use pers.lastCommand.time, because
	// of the g_sycronousclients case
	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	vec3_t		oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation
	qboolean	damage_fromWorld;	// if true, don't use the damage_from vector

	int			damageBoxHandle_Head; //entity number of head damage box
	int			damageBoxHandle_RLeg; //entity number of right leg damage box
	int			damageBoxHandle_LLeg; //entity number of left leg damage box

	int			accurateCount;		// for "impressive" reward sound

	int			accuracy_shots;		// total number of shots
	int			accuracy_hits;		// total number of hits

	//
	int			lastkilled_client;	// last client that this client killed
	int			lasthurt_client;	// last client that damaged this client
	int			lasthurt_mod;		// type of damage the client did

	// timers
	int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
	int			inactivityTime;		// kick players when time > this
	qboolean	inactivityWarning;	// qtrue if the five seoond warning has been given
	int			rewardTime;			// clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int			airOutTime;

	int			lastKillTime;		// for multiple kill rewards

	qboolean	fireHeld;			// used for hook
	gentity_t	*hook;				// grapple hook if out

	int			switchTeamTime;		// time the player switched teams

	int			switchDuelTeamTime;		// time the player switched duel teams

	int			switchClassTime;	// class changed debounce timer

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int			timeResidual;

	char		*areabits;

	int			g2LastSurfaceHit; //index of surface hit during the most recent ghoul2 collision performed on this client.
	int			g2LastSurfaceTime; //time when the surface index was set (to make sure it's up to date)

	int			corrTime;

	vec3_t		lastHeadAngles;
	int			lookTime;

	int			brokenLimbs;

	qboolean	noCorpse; //don't leave a corpse on respawn this time.				// 6708

	int			jetPackTime;														// 6712

	qboolean	jetPackOn;			// 6716
	int			jetPackToggleTime;	// 6720
	int			jetPackDebRecharge;	// 6724
	int			jetPackDebReduce;	// 6728

	int			cloakToggleTime;	// 6732
	int			cloakDebRecharge;	// 6736
	int			cloakDebReduce;		// 6740

	int			saberStoredIndex; //stores saberEntityNum from playerstate for when it's set to 0 (indicating saber was knocked out of the air) //7216

	int			saberKnockedTime; //if saber gets knocked away, can't pull it back until this value is < level.time //7220

	vec3_t		olderSaberBase; //Set before lastSaberBase_Always, to whatever lastSaberBase_Always was previously //7224
	qboolean	olderIsValid;	//is it valid? //7228

	vec3_t		lastSaberDir_Always; //every getboltmatrix, set to saber dir //7232
	vec3_t		lastSaberBase_Always; //every getboltmatrix, set to saber base // 7236
	int			lastSaberStorageTime; //server time that the above two values were updated (for making sure they aren't out of date) //7240

	qboolean	hasCurrentPosition;	//are lastSaberTip and lastSaberBase valid? //7244

	int			dangerTime;		// level.time when last attack occured //7248

	int			idleTime;		//keep track of when to play an idle anim on the client.		//7252

	int			idleHealth;		//stop idling if health decreases	//7256
	vec3_t		idleViewAngles;	//stop idling if viewangles change	//7260

	int			forcePowerSoundDebounce; //if > level	.time, don't do certain sound events again (drain sound, absorb sound, etc) // 7264

	char		modelname[MAX_QPATH]; //7268

	qboolean	fjDidJump;	//7272

	qboolean	ikStatus;	//7276

	int			throwingIndex;	//7280
	int			beingThrown;	//7284
	int			doingThrow;	//7288

	float		hiddenDist;//How close ents have to be to pick you up as an enemy	//7292
	vec3_t		hiddenDir;//Normalized direction in which NPCs can't see you (you are hidden) //7296

	renderInfo_t	renderInfo; //7300

	//mostly NPC stuff:
	npcteam_t	playerTeam;	//7304
	npcteam_t	enemyTeam;	//7308
	char		*squadname; //7312
	gentity_t	*team_leader; //7316
	gentity_t	*leader;	//7320
	gentity_t	*follower;	//7324
	int			numFollowers;	//7328
	gentity_t	*formationGoal; //7332
	int			nextFormGoal;	//7336
	class_t		NPC_class;		//7340

	vec3_t		pushVec;		//7344
	int			pushVecTime;	//7348

	int			siegeClass;		//7352
	int			holdingObjectiveItem; //7356

	//time values for when being healed/supplied by supplier class
	int			isMedHealed; //7360
	int			isMedSupplied; //7364

	//seperate debounce time for refilling someone's ammo as a supplier
	int			medSupplyDebounce; //7368

	//used in conjunction with ps.hackingTime
	int			isHacking; //7372
	vec3_t		hackingAngles; //7376

	//debounce time for sending extended siege data to certain classes
	int			siegeEDataSend;	// 7380

	int			ewebIndex; //index of e-web gun if spawned //7384
	int			ewebTime; //e-web use debounce	//7388
	int			ewebHealth; //health of e-web (to keep track between deployments)	//7392

	int			inSpaceIndex; //ent index of space trigger if inside one	//7396
	int			inSpaceSuffocation; //suffocation timer			// 7400

	int			tempSpectate; //time to force spectator mode	// 7404

	//keep track of last person kicked and the time so we don't hit multiple times per kick
	int			jediKickIndex;		//7408
	int			jediKickTime;		//7412

	//special moves (designed for kyle boss npc, but useable by players in mp)
	int			grappleIndex;		//7416
	int			grappleState;		//7420

	int			solidHack;			//7424

	int			noLightningTime;	//7428

	unsigned	mGameFlags;			//7432

	//fallen duelist
	qboolean	iAmALoser;			//7436

	int			lastGenCmd;			//7440
	int			lastGenCmdTime;		//7440

	char* cl_7444; // duel end - gametype?
	char* cl_7448; // duel end - gametype?
	char* cl_7452;
	char* cl_7456;

	char* cl_7460;
	char* cl_7464;
	char* cl_7468;
	char* cl_7472;
	char* cl_7476;

	char* cl_7480;
	char* cl_7484;
	char* cl_7488;
	char* cl_7492;
	int challengeType;

	char* cl_7500;
	int challengeBet;
	char* cl_7508;
	char* cl_7512;
	char* cl_7516;

	char* cl_7520;
	char* cl_7524;
	char* cl_7528;
	char* cl_7532;
	char* cl_7536;

	struct stashData_s
	{
		int	EntityNum;	// the entity number of the stash spawner
	} *stash;

	int playerFlags;	// lmd_flagplayer flags
	char* cl_7548;
	char* cl_7552;
	char* cl_7556;

	char* cl_7560;
	char* cl_7564;
	char* cl_7568;
	char* cl_7572;
	char* cl_7576;

	char* cl_7580;
	char* cl_7584;
	char* cl_7588;
	char* cl_7592;
	char* cl_7596;

	char* cl_7600;
	char* cl_7604;
	char* cl_7608;
	char* cl_7612;
	char* cl_7616;

	char* cl_7620;
	char* cl_7624;

	int gravity;
	int gravityTime;

	char* cl_7636;

	int speed;
	int speedTime; // 7644

	char* cl_7648;
	char* cl_7652;
	char* cl_7656;
	char* cl_7660;
	lmdDummy_t* cl_7664; // verification stuff
	lmdDummy_t* cl_7668; // verification stuff
	char* cl_7672;
	char* cl_7676;

	vec3_t cl_7680;

	char* cl_7692;
	char* cl_7696;
	char* cl_7700;
	char* cl_7704;	// hacktime stash_depo
	char* cl_7708;
};

//Interest points

#define MAX_INTEREST_POINTS		64

typedef struct
{
	vec3_t		origin;
	char		*target;
} interestPoint_t;

//Combat points

#define MAX_COMBAT_POINTS		512

typedef struct
{
	vec3_t		origin;
	int			flags;
	//	char		*NPC_targetname;
	//	team_t		team;
	qboolean	occupied;
	int			waypoint;
	int			dangerTime;
} combatPoint_t;

// Alert events

#define	MAX_ALERT_EVENTS	32

typedef enum
{
	AET_SIGHT,
	AET_SOUND,
} alertEventType_e;

typedef enum
{
	AEL_MINOR,			//Enemy responds to the sound, but only by looking
	AEL_SUSPICIOUS,		//Enemy looks at the sound, and will also investigate it
	AEL_DISCOVERED,		//Enemy knows the player is around, and will actively hunt
	AEL_DANGER,			//Enemy should try to find cover
	AEL_DANGER_GREAT,	//Enemy should run like hell!
} alertEventLevel_e;

typedef struct alertEvent_s
{
	vec3_t				position;	//Where the event is located
	float				radius;		//Consideration radius
	alertEventLevel_e	level;		//Priority level of the event
	alertEventType_e	type;		//Event type (sound,sight)
	gentity_t			*owner;		//Who made the sound
	float				light;		//ambient light level at point
	float				addLight;	//additional light- makes it more noticable, even in darkness
	int					ID;			//unique... if get a ridiculous number, this will repeat, but should not be a problem as it's just comparing it to your lastAlertID
	int					timestamp;	//when it was created
} alertEvent_t;

//
// this structure is cleared as each map is entered
//
typedef struct
{
	char	targetname[MAX_QPATH];
	char	target[MAX_QPATH];
	char	target2[MAX_QPATH];
	char	target3[MAX_QPATH];
	char	target4[MAX_QPATH];
	int		nodeID;
} waypointData_t;

typedef struct {
	struct gclient_s	*clients;		// [maxclients]

	struct gentity_s	*gentities;
	int			gentitySize;
	int			num_entities;		// current number, <= MAX_GENTITIES

	int			warmupTime;			// restart match at this time

	fileHandle_t	logFile;

	// store latched cvars here that we want to get at often
	int			maxclients;

	int			framenum;
	int			time;					// in msec
	int			previousTime;			// so movers can back up when blocked

	int			startTime;				// level.time the map was started

	int			teamScores[TEAM_NUM_TEAMS];
	int			lastTeamLocationTime;		// last time of client team location update

	qboolean	newSession;				// don't use any old session data, because
	// we changed gametype

	qboolean	restarted;				// waiting for a map_restart to fire

	int			numConnectedClients;
	int			numNonSpectatorClients;	// includes connecting clients
	int			numPlayingClients;		// connected, non-spectators
	int			sortedClients[MAX_CLIENTS];		// sorted by score
	int			follow1, follow2;		// clientNums for auto-follow spectators

	int			snd_fry;				// sound index for standing in lava

	int			snd_hack;				//hacking loop sound
	int			snd_medHealed;			//being healed by supply class
	int			snd_medSupplied;		//being supplied by supply class

	int			warmupModificationCount;	// for detecting if g_warmup is changed

	// voting state
	char		voteString[MAX_STRING_CHARS];
	char		voteDisplayString[MAX_STRING_CHARS];
	int			voteTime;				// level.time vote was called
	int			voteExecuteTime;		// time the vote is executed
	int			voteYes;
	int			voteNo;
	int			numVotingClients;		// set by CalculateRanks

	qboolean	votingGametype;
	int			votingGametypeTo;

	// team voting state
	char		teamVoteString[2][MAX_STRING_CHARS];
	int			teamVoteTime[2];		// level.time vote was called
	int			teamVoteYes[2];
	int			teamVoteNo[2];
	int			numteamVotingClients[2];// set by CalculateRanks

	// spawn variables
	qboolean	spawning;				// the G_Spawn*() functions are valid
	int			numSpawnVars;
	char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int			numSpawnVarChars;
	char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int			intermissionQueued;		// intermission was qualified, but
	// wait INTERMISSION_DELAY_TIME before
	// actually going there so the last
	// frag can be watched.  Disable future
	// kills during this delay
	int			intermissiontime;		// time the intermission was started
	char		*changemap;
	qboolean	readyToExit;			// at least one client wants to exit
	int			exitTime;
	vec3_t		intermission_origin;	// also used for spectator spawns
	vec3_t		intermission_angle;

	qboolean	locationLinked;			// target_locations get linked
	gentity_t	*locationHead;			// head of the location list
	int			bodyQueIndex;			// dead bodies
	gentity_t	*bodyQue[BODY_QUEUE_SIZE];
	int			portalSequence;

	alertEvent_t	alertEvents[ MAX_ALERT_EVENTS ];
	int				numAlertEvents;
	int				curAlertID;

	AIGroupInfo_t	groups[MAX_FRAME_GROUPS];

	//Interest points- squadmates automatically look at these if standing around and close to them
	interestPoint_t	interestPoints[MAX_INTEREST_POINTS];
	int			numInterestPoints;

	//Combat points- NPCs in bState BS_COMBAT_POINT will find their closest empty combat_point
	combatPoint_t	combatPoints[MAX_COMBAT_POINTS];
	int			numCombatPoints;

	//rwwRMG - added:
	int			mNumBSPInstances;
	int			mBSPInstanceDepth;
	vec3_t		mOriginAdjust;
	float		mRotationAdjust;
	char		*mTargetAdjust;

	char		mTeamFilter[MAX_QPATH];
} level_locals_t;

// damage flags
#define DAMAGE_NORMAL				0x00000000	// No flags set.
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	// armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000004	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000008  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION	0x00000010  // armor, shields, invulnerability, and godmode have no effect
//JK2 flags
#define DAMAGE_EXTRA_KNOCKBACK		0x00000040	// add extra knockback to this damage
#define DAMAGE_DEATH_KNOCKBACK		0x00000080	// only does knockback on death of target
#define DAMAGE_IGNORE_TEAM			0x00000100	// damage is always done, regardless of teams
#define DAMAGE_NO_DAMAGE			0x00000200	// do no actual damage but react as if damage was taken
#define DAMAGE_HALF_ABSORB			0x00000400	// half shields, half health
#define DAMAGE_HALF_ARMOR_REDUCTION	0x00000800	// This damage doesn't whittle down armor as efficiently.
#define DAMAGE_HEAVY_WEAP_CLASS		0x00001000	// Heavy damage
#define DAMAGE_NO_HIT_LOC			0x00002000	// No hit location
#define DAMAGE_NO_SELF_PROTECTION	0x00004000	// Dont apply half damage to self attacks
#define DAMAGE_NO_DISMEMBER			0x00008000	// Dont do dismemberment
#define DAMAGE_SABER_KNOCKBACK1		0x00010000	// Check the attacker's first saber for a knockbackScale
#define DAMAGE_SABER_KNOCKBACK2		0x00020000	// Check the attacker's second saber for a knockbackScale
#define DAMAGE_SABER_KNOCKBACK1_B2	0x00040000	// Check the attacker's first saber for a knockbackScale2
#define DAMAGE_SABER_KNOCKBACK2_B2	0x00080000	// Check the attacker's second saber for a knockbackScale2

#define SPF_BUTTON_USABLE		1
#define SPF_BUTTON_FPUSHABLE	2

//
// g_misc.c
//
#define MAX_REFNAME	32
#define	START_TIME_LINK_ENTS		FRAMETIME*1

#define	RTF_NONE	0
#define	RTF_NAVGOAL	0x00000001

typedef struct reference_tag_s
{
	char		name[MAX_REFNAME];
	vec3_t		origin;
	vec3_t		angles;
	int			flags;	//Just in case
	int			radius;	//For nav goals
	qboolean	inuse;
} reference_tag_t;

// ai_main.c
#define MAX_FILEPATH			144

//bot settings
typedef struct bot_settings_s
{
	char personalityfile[MAX_FILEPATH];
	float skill;
	char team[MAX_FILEPATH];
} bot_settings_t;

#include "g_team.h" // teamplay specific stuff

extern	level_locals_t	level;
extern	gentity_t		g_entities[MAX_GENTITIES];

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))
