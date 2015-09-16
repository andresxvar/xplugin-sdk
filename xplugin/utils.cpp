#include "utils.h"
#include "cmds.h"
#include "interface.h"
#include "qmmapi.h"

#pragma region Math
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees)
{
	float q[3];
	float q3;
	float t[3];
	float t3;

	{
		float hrad;
		float s;

		hrad = DEG2RAD(degrees) / 2;
		s = sin(hrad);
		VectorScale(dir, s, q);
		q3 = cos(hrad);
	}

	CrossProduct(q, point, t);
	VectorMA(t, q3, point, t);
	t3 = DotProduct(q, point);

	CrossProduct(q, t, dst);
	VectorMA(dst, t3, q, dst);
	VectorMA(dst, q3, t, dst);
}


void vectoangles(const vec3_t value1, vec3_t angles) {
	float	forward;
	float	yaw, pitch;

	if (value1[1] == 0 && value1[0] == 0) {
		yaw = 0;
		if (value1[2] > 0) {
			pitch = 90;
		}
		else {
			pitch = 270;
		}
	}
	else {
		if (value1[0]) {
			yaw = (atan2(value1[1], value1[0]) * 180 / M_PI);
		}
		else if (value1[1] > 0) {
			yaw = 90;
		}
		else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}

		forward = sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = (atan2(value1[2], forward) * 180 / M_PI);
		if (pitch < 0) {
			pitch += 360;
		}
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

/*
AngleSubtract
Always returns a value from -180 to 180
*/
float AngleSubtract(float a1, float a2) {
	float	a;

	a = a1 - a2;
	a = fmod(a, 360);//chop it down quickly, then level it out
	while (a > 180) {
		a -= 360;
	}
	while (a < -180) {
		a += 360;
	}
	return a;
}

/*
AngleNormalize360
returns angle normalized to the range [0 <= angle < 360]
*/
float AngleNormalize360(float angle) {
	return (360.0 / 65536) * ((int)(angle * (65536 / 360.0)) & 65535);
}

typedef unsigned __int32 uint32_t;
static uint32_t	holdrand = 0x89abcdef; // 64 bit support for iojamp
#define QRAND_MAX 32768
float flrand(float min, float max)
{
	float	result;

	holdrand = (holdrand * 214013L) + 2531011L;
	result = (float)(holdrand >> 17);						// 0 - 32767 range
	result = ((result * (max - min)) / (float)QRAND_MAX) + min;

	return(result);
}

// Returns an integer min <= x <= max (ie inclusive)
int irand(int min, int max)
{
	int		result;

	assert((max - min) < QRAND_MAX);

	max++;
	holdrand = (holdrand * 214013L) + 2531011L;
	result = holdrand >> 17;
	result = ((result * (max - min)) >> 15) + min;
	return(result);
}

int Q_irand(int value1, int value2)
{
	return irand(value1, value2);
}

/*
AngleNormalize180
returns angle normalized to the range [-180 < angle <= 180]
*/
float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}
#pragma endregion

#pragma region String

/*
G_SanitiseString
Strip color and and special characters from string
*/
void G_SanitiseString(const char *in, char *out, int len)
{
	len--;
	if (in != NULL) {
		while (*in && len > 0) {
			if (Q_IsColorString(in)) {
				in += 2;    // skip color code
				continue;
			}

			if (iswalnum(*in)) {
				*out++ = tolower(*in);
				len--;
			}
			in++;
		}
	}
	*out = 0;
}

/*
Q_strncpyz
Safe strncpy that ensures a trailing zero
*/
void Q_strncpyz(char *dest, const char *src, int destsize) {
	// bk001129 - also NULL dest
	if (!dest) {
		g_syscall(G_ERROR, "Q_strncpyz: NULL dest");
		return;
	}
	if (!src) {
		g_syscall(G_ERROR, "Q_strncpyz: NULL src");
		return;
	}
	if (destsize < 1) {
		g_syscall(G_ERROR, "Q_strncpyz: destsize < 1");
		return;
	}

	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = 0;
}

