// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_predict.c -- this file generates cg.predictedPlayerState by either
// interpolating between snapshots from the server or locally predicting
// ahead the client's movement.
// It also handles local physics interaction, like fragments bouncing off walls

#include "cg_local.h"

static	pmove_t		cg_pmove;

static	int			cg_numSolidEntities;
static	centity_t	*cg_solidEntities[MAX_ENTITIES_IN_SNAPSHOT];
static	int			cg_numTriggerEntities;
static	centity_t	*cg_triggerEntities[MAX_ENTITIES_IN_SNAPSHOT];

/*
====================
CG_BuildSolidList

When a new cg.snap has been set, this function builds a sublist
of the entities that are actually solid, to make for more
efficient collision detection
====================
*/
void CG_BuildSolidList( void ) {
	int			i;
	centity_t	*cent;
	snapshot_t	*snap;
	entityState_t	*ent;

	cg_numSolidEntities = 0;
	cg_numTriggerEntities = 0;

	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		snap = cg.nextSnap;
	} else {
		snap = cg.snap;
	}

	for ( i = 0 ; i < snap->numEntities ; i++ ) {
		cent = &cg_entities[ snap->entities[ i ].number ];
		ent = &cent->currentState;

		if ( ent->eType == ET_ITEM || ent->eType == ET_PUSH_TRIGGER || ent->eType == ET_TELEPORT_TRIGGER ) {
			cg_triggerEntities[cg_numTriggerEntities] = cent;
			cg_numTriggerEntities++;
			continue;
		}

		if ( cent->nextState.solid ) {
			cg_solidEntities[cg_numSolidEntities] = cent;
			cg_numSolidEntities++;
			continue;
		}
	}
}


/*
====================
CG_ClipMoveToEntities

====================
*/
static void CG_ClipMoveToEntities ( const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end,
							int skipNumber, int mask, trace_t *tr ) {
	int			i, x, zd, zu;
	trace_t		trace;
	entityState_t	*ent;
	clipHandle_t 	cmodel;
	vec3_t		bmins, bmaxs;
	vec3_t		origin, angles;
	centity_t	*cent;

	for ( i = 0 ; i < cg_numSolidEntities ; i++ ) {
		cent = cg_solidEntities[ i ];
		ent = &cent->currentState;

		if ( ent->number == skipNumber ) {
			continue;
		}

		if ( ent->solid == SOLID_BMODEL ) {
			// special value for bmodel
			cmodel = trap_CM_InlineModel( ent->modelindex );
			VectorCopy( cent->lerpAngles, angles );
			BG_EvaluateTrajectory( &cent->currentState.pos, cg.physicsTime, origin );
		} else {
			// encoded bbox
			x = (ent->solid & 255);
			zd = ((ent->solid>>8) & 255);
			zu = ((ent->solid>>16) & 255) - 32;

			bmins[0] = bmins[1] = -x;
			bmaxs[0] = bmaxs[1] = x;
			bmins[2] = -zd;
			bmaxs[2] = zu;

			cmodel = trap_CM_TempBoxModel( bmins, bmaxs );
			VectorCopy( vec3_origin, angles );
			VectorCopy( cent->lerpOrigin, origin );
		}


		trap_CM_TransformedBoxTrace ( &trace, start, end,
			mins, maxs, cmodel,  mask, origin, angles);

		if (trace.allsolid || trace.fraction < tr->fraction) {
			trace.entityNum = ent->number;
			*tr = trace;
		} else if (trace.startsolid) {
			tr->startsolid = qtrue;
		}
		if ( tr->allsolid ) {
			return;
		}
	}
}


/*
================
CG_Trace
================
*/
void	CG_Trace( trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, 
					 int skipNumber, int mask ) {
	trace_t	t;

	trap_CM_BoxTrace ( &t, start, end, mins, maxs, 0, mask);
	if ( t.fraction == 1.0 )
		t.entityNum = ENTITYNUM_NONE;
	else
		t.entityNum = ENTITYNUM_WORLD;

	// check all other solid models
	CG_ClipMoveToEntities (start, mins, maxs, end, skipNumber, mask, &t);

	*result = t;
}


/*
================
CG_PointContents
================
*/
int		CG_PointContents( const vec3_t point, int passEntityNum ) {
	int			i;
	entityState_t	*ent;
	centity_t	*cent;
	clipHandle_t cmodel;
	int			contents;

	contents = trap_CM_PointContents (point, 0);

	for ( i = 0 ; i < cg_numSolidEntities ; i++ ) {
		cent = cg_solidEntities[ i ];

		ent = &cent->currentState;

		if ( ent->number == passEntityNum ) {
			continue;
		}

		if (ent->solid != SOLID_BMODEL) { // special value for bmodel
			continue;
		}

		cmodel = trap_CM_InlineModel( ent->modelindex );
		if ( !cmodel ) {
			continue;
		}

		contents |= trap_CM_TransformedPointContents( point, cmodel, cent->lerpOrigin, cent->lerpAngles );
	}

	return contents;
}


