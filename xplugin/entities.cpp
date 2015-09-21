#include "local.h"
#include "qmmapi.h"
#include "interface.h"
#include "utils.h"

#pragma region Entitities

/* Lmdp_Trigger
targetname:		a name to trigger this entity
target:			target fired for all clients
*/
void Lmdp_Trigger_Use(gentity_t *self, gentity_t *other, gentity_t *activator)
{
	gentity_t *player = NULL;
	
	// it is not necessary to check for a valid target, since
	// this entity can only exist with a target and targetname

	for (int i = 0; i < g_maxclients; ++i) {
		player = ENT_FROM_NUM(i);
		if (player->client->pers.connected == CON_CONNECTED
			&& PlayerUseableCheck(self, player))
			LmdApi.entities->useTargets(self, player, self->target);
	}
}
void Lmdp_Trigger_Spawn(gentity_t *self)
{
	if (self->target && self->targetname) {

		// set use function
		self->use = Lmdp_Trigger_Use;
	}
	else
	{
		// report failure to spawn
		LmdApi.entities->reportSpawnFailure(
			"Lmdp_Trigger_Spawn: entitiy must have target and targetname");
		
		// free the entity
		LmdApi.entities->freeEntity(self);		
	}

	// clear spawn keys
	LmdApi.entities->clearSpawnKeys();
}

// Lmdp_Entities
// list of entities
spawn_t Lmdp_Entities[] =
{
	{ "lmdp_trigger", Lmdp_Trigger_Spawn, Logical_True, NULL },
};
static const size_t Lmdp_Entities_Count = ARRAY_LEN(Lmdp_Entities);

#pragma endregion


// Lmdp_RegisterEntities
// Register plugin entities using API function
void Lmdp_RegisterEntities()
{
	for (int i = 0; i < Lmdp_Entities_Count; ++i) {
		LmdApi.entities->registerSpawnableEntity(
			Lmdp_Entities[i].name,
			Lmdp_Entities[i].spawn,
			Lmdp_Entities[i].logical,
			Lmdp_Entities[i].info);
	}
}