// never goes past bounds or leaves without a terminating 0
void Q_strcat(char *dest, int size, const char *src) {
	int		l1;

	l1 = strlen(dest);
	if (l1 >= size) {
		g_syscall(G_ERROR, ERR_FATAL, "Q_strcat: already overflowed");
	}
	Q_strncpyz(dest + l1, src, size - l1);
}

/*
StringIsInteger (const char *s)
Returns qtrue if the string is an integer.
*/
qboolean StringIsInteger(const char *s) {
	int			i;
	int			len;
	qboolean	foundDigit;

	len = strlen(s);
	foundDigit = qfalse;

	for (i = 0; i < len; i++) {
		if (!isdigit(s[i])) {
			return qfalse;
		}

		foundDigit = qtrue;
	}

	return foundDigit;
}

char *ConcatArgs(int start)
{
	int		i, c, tlen;
	static char	line[MAX_SAY_TEXT];
	char	arg[MAX_SAY_TEXT] = { 0 };
	int		len;

	len = 0;
	c = g_syscall(G_ARGC);
	for (i = start; i < c; i++)
	{
		g_syscall(G_ARGV, i, arg, sizeof(arg));
		tlen = strlen(arg);
		if (len + tlen >= MAX_SAY_TEXT - 1)
		{
			break;
		}
		memcpy(line + len, arg, tlen);
		len += tlen;
		if (i != c - 1)
		{
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

char *Info_ValueForKey(const char *s, const char *key)
{
	char    pkey[BIG_INFO_KEY];
	static    char value[2][BIG_INFO_VALUE];    // use two buffers so compares
	// work without stomping on each other
	static    int    valueindex = 0;
	char    *o;

	if (!s || !key) {
		return "";
	}

	if (strlen(s) >= BIG_INFO_STRING) {
		g_syscall(G_PRINT, "Info_ValueForKey: oversize infostring");
		return "";
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1) {
		o = pkey;
		while (*s != '\\') {
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s) {
			*o++ = *s++;
		}

		*o = 0;

		if (!strcmp(key, pkey))
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}

int Q_stricmpn(const char *s1, const char *s2, int n) {
	int     c1, c2;

	if (s1 == NULL) {
		if (s2 == NULL)
			return 0;
		else
			return -1;
	}
	else if (s2 == NULL)
		return 1;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;       // strings are equal until end point
		}

		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);

	return 0;       // strings are equal
}

int Q_stricmp(const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn(s1, s2, 99999) : -1;
}

/*
============
Q_vsnprintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final string if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count, then count
characters are written and -1 is returned and no trailing '\0' is added.

Q_vsnprintf: always appends a trailing '\0', returns number of characters written (not including terminal \0)
or returns -1 on failure or if the buffer would be overflowed.
============
*/
int Q_vsnprintf(char *str, size_t size, const char *format, va_list args)
{
	int ret;

#ifdef _WIN32
	ret = _vsnprintf(str, size - 1, format, args);
#else
	ret = vsnprintf(str, size, format, args);
#endif

	str[size - 1] = '\0';

	if (ret < 0 || ret >= (signed)size)
		return -1;

	return ret;
}

void QDECL Com_Printf(const char *msg, ...) {
	va_list		argptr;
	char		text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof(text), msg, argptr);
	va_end(argptr);

	g_syscall(G_PRINT, text);
}

//Raz: Patched version of Com_sprintf
void QDECL Com_sprintf(char *dest, int size, const char *fmt, ...)
{
	int			ret;
	va_list		argptr;

	va_start(argptr, fmt);
	ret = Q_vsnprintf(dest, size, fmt, argptr);
	va_end(argptr);

	if (ret == -1)
		Com_Printf("Com_sprintf: overflow of %i bytes buffer\n", size);
}
#pragma endregion

#pragma region Game 

int G_EffectIndex( const char *name )
{
	return G_FindConfigstringIndex (name, CS_EFFECTS, MAX_FX, qtrue);
}

int G_SoundIndex( const char *name ) {
	assert(name && name[0]);
	return G_FindConfigstringIndex (name, CS_SOUNDS, MAX_SOUNDS, qtrue);
}

void G_SetAngles(gentity_t *ent, vec3_t angles)
{
	VectorCopy(angles, ent->r.currentAngles);
	VectorCopy(angles, ent->s.angles);
	VectorCopy(angles, ent->s.apos.trBase);
}

void G_SetOrigin(gentity_t *ent, vec3_t origin)
{
	VectorCopy(origin, ent->s.pos.trBase);
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorClear(ent->s.pos.trDelta);
	VectorCopy(origin, ent->r.currentOrigin);
}

void ScorePlum(int clientId, vec3_t origin, int score)
{
	gentity_t *plum = Lmdp_TempEntity(origin, EV_SCOREPLUM);
	plum->r.svFlags |= SVF_SINGLECLIENT;	// plums are only sent to activator
	plum->r.singleClient = clientId;
	plum->s.otherEntityNum = clientId;
	plum->s.time = score;
}

void PlaySound(vec3_t origin, int channel, int soundIndex)
{
	gentity_t *speaker = Lmdp_TempEntity(origin, EV_GENERAL_SOUND);
	speaker->s.eventParm = soundIndex;
	speaker->s.saberEntityNum = channel;
}

/*
BG_EvaluateTrajectory
*/
void BG_EvaluateTrajectory(const trajectory_t *tr, int atTime, vec3_t result) {
	float		deltaTime;
	float		phase;

	switch (tr->trType) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorCopy(tr->trBase, result);
		break;
	case TR_LINEAR:
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		break;
	case TR_SINE:
		deltaTime = (atTime - tr->trTime) / (float)tr->trDuration;
		phase = sin(deltaTime * M_PI * 2);
		VectorMA(tr->trBase, phase, tr->trDelta, result);
		break;
	case TR_LINEAR_STOP:
		if (atTime > tr->trTime + tr->trDuration) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		if (deltaTime < 0) {
			deltaTime = 0;
		}
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		break;
	case TR_NONLINEAR_STOP:
		if (atTime > tr->trTime + tr->trDuration)
		{
			atTime = tr->trTime + tr->trDuration;
		}
		//new slow-down at end
		if (atTime - tr->trTime > tr->trDuration || atTime - tr->trTime <= 0)
		{
			deltaTime = 0;
		}
		else
		{//FIXME: maybe scale this somehow?  So that it starts out faster and stops faster?
			deltaTime = tr->trDuration*0.001f*((float)cos(DEG2RAD(90.0f - (90.0f*((float)(atTime - tr->trTime)) / (float)tr->trDuration))));
		}
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		break;
	case TR_GRAVITY:
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	default:
		break;
	}
}