/*
========================
CG_InterpolatePlayerState

Generates cg.predictedPlayerState by interpolating between
cg.snap->player_state and cg.nextFrame->player_state
========================
*/
static void CG_InterpolatePlayerState( qboolean grabAngles ) {
	float			f;
	int				i;
	playerState_t	*out;
	snapshot_t		*prev, *next;

	out = &cg.predictedPlayerState;
	prev = cg.snap;
	next = cg.nextSnap;

	*out = cg.snap->ps;

	// if we are still allowing local input, short circuit the view angles
	if ( grabAngles ) {
		usercmd_t	cmd;
		int			cmdNum;

		cmdNum = trap_GetCurrentCmdNumber();
		trap_GetUserCmd( cmdNum, &cmd );

		PM_UpdateViewAngles( out, &cmd );
	}

	// if the next frame is a teleport, we can't lerp to it
	if ( cg.nextFrameTeleport ) {
		return;
	}

	if ( !next || next->serverTime <= prev->serverTime ) {
		return;
	}

	f = (float)( cg.time - prev->serverTime ) / ( next->serverTime - prev->serverTime );

	i = next->ps.bobCycle;
	if ( i < prev->ps.bobCycle ) {
		i += 256;		// handle wraparound
	}
	out->bobCycle = prev->ps.bobCycle + f * ( i - prev->ps.bobCycle );

	for ( i = 0 ; i < 3 ; i++ ) {
		out->origin[i] = prev->ps.origin[i] + f * (next->ps.origin[i] - prev->ps.origin[i] );
		if ( !grabAngles ) {
			out->viewangles[i] = LerpAngle( 
				prev->ps.viewangles[i], next->ps.viewangles[i], f );
		}
		out->velocity[i] = prev->ps.velocity[i] + 
			f * (next->ps.velocity[i] - prev->ps.velocity[i] );
	}

}

int				eventStack;
entity_event_t	events[ MAX_PREDICTED_EVENTS ];
int				eventParms[ MAX_PREDICTED_EVENTS ];
int				eventParm2[ MAX_PREDICTED_EVENTS ]; // client entity index

void CG_AddFallDamage( int damage );

/*
===================
CG_StoreEvents

Save events that may be dropped during prediction
===================
*/
void CG_StoreEvent( entity_event_t evt, int eventParm, int entityNum ) 
{
	if ( eventStack >= MAX_PREDICTED_EVENTS )
		return;

	if ( evt == EV_FALL_FAR ) {
		CG_AddFallDamage( 10 );
	} else if ( evt == EV_FALL_MEDIUM ) {
		CG_AddFallDamage( 5 );
	}

	events[ eventStack ] = evt;
	eventParms[ eventStack ] = eventParm;
	eventParm2[ eventStack ] = entityNum;
	eventStack++;
}


/*
===================
CG_PlayDroppedEvents
===================
*/
void CG_PlayDroppedEvents( playerState_t *ps, playerState_t *ops ) {
	centity_t	*cent;
	entity_event_t oldEvent;
	int i, oldParam;

	if ( ps == ops ) {
		return;
	}

	if ( eventStack <= MAX_PS_EVENTS ) {
		return;
	}

	cent = &cg.predictedPlayerEntity;

	oldEvent = cent->currentState.event;
	oldParam = cent->currentState.eventParm;

	for ( i = 0; i < eventStack - MAX_PS_EVENTS ; i++ ) {
		cent->currentState.event = events[ i ];
		cent->currentState.eventParm = eventParms[ i ];
		if ( cg_showmiss.integer ) 
		{
			CG_Printf( "Playing dropped event: %s %i", eventnames[ events[ i ] ], eventParms[ i ] );
		}
		CG_EntityEvent( cent, cent->lerpOrigin, eventParm2[ i ] );
		cg.eventSequence++;
	}

	cent->currentState.event = oldEvent;
	cent->currentState.eventParm = oldParam;
}


static void CG_AddArmor( const gitem_t *item, int quantity ) {

	cg.predictedPlayerState.stats[STAT_ARMOR] += quantity;

		if ( cg.predictedPlayerState.stats[STAT_ARMOR] > cg.predictedPlayerState.stats[STAT_MAX_HEALTH]*2 )
			cg.predictedPlayerState.stats[STAT_ARMOR] = cg.predictedPlayerState.stats[STAT_MAX_HEALTH]*2;
}


static void CG_AddAmmo( int weapon, int count )
{
	if ( weapon == WP_GAUNTLET || weapon == WP_GRAPPLING_HOOK ) {
		cg.predictedPlayerState.ammo[weapon] = -1;
	} else {
		cg.predictedPlayerState.ammo[weapon] += count;
		if ( weapon >= WP_MACHINEGUN && weapon <= WP_BFG ) {
			if ( cg.predictedPlayerState.ammo[weapon] > AMMO_HARD_LIMIT ) {
				cg.predictedPlayerState.ammo[weapon] = AMMO_HARD_LIMIT;
			}
		}
	}
}


static void CG_AddWeapon( int weapon, int quantity, qboolean dropped ) 
{
	//int	ammo;
	
	//ammo = quantity;

	// dropped items and teamplay weapons always have full ammo
	if ( !dropped && cgs.gametype != GT_TEAM ) {
		if ( cg.predictedPlayerState.ammo[ weapon ] < quantity ) {
			quantity = quantity - cg.predictedPlayerState.ammo[ weapon ];
		} else {
			quantity = 1;
		}
	}

	// add the weapon
	cg.predictedPlayerState.stats[STAT_WEAPONS] |= ( 1 << weapon );

	CG_AddAmmo( weapon, quantity );
}


static int CG_CheckArmor( int damage ) {
	int				save;
	int				count;

	count = cg.predictedPlayerState.stats[STAT_ARMOR];

	save = ceil( damage * ARMOR_PROTECTION );

	if (save >= count)
		save = count;

	if ( !save )
		return 0;
	
	cg.predictedPlayerState.stats[STAT_ARMOR] -= save;

	return save;
}


 void CG_AddFallDamage( int damage ) 
{
	int take, asave;

	if ( cg.predictedPlayerState.powerups[ PW_BATTLESUIT ] )
		return;

	if ( cg.predictedPlayerState.clientNum != cg.snap->ps.clientNum || cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	take = damage;

	asave = CG_CheckArmor( take );

	take -= asave;

	cg.predictedPlayerState.stats[STAT_HEALTH] -= take;

#if 0
	CG_Printf( "take: %i asave:%i health:%i armor:%i\n", take, asave, 
		cg.predictedPlayerState.stats[STAT_HEALTH], cg.predictedPlayerState.stats[STAT_ARMOR] );
#endif

	cg.predictedPlayerState.damagePitch = 255;
	cg.predictedPlayerState.damageYaw = 255;
	//cg.predictedPlayerState.damageEvent++;
	cg.predictedPlayerState.damageCount = take + asave;
}
 

static void CG_PickupPrediction( centity_t *cent, const gitem_t *item ) {

	// health prediction
	if ( item->giType == IT_HEALTH && cent->currentState.time2 > 0 ) {
		int limit;

		limit = cg.predictedPlayerState.stats[ STAT_MAX_HEALTH ]; // soft limit
		if ( !Q_stricmp( item->classname, "item_health_small" ) || !Q_stricmp( item->classname, "item_health_mega" ) ) {
			limit *= 2; // hard limit
		}

		cg.predictedPlayerState.stats[STAT_HEALTH] += cent->currentState.time2;
		if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] > limit ) {
			cg.predictedPlayerState.stats[ STAT_HEALTH ] = limit;
		}
	}

	// armor prediction
	if ( item->giType == IT_ARMOR && cent->currentState.time2 > 0 ) {
		CG_AddArmor( item, cent->currentState.time2 );
		return;
	}

	// ammo prediction
	if ( item->giType == IT_AMMO && cent->currentState.time2 > 0 ) {
		CG_AddAmmo( item->giTag, cent->currentState.time2 );
		return;
	}

	// weapon prediction
	if ( item->giType == IT_WEAPON && cent->currentState.time2 > 0 ) {
		CG_AddWeapon( item->giTag, cent->currentState.time2, (cent->currentState.modelindex2 == 1) );
		return;
	}

	// powerups prediction
	if ( item->giType == IT_POWERUP && item->giTag >= PW_QUAD && item->giTag <= PW_FLIGHT ) {
		// round timing to seconds to make multiple powerup timers count in sync
		if ( !cg.predictedPlayerState.powerups[ item->giTag ] ) {
			cg.predictedPlayerState.powerups[ item->giTag ] = cg.predictedPlayerState.commandTime - ( cg.predictedPlayerState.commandTime % 1000 );
			// this assumption is correct only on transition and implies hardcoded 1.3 coefficient:
			if ( item->giTag == PW_HASTE ) {
				cg.predictedPlayerState.speed *= 1.3f;
			}
		}
		cg.predictedPlayerState.powerups[ item->giTag ] += cent->currentState.time2 * 1000;
	}	

	// holdable prediction
	if ( item->giType == IT_HOLDABLE && ( item->giTag == HI_TELEPORTER || item->giTag == HI_MEDKIT ) ) {
		cg.predictedPlayerState.stats[ STAT_HOLDABLE_ITEM ] = item - bg_itemlist;
	}
}