int G_RadiusList(vec3_t origin, float radius, gentity_t *ignore, qboolean takeDamage, gentity_t *ent_list[MAX_GENTITIES])
{
	float		dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	int			i, e;
	int			ent_count = 0;

	if (radius < 1)
		radius = 1;

	for (i = 0; i < 3; i++)	{
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = g_syscall(G_ENTITIES_IN_BOX, mins, maxs, entityList, MAX_GENTITIES);

	for (e = 0; e < numListedEntities; e++) {
		ent = LmdApi.entities->getEntity(entityList[e]);

		if ((ent == ignore) || !(ent->inuse) || ent->takedamage != takeDamage)
			continue;

		// find the distance from the edge of the bounding box
		for (i = 0; i < 3; i++) {
			if (origin[i] < ent->r.absmin[i])
			{
				v[i] = ent->r.absmin[i] - origin[i];
			}
			else if (origin[i] > ent->r.absmax[i])
			{
				v[i] = origin[i] - ent->r.absmax[i];
			}
			else
			{
				v[i] = 0;
			}
		}

		dist = VectorLength(v);
		if (dist >= radius)
			continue;

		// add target
		ent_list[ent_count] = ent;
		ent_count++;
	}

	return ent_count;
}

/*
G_ScaleNetHealth
Scale health down below 1024 to fit in health bits
*/
void G_ScaleNetHealth(gentity_t *self)
{
	int maxHealth = self->maxHealth;

	if (maxHealth < 1000) {
		//it's good then
		self->s.maxhealth = maxHealth;
		self->s.health = self->health;

		if (self->s.health < 0) {
			//don't let it wrap around
			self->s.health = 0;
		}
		return;
	}

	//otherwise, scale it down
	self->s.maxhealth = (maxHealth / 100);
	self->s.health = (self->health / 100);

	if (self->s.health < 0) {
		//don't let it wrap around
		self->s.health = 0;
	}

	if (self->health > 0 && self->s.health <= 0) {
		//don't let it scale to 0 if the thing is still not "dead"
		self->s.health = 1;
	}
}// Lmdp_ModelIndex
int G_ModelIndex( const char *name )
{
	fileHandle_t fh;

	g_syscall(G_FS_FOPEN_FILE, name, &fh, FS_READ);
	if (!fh) {
		g_syscall(G_SEND_SERVER_COMMAND, -1, "print \"^1Lmdp_ModelIndex: no model file\n\"");
		return 0;
	}

	g_syscall( G_FS_FCLOSE_FILE, fh);
	return G_FindConfigstringIndex (name, CS_MODELS, MAX_MODELS, qtrue);
}

// SetClientViewAngle
void SetClientViewAngle(gentity_t *ent, vec3_t angle) {
	int			i;

	// set the delta angle
	for (i = 0; i<3; i++) {
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy(angle, ent->s.angles);
	VectorCopy(ent->s.angles, ent->client->ps.viewangles);
}

#pragma endregion

#pragma region Plugin

/*
Lmdp_Trace
trace line of sight and return entity or player number
*/
int Lmdp_Trace(gentity_t *ent, int range, qboolean clientOnly)
{
	trace_t tr;
	playerState_t *ps = &ent->client->ps;
	vec3_t start, forward, end, maxs = { 8, 8, 8 }, mins = { -8, -8, -8 };
	int clientNum = NUM_FROM_ENT(ent);

	memset(&tr, NULL, sizeof(tr));

	VectorCopy(ps->origin, start);
	start[2] += ps->viewheight;
	AngleVectors(ps->viewangles, forward, NULL, NULL);
	VectorMA(start, range, forward, end);

	g_syscall(G_TRACE, &tr, start, mins, maxs, end, clientNum, MASK_SHOT);

	if (tr.fraction != 1 && tr.entityNum < (clientOnly ? g_maxclients : ENTITYNUM_WORLD))
		return tr.entityNum;
	else {
		g_syscall(G_SEND_SERVER_COMMAND, clientNum, "print \"^1Target not found\n\"");
		return -1;
	}
}

/*
Lmdp_Find (works like G_Find)
Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.
Searches beginning at the entity after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.
*/
gentity_t* Lmdp_Find(gentity_t *from, int fieldOfs, const char *match)
{
	char	*s;

	from = LmdApi.entities->iterateEntities(from);

	while (from)
	{
		if (from->inuse)
		{
			s = *(char **)((byte *)from + fieldOfs);
			if (s)
			{
				if (!Q_stricmp(s, match))
					return from;
			}
		}
		from = LmdApi.entities->iterateEntities(from);
	}

	return NULL;
}

/*
Lmdp_RemoveNewLine
fixe console commands with newline bug.
*/
void Lmdp_RemoveNewLine(char *s)
{
	if (s != NULL) {
		int length = strlen(s);

		for (int i = 0; i < length; ++i) {
			if (s[i] == '\r' || s[i] == '\n' || s[i] == ';') {
				s[i] = '\0';
				return;
			}
		}
	}
}

/*
Lmdp_ClientIdFromString (gentity_t *to, const char *s)
to: reports error to this client
s: search argument
return: player id if a match is found, else -1
*/
int Lmdp_ClientIdFromString(gentity_t *to, const char *s)
{
	int i, m = -1;
	char *err = NULL;	// error to report

	if (*s) {
		gclient_t *cl = NULL;

		if (StringIsInteger(s)) {
			// string is a number
			i = atoi(s);

			if (i >= 0 && i < g_maxclients) {
				cl = CLIENT_FROM_NUM(i);

				if (cl->pers.connected == CON_CONNECTED)
					m = i;
				else
					err = "Client is not active";
			}
			else
				err = "Client number is out of bounds";
		}
		else
		{
			// string is possibly a name or partial name
			char cleanName[MAX_NAME_LENGTH] = { 0 };
			for (i = 0, cl = g_clients; i < g_maxclients; i++, cl++) {
				if (cl->pers.connected == CON_CONNECTED)
				{
					G_SanitiseString(cl->pers.netname, cleanName,
						sizeof(cleanName));

					if (!strcmp(cleanName, s)) {
						m = i;	// an exact name match
						break;
					}
					else
					{
						if (strstr(cleanName, s)) {
							if (m == -1)
								m = i;	// a potential match
							else
							{
								m = -1;
								err = "Too many players match this name";
								break;
							}
						}
					}
				}
			}
		}
	}
	else
		err = "No player name or slot # was provided";

	if (m == -1) {
		if (err == NULL)
			err = "No matching player was found";

		g_syscall(G_SEND_SERVER_COMMAND, to == NULL ? -1 : NUM_FROM_ENT(to),
			QMM_VARARGS("print \"^1%s\n\"", err));
	}
	return m;
}

gentity_t *Lmdp_TempEntity(vec3_t origin, int event)
{
	gentity_t *ent = LmdApi.entities->createEntity();

	if (ent) {
		ent->s.eType = ET_EVENTS + event;
		ent->classname = "tempEntity";
		ent->eventTime = g_level->time;
		ent->freeAfterEvent = qtrue;

		if (origin) {
			vec3_t	snapped;
			VectorCopy(origin, snapped);
			SnapVector(snapped);
			G_SetOrigin(ent, snapped);
		}

		g_syscall(G_LINKENTITY, ent);
	}
	return ent;
}

AccountDataCategory_t *Lmdp_GetAccountData(Account_t *acc, int DataCategoryOfs)
{
	return acc->accountData.data[(acc->accountData.count - ACCOUNT_DATA_COUNT_BASE) + DataCategoryOfs];
}

int Lmdp_GetAccountLevel(Account_t *acc)
{
	return ((professionData_t*)(acc->accountData.data[(acc->accountData.count - ACCOUNT_DATA_COUNT_BASE) + ACCOUNT_DATA_PROFESSION]))->level;
}

int Lmdp_GetAccountProfession(Account_t *acc)
{
	return ((professionData_t*)(acc->accountData.data[(acc->accountData.count - ACCOUNT_DATA_COUNT_BASE) + ACCOUNT_DATA_PROFESSION]))->prof;
}

qboolean Lmdp_EditEntity(gentity_t *ent, const char *key, const char *value)
{
	SpawnData_t *spawnData = ent->Lmd.spawnData;

	if (spawnData) {
		SpawnData_t *previousData = cloneSpawnstring(spawnData);

		if (key && value)
			Lmd_Entities_setSpawnstringKey(spawnData, key, value);

		if (!spawnEntity(ent, spawnData))
			if (!spawnEntity(ent, previousData))
				LmdApi.entities->freeEntity(ent);

		removeSpawnstring(previousData);

		if (ent->inuse)
			return qtrue;
	}
	return qfalse;
}
#pragma endregion