/*
===================
CG_TouchItem
===================
*/
static void CG_TouchItem( centity_t *cent ) {
	const gitem_t *item;

	if ( cg.allowPickupPrediction && cg.allowPickupPrediction > cg.time ) {
		return;
	}

	if ( !cg_predictItems.integer ) {
		return;
	}

	if ( !BG_PlayerTouchesItem( &cg.predictedPlayerState, &cent->currentState, cg.time ) ) {
		return;
	}

	// never pick an item up twice in a prediction
	if ( cent->delaySpawn > cg.time ) { 
		return;
	}

	if ( !BG_CanItemBeGrabbed( cgs.gametype, &cent->currentState, &cg.predictedPlayerState ) ) {
		return;	// can't hold it
	}

	item = &bg_itemlist[ cent->currentState.modelindex ];

	// Special case for flags.  
	// We don't predict touching our own flag
#ifdef MISSIONPACK
	if( cgs.gametype == GT_1FCTF ) {
		if( item->giTag != PW_NEUTRALFLAG ) {
			return;
		}
	}
	if( cgs.gametype == GT_CTF || cgs.gametype == GT_HARVESTER ) {
#else
	if( cgs.gametype == GT_CTF ) {
#endif
		if (cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_RED &&
			item->giType == IT_TEAM && item->giTag == PW_REDFLAG)
			return;
		if (cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_BLUE &&
			item->giType == IT_TEAM && item->giTag == PW_BLUEFLAG)
			return;
	}

	// grab it
	BG_AddPredictableEventToPlayerstate( EV_ITEM_PICKUP, cent->currentState.modelindex , &cg.predictedPlayerState, cent - cg_entities );

	// perform prediction
	CG_PickupPrediction( cent, item );

	// remove it from the frame so it won't be drawn
	cent->currentState.eFlags |= EF_NODRAW;

	// don't touch it again this prediction
	cent->miscTime = cg.time;

	// delay next potential pickup for some time
	cent->delaySpawn = cg.time + ( cg.meanPing > 0 ? cg.meanPing * 2 + 100 : 333 );
	cent->delaySpawnPlayed = qfalse;

	// if it's a weapon, give them some predicted ammo so the autoswitch will work
	if ( item->giType == IT_WEAPON ) {
		cg.predictedPlayerState.stats[ STAT_WEAPONS ] |= 1 << item->giTag;
		if ( !cg.predictedPlayerState.ammo[ item->giTag ] ) {
			cg.predictedPlayerState.ammo[ item->giTag ] = 1;
		}
	}
}


/*
=========================
CG_TouchTriggerPrediction

Predict push triggers and items
=========================
*/
static void CG_TouchTriggerPrediction( void ) {
	int			i;
	trace_t		trace;
	entityState_t	*ent;
	clipHandle_t cmodel;
	centity_t	*cent;
	qboolean	spectator;

	// dead clients don't activate triggers
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	spectator = ( cg.predictedPlayerState.pm_type == PM_SPECTATOR );

	if ( cg.predictedPlayerState.pm_type != PM_NORMAL && !spectator ) {
		return;
	}

	for ( i = 0 ; i < cg_numTriggerEntities ; i++ ) {
		cent = cg_triggerEntities[ i ];
		ent = &cent->currentState;

		if ( ent->eType == ET_ITEM && !spectator ) {
			CG_TouchItem( cent );
			continue;
		}

		if ( ent->solid != SOLID_BMODEL ) {
			continue;
		}

		cmodel = trap_CM_InlineModel( ent->modelindex );
		if ( !cmodel ) {
			continue;
		}

		trap_CM_BoxTrace( &trace, cg.predictedPlayerState.origin, cg.predictedPlayerState.origin, 
			cg_pmove.mins, cg_pmove.maxs, cmodel, -1 );

		if ( !trace.startsolid ) {
			continue;
		}

		if ( ent->eType == ET_TELEPORT_TRIGGER ) {
			cg.hyperspace = qtrue;
		} else if ( ent->eType == ET_PUSH_TRIGGER ) {
			BG_TouchJumpPad( &cg.predictedPlayerState, ent );
		}
	}

	// if we didn't touch a jump pad this pmove frame
	if ( cg.predictedPlayerState.jumppad_frame != cg.predictedPlayerState.pmove_framecount ) {
		cg.predictedPlayerState.jumppad_frame = 0;
		cg.predictedPlayerState.jumppad_ent = 0;
	}
}


static void CG_CheckTimers( void ) {
	int i;

	// no prediction for spectators
	if ( cg.predictedPlayerState.pm_type == PM_SPECTATOR ) {
		return;
	}

	// no armor/health/powerups prediction for dead bodies
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )
		return;

	// periodic tasks
	if ( cg.timeResidual && cg.predictedPlayerState.commandTime >= cg.timeResidual && !cg.thisFrameTeleport ) {
		cg.timeResidual += 1000;
		if ( cg.predictedPlayerState.powerups[ PW_REGEN ] ) {
			int maxhealth = cg.predictedPlayerState.stats[ STAT_MAX_HEALTH ];
			if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] < maxhealth ) {
				cg.predictedPlayerState.stats[ STAT_HEALTH ] += 15;
				if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] > maxhealth * 1.1 ) {
					cg.predictedPlayerState.stats[ STAT_HEALTH ] = maxhealth * 1.1;
				}
				// TODO: add external EV_POWERUP_REGEN
			} else if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] < maxhealth * 2) {
				cg.predictedPlayerState.stats[ STAT_HEALTH ] += 5;
				if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] > maxhealth * 2 ) {
					cg.predictedPlayerState.stats[ STAT_HEALTH ] = maxhealth * 2;
				}
				// TODO: add external EV_POWERUP_REGEN
			}
		} else {
			if ( cg.predictedPlayerState.stats[ STAT_HEALTH ] > cg.predictedPlayerState.stats[ STAT_MAX_HEALTH ] ) {
				cg.predictedPlayerState.stats[ STAT_HEALTH ]--;
			}
		}
		if ( cg.predictedPlayerState.stats[ STAT_ARMOR ] > cg.predictedPlayerState.stats[ STAT_MAX_HEALTH ] ) {
			cg.predictedPlayerState.stats[ STAT_ARMOR ]--;
		}
	}

	// turn off any expired powerups
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !cg.predictedPlayerState.powerups[ i ] )
			continue;
		if ( cg.predictedPlayerState.powerups[ i ] < cg.predictedPlayerState.commandTime ) {
			cg.predictedPlayerState.powerups[ i ] = 0;
		}
	}
}


static int CG_IsUnacceptableError( playerState_t *ps, playerState_t *pps, qboolean *forceMove ) {
	vec3_t delta;
	int i, n, v0, v1;
 
	if ( pps->pm_time != ps->pm_time ||
		 pps->pm_type != ps->pm_type ||
		 pps->pm_flags != ps->pm_flags ) {
		return 1;
	}
 
	VectorSubtract( pps->origin, ps->origin, delta );
	if ( VectorLengthSquared( delta ) > 0.01f * 0.01f ) {
		if( cg_showmiss.integer > 2 ) {
			CG_Printf( "origin delta: %.2f  ", VectorLength( delta ) );
		}
		return 2;
	}

	VectorSubtract( pps->velocity, ps->velocity, delta );
	if( VectorLengthSquared( delta ) > 0.01f * 0.01f ) {
		if( cg_showmiss.integer > 2 ) {
			CG_Printf( "velocity delta: %.2f  ", VectorLength( delta ) );
		}
		return 3;
	}

	if( pps->weaponTime != ps->weaponTime ||
		pps->gravity != ps->gravity ||
		pps->speed != ps->speed ||
		pps->delta_angles[ 0 ] != ps->delta_angles[ 0 ] ||
		pps->delta_angles[ 1 ] != ps->delta_angles[ 1 ] ||
		pps->delta_angles[ 2 ] != ps->delta_angles[ 2 ] || 
		pps->groundEntityNum != ps->groundEntityNum ) {
		if ( cg_showmiss.integer > 1 )
			CG_Printf( "%i %i %i %i => %i %i %i %i", 
				pps->weaponTime, pps->gravity, pps->speed, pps->groundEntityNum,
				ps->weaponTime, ps->gravity, ps->speed, ps->groundEntityNum ); 

		return 4;
	}

	// forward gesture animation
	if ( pps->torsoAnim != ps->torsoAnim && (ps->torsoAnim & ~ANIM_TOGGLEBIT ) == TORSO_GESTURE ) {
		for ( n = 0 ; n < NUM_SAVED_STATES; n++ ) {
			cg.savedPmoveStates[ n ].torsoAnim = ps->torsoAnim;
			cg.savedPmoveStates[ n ].torsoTimer = ps->torsoTimer;
		}
	}

	if ( pps->legsTimer != ps->legsTimer ||	pps->legsAnim != ps->legsAnim ||
		pps->torsoTimer != ps->torsoTimer || pps->torsoAnim != ps->torsoAnim ||
		pps->movementDir != ps->movementDir ) {
		return 5;
	}

	VectorSubtract( pps->grapplePoint, ps->grapplePoint, delta );
	if( VectorLengthSquared( delta ) > 0.01f * 0.01f )
		return 6;

	// check/update eFlags if needed
	v0 = pps->eFlags & EF_NOPREDICT;
	v1 = ps->eFlags & EF_NOPREDICT;
	if ( v0 != v1 ) {
		for ( i = 0 ; i < NUM_SAVED_STATES; i++ ) {
			cg.savedPmoveStates[ i ].eFlags = (cg.savedPmoveStates[ i ].eFlags & ~EF_NOPREDICT) | v1 ;
		}
		pps->eFlags = (pps->eFlags & ~EF_NOPREDICT) | v1;
	}

	if ( pps->eFlags !=  ps->eFlags ) {
		if ( cg_showmiss.integer > 1 )
			CG_Printf( "eFlags %i => %i", pps->eFlags, ps->eFlags );
		return 7;
	}

	if( pps->eventSequence != ps->eventSequence )
		return 8;

	for( i = 0; i < MAX_PS_EVENTS; i++ ) {
		if ( pps->events[ i ] != ps->events[ i ] ) {
			if ( cg_showmiss.integer > 1 ) {
				CG_Printf( "event[%i] %i => %i\n", i, pps->events[ i ], ps->events[ i ] );
			}
			return 9;
		}
		if ( pps->eventParms[ i ] != ps->eventParms[ i ] ) {
			if ( cg_showmiss.integer > 1 ) {
				CG_Printf( "eventParms[%i] %i => %i\n", i, pps->eventParms[ i ], ps->eventParms[ i ] );
			}
			return 9;
		}
	}

	if ( pps->externalEvent != ps->externalEvent ||
		pps->externalEventParm != ps->externalEventParm ||
		pps->externalEventTime != ps->externalEventTime ) {
		return 10;
	}

	if ( pps->clientNum != ps->clientNum ||
		pps->weapon != ps->weapon ||
		pps->weaponstate != ps->weaponstate ) {
		return 11;
	}

	if ( fabs( AngleDelta( ps->viewangles[ 0 ], pps->viewangles[ 0 ] ) ) > 1.0f ||
		fabs( AngleDelta( ps->viewangles[ 1 ], pps->viewangles[ 1 ] ) ) > 1.0f ||
		fabs( AngleDelta( ps->viewangles[ 2 ], pps->viewangles[ 2 ] ) ) > 1.0f ) {
		return 12;
	}

	if ( pps->viewheight != ps->viewheight )
  		return 13;

	if( pps->damageEvent != ps->damageEvent ||
		pps->damageYaw != ps->damageYaw ||
		pps->damagePitch != ps->damagePitch ||
		pps->damageCount != ps->damageCount ) {
		if ( cg_showmiss.integer > 1 )
			CG_Printf( "dmg %i %i %i %i >= %i %i %i %i\n", 
				pps->damageEvent, pps->damageYaw, pps->damagePitch, pps->damageCount,
				ps->damageEvent, ps->damageYaw, ps->damagePitch, ps->damageCount );
		return 14;
	}

	// health countdown?
	if ( pps->stats[ STAT_HEALTH ] == ps->stats[ STAT_HEALTH ] + 1 && ps->stats[ STAT_HEALTH ] >= ps->stats[ STAT_MAX_HEALTH ] ) {
		cg.timeResidual = ps->commandTime + 1000;
		for ( n = 0 ; n < NUM_SAVED_STATES; n++ ) {
			cg.savedPmoveStates[ n ].stats[ STAT_HEALTH ] = ps->stats[ STAT_HEALTH ];
		}

	}
	// armor countdown?
	if ( pps->stats[ STAT_ARMOR ] == ps->stats[ STAT_ARMOR ] - 1 && ps->stats[ STAT_ARMOR ] >= ps->stats[ STAT_MAX_HEALTH ] ) {
		// we may need few frames to sync with client->timeResidual on server side
		cg.timeResidual = ps->commandTime + 1000;
		for ( n = 0 ; n < NUM_SAVED_STATES; n++ ) {
			cg.savedPmoveStates[ n ].stats[ STAT_ARMOR ] = ps->stats[ STAT_ARMOR ];
		}
	}

	for( i = 0; i < MAX_STATS; i++ ) {
		// we can't predict some flags
		if ( i == STAT_CLIENTS_READY /*|| i == STAT_MAX_HEALTH */ ) {
			for ( n = 0 ; n < NUM_SAVED_STATES; n++ ) {
				cg.savedPmoveStates[ n ].stats[ i ] = ps->stats[ i ];
			}
			continue;
		}
		if ( pps->stats[ i ] != ps->stats[ i ] ) {
			if ( cg_showmiss.integer > 1 ) {
				CG_Printf( "stats[%i] %i => %i ", i, pps->stats[ i ], ps->stats[ i ] );
			}
			return 15;
		}
	}


	for( i = 0; i < MAX_PERSISTANT ; i++ )
	{
		if ( pps->persistant[ i ] != ps->persistant[ i ] ) {
			if ( i >= PERS_TEAM && i <= PERS_PLAYEREVENTS ) {
				if ( cg_showmiss.integer > 1 ) {
					CG_Printf( "persistant[%i] %i => %i ", i, pps->persistant[ i ], ps->persistant[ i ] );
				}
				return 16;
			}
			v0 = ps->persistant[ i ];
			for ( n = 0 ; n < NUM_SAVED_STATES; n++ ) {
				cg.savedPmoveStates[ n ].persistant[ i ] = v0;
			}
			*forceMove = qtrue;
		}
	}

	for( i = 0; i < MAX_WEAPONS; i++ ) {
		if( pps->ammo[ i ] != ps->ammo[ i ] ) {
			if ( cg_showmiss.integer > 1 ) {
				CG_Printf( "ammo[%i] %i => %i ", i, pps->ammo[ i ], ps->ammo[ i ] );
			}
			return 18;
		}
	}

	if ( pps->generic1 != ps->generic1 || pps->loopSound != ps->loopSound ) {
		return 19;
	}

	for ( i = 0; i < MAX_POWERUPS; i++ ) {
		if( pps->powerups[ i ] != ps->powerups[ i ] ) {
			if ( cg_showmiss.integer > 1 )
				CG_Printf( "powerups[%i] %i => %i ", i, pps->powerups[i], ps->powerups[i] );
			return 20;
		}
	}

	return 0;
}


/*
=================
CG_PredictPlayerState

Generates cg.predictedPlayerState for the current cg.time
cg.predictedPlayerState is guaranteed to be valid after exiting.

For demo playback, this will be an interpolation between two valid
playerState_t.

For normal gameplay, it will be the result of predicted usercmd_t on
top of the most recent playerState_t received from the server.

Each new snapshot will usually have one or more new usercmd over the last,
but we simulate all unacknowledged commands each time, not just the new ones.
This means that on an internet connection, quite a few pmoves may be issued
each frame.

OPTIMIZE: don't re-simulate unless the newly arrived snapshot playerState_t
differs from the predicted one.  Would require saving all intermediate
playerState_t during prediction.

We detect prediction errors and allow them to be decayed off over several frames
to ease the jerk.
=================
*/
void CG_PredictPlayerState( void ) {
	int			cmdNum, current;
	playerState_t	oldPlayerState;
	qboolean	moved;
	usercmd_t	oldestCmd;
	usercmd_t	latestCmd;
	int stateIndex = 0, predictCmd = 0;

	cg.hyperspace = qfalse;	// will be set if touching a trigger_teleport

	// if this is the first frame we must guarantee
	// predictedPlayerState is valid even if there is some
	// other error condition
	if ( !cg.validPPS ) {
		cg.validPPS = qtrue;
		cg.predictedPlayerState = cg.snap->ps;
	}


	// demo playback just copies the moves
	if ( cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
		CG_InterpolatePlayerState( qfalse );
		return;
	}

	// non-predicting local movement will grab the latest angles
	if ( cg_nopredict.integer || cgs.synchronousClients ) {
		CG_InterpolatePlayerState( qtrue );
		return;
	}

	// prepare for pmove
	cg_pmove.ps = &cg.predictedPlayerState;
	cg_pmove.trace = CG_Trace;
	cg_pmove.pointcontents = CG_PointContents;
	if ( cg_pmove.ps->pm_type == PM_DEAD ) {
		cg_pmove.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
	}
	else {
		cg_pmove.tracemask = MASK_PLAYERSOLID;
	}
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		cg_pmove.tracemask &= ~CONTENTS_BODY;	// spectators can fly through bodies
	}

	// save the state before the pmove so we can detect transitions
	oldPlayerState = cg.predictedPlayerState;

	current = trap_GetCurrentCmdNumber();

	// if we don't have the commands right after the snapshot, we
	// can't accurately predict a current position, so just freeze at
	// the last good position we had
	cmdNum = current - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &oldestCmd );
	if ( oldestCmd.serverTime > cg.snap->ps.commandTime
		&& oldestCmd.serverTime < cg.time ) {	// special check for map_restart
		if ( cg_showmiss.integer ) {
			CG_Printf ("exceeded PACKET_BACKUP on commands\n");
		}
		return;
	}

	// get the latest command so we can know which commands are from previous map_restarts
	trap_GetUserCmd( current, &latestCmd );

	// get the most recent information we have, even if
	// the server time is beyond our current cg.time,
	// because predicted player positions are going to 
	// be ahead of everything else anyway
	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		cg.predictedPlayerState = cg.nextSnap->ps;
		cg.physicsTime = cg.nextSnap->serverTime;
	} else {
		cg.predictedPlayerState = cg.snap->ps;
		cg.physicsTime = cg.snap->serverTime;
	}

	cg_pmove.pmove_fixed = cgs.pmove_fixed;
	cg_pmove.pmove_msec = cgs.pmove_msec;

	// clean event stack
	eventStack = 0;
	
	// run cmds
	moved = qfalse;

	cg_pmove.pmove_fixed = cgs.pmove_fixed;
	cg_pmove.pmove_msec = cgs.pmove_msec;

	// Like the comments described above, a player's state is entirely
	// re-predicted from the last valid snapshot every client frame, which
	// can be really, really, really slow.  Every old command has to be
	// run again.  For every client frame that is *not* directly after a
	// snapshot, this is unnecessary, since we have no new information.
	// For those, we'll play back the predictions from the last frame and
	// predict only the newest commands.  Essentially, we'll be doing
	// an incremental predict instead of a full predict.
	//
	// If we have a new snapshot, we can compare its player state's command
	// time to the command times in the queue to find a match.  If we find
	// a matching state, and the predicted version has not deviated, we can
	// use the predicted state as a base - and also do an incremental predict.
	//
	// With this method, we get incremental predicts on every client frame
	// except a frame following a new snapshot in which there was a prediction
	// error.  This yeilds anywhere from a 15% to 40% performance increase,
	// depending on how much of a bottleneck the CPU is.
	if( 1 /* cg_optimizePrediction.integer */ ) {
		if( cg.nextFrameTeleport || cg.thisFrameTeleport ) {
			// do a full predict
			cg.lastPredictedCommand = 0;
			cg.stateTail = cg.stateHead;
			predictCmd = current - CMD_BACKUP + 1;
		}
		// cg.physicsTime is the current snapshot's serverTime if it's the same
		// as the last one
		else if( cg.physicsTime == cg.lastServerTime ) {
			// we have no new information, so do an incremental predict
			predictCmd = cg.lastPredictedCommand + 1;
		} else {
			// we have a new snapshot
			int i;
			int errorcode;
			qboolean error = qtrue;

			// loop through the saved states queue
			for( i = cg.stateHead; i != cg.stateTail; i = ( i + 1 ) % NUM_SAVED_STATES ) {
				// if we find a predicted state whose commandTime matches the snapshot
				// player state's commandTime
				if( cg.savedPmoveStates[ i ].commandTime != cg.predictedPlayerState.commandTime ) {
					continue;
				}
				// make sure the state differences are acceptable
				errorcode = CG_IsUnacceptableError( &cg.predictedPlayerState, &cg.savedPmoveStates[ i ], &moved );
				if ( errorcode ) {
					if( cg_showmiss.integer > 1 )
						CG_Printf( "errorcode %d at %d\n", errorcode, cg.time );
					break;
				}
  
				// this one is almost exact, so we'll copy it in as the starting point
				*cg_pmove.ps = cg.savedPmoveStates[ i ];
				// advance the head
				cg.stateHead = ( i + 1 ) % NUM_SAVED_STATES;
  
				// set the next command to predict
				predictCmd = cg.lastPredictedCommand + 1;
  
				// a saved state matched, so flag it
				error = qfalse;
				break;
			}

			// if no saved states matched
			if ( error ) {
				// do a full predict
				cg.lastPredictedCommand = 0;
				cg.stateTail = cg.stateHead;
				predictCmd = current - CMD_BACKUP + 1;
			}
		}
		// keep track of the server time of the last snapshot so we
		// know when we're starting from a new one in future calls
		cg.lastServerTime = cg.physicsTime;
		stateIndex = cg.stateHead;
	}

	cmdNum = current - CMD_BACKUP + 1;
	if ( cmdNum < 0 ) // can happen on first spawn
		cmdNum = 0;

	// run cmds
	// moved = qfalse;

	for ( /* cmdNum = current - CMD_BACKUP + 1 */; cmdNum <= current ; cmdNum++ ) {
		// get the command
		trap_GetUserCmd( cmdNum, &cg_pmove.cmd );

		if ( cgs.pmove_fixed ) {
			PM_UpdateViewAngles( cg_pmove.ps, &cg_pmove.cmd );
		}

		// don't do anything if the time is before the snapshot player time
		if ( cg_pmove.cmd.serverTime <= cg.predictedPlayerState.commandTime ) {
			continue;
		}

		// don't do anything if the command was from a previous map_restart
		if ( cg_pmove.cmd.serverTime > latestCmd.serverTime ) {
			continue;
		}

		// check for a prediction error from last frame
		// on a lan, this will often be the exact value
		// from the snapshot, but on a wan we will have
		// to predict several commands to get to the point
		// we want to compare
		if ( cg.predictedPlayerState.commandTime == oldPlayerState.commandTime ) {
			vec3_t	delta;
			float	len;

			if ( cg.thisFrameTeleport ) {
				// a teleport will not cause an error decay
				VectorClear( cg.predictedError );
				if ( cg_showmiss.integer ) {
					CG_Printf( "PredictionTeleport\n" );
				}
				cg.thisFrameTeleport = qfalse;

				// delay prediction for some time or until first server event
				cg.allowPickupPrediction = cg.time + PICKUP_PREDICTION_DELAY;
			} else {
				vec3_t adjusted, new_angles;
				CG_AdjustPositionForMover( cg.predictedPlayerState.origin, 
					cg.predictedPlayerState.groundEntityNum, cg.physicsTime, cg.oldTime, adjusted, cg.predictedPlayerState.viewangles, new_angles);

				if ( cg_showmiss.integer ) {
					if ( !VectorCompare( oldPlayerState.origin, adjusted ) ) {
						CG_Printf( "prediction error\n" );
					}
				}
				VectorSubtract( oldPlayerState.origin, adjusted, delta );
				len = VectorLengthSquared( delta );
				if ( len > (0.01f * 0.01f) ) {
					if ( cg_showmiss.integer ) {
						CG_Printf( "Prediction miss: %f\n", sqrt( len ) );
					}
					if ( cg_errorDecay.integer ) {
						int		t;
						float	f;

						t = cg.time - cg.predictedErrorTime;
						f = ( cg_errorDecay.value - t ) / cg_errorDecay.value;
						if ( f < 0 ) {
							f = 0;
						} else
						if ( f > 0 && cg_showmiss.integer ) {
							CG_Printf("Double prediction decay: %f\n", f);
						}
						VectorScale( cg.predictedError, f, cg.predictedError );
					} else {
						VectorClear( cg.predictedError );
					}
					VectorAdd( delta, cg.predictedError, cg.predictedError );
					cg.predictedErrorTime = cg.oldTime;
				}
			}
		}

		// don't predict gauntlet firing, which is only supposed to happen
		// when it actually inflicts damage
		cg_pmove.gauntletHit = qfalse;

		if ( cg_pmove.pmove_fixed ) {
			cg_pmove.cmd.serverTime = ((cg_pmove.cmd.serverTime + cg_pmove.pmove_msec-1) / cg_pmove.pmove_msec) * cg_pmove.pmove_msec;
		}
#if 0
		if ( !cg_optimizePrediction.integer ) {
			Pmove (&cg_pmove);
		} else 
#endif
		if ( /*cg_optimizePrediction.integer && */ ( cmdNum >= predictCmd || ( stateIndex + 1 ) % NUM_SAVED_STATES == cg.stateHead ) ) {

			Pmove( &cg_pmove );

			// add push trigger movement effects
			CG_TouchTriggerPrediction();

			// check for expired powerups etc.
			CG_CheckTimers();

			// record the last predicted command
			cg.lastPredictedCommand = cmdNum;
 
			// if we haven't run out of space in the saved states queue
			if( ( stateIndex + 1 ) % NUM_SAVED_STATES != cg.stateHead ) {
				// save the state for the false case ( of cmdNum >= predictCmd )
				// in later calls to this function
				cg.savedPmoveStates[ stateIndex ] = *cg_pmove.ps;
				stateIndex = ( stateIndex + 1 ) % NUM_SAVED_STATES;
				cg.stateTail = stateIndex;
			}
		} else {
			*cg_pmove.ps = cg.savedPmoveStates[ stateIndex ];
			stateIndex = ( stateIndex + 1 ) % NUM_SAVED_STATES;
		}

		moved = qtrue;
	}

	if ( cg_showmiss.integer > 3 ) {
		CG_Printf( "[%i : %i] ", cg_pmove.cmd.serverTime, cg.time );
	}

	if ( !moved ) {
		if ( cg_showmiss.integer ) {
			CG_Printf( "not moved\n" );
		}
		// clean event stack
		eventStack = 0;
		return;
	}

	// adjust for the movement of the groundentity
	CG_AdjustPositionForMover( cg.predictedPlayerState.origin, cg.predictedPlayerState.groundEntityNum, 
		cg.physicsTime, cg.time, cg.predictedPlayerState.origin, 
		cg.predictedPlayerState.viewangles, cg.predictedPlayerState.viewangles );

	if ( cg_showmiss.integer ) {
		if ( cg.predictedPlayerState.eventSequence > oldPlayerState.eventSequence + MAX_PS_EVENTS ) {
			CG_Printf( "WARNING: dropped event\n" );
		}
	}

	// fire events and other transition triggered things
	CG_TransitionPlayerState( &cg.predictedPlayerState, &oldPlayerState );

	if ( cg_showmiss.integer ) {
		if ( cg.eventSequence > cg.predictedPlayerState.eventSequence ) {
			CG_Printf( "WARNING: double event\n" );
			cg.eventSequence = cg.predictedPlayerState.eventSequence;
		}
	}
}
