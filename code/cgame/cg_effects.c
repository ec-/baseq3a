// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_effects.c -- these functions generate localentities, usually as a result
// of event processing

#include "cg_local.h"


/*
==================
CG_BubbleTrail

Bullets shot underwater
==================
*/
void CG_BubbleTrail( const vec3_t start, const vec3_t end, float spacing ) {
	vec3_t		move;
	vec3_t		vec;
	float		len;
	int			i;

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);

	// advance a random amount first
	i = rand() % (int)spacing;
	VectorMA( move, i, vec, move );

	VectorScale (vec, spacing, vec);

	for ( ; i < len; i += spacing ) {
		localEntity_t	*le;
		refEntity_t		*re;

		le = CG_AllocLocalEntity();
		le->leFlags = LEF_PUFF_DONT_SCALE;
		le->leType = LE_MOVE_SCALE_FADE;
		le->startTime = cg.time;
		le->endTime = cg.time + 1000 + random() * 250;
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );

		re = &le->refEntity;
		if ( intShaderTime )
			re->u.intShaderTime = cg.time;
		else
			re->u.shaderTime = cg.time / 1000.0f;

		re->reType = RT_SPRITE;
		re->rotation = 0;
		re->radius = 3;
		re->customShader = cgs.media.waterBubbleShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;

		le->color[3] = 1.0;

		le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		VectorCopy( move, le->pos.trBase );
		le->pos.trDelta[0] = crandom()*5;
		le->pos.trDelta[1] = crandom()*5;
		le->pos.trDelta[2] = crandom()*5 + 6;

		VectorAdd (move, vec, move);
	}
}

/*
=====================
CG_SmokePuff

Adds a smoke puff or blood trail localEntity.
=====================
*/
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel, 
				   float radius,
				   float r, float g, float b, float a,
				   float duration,
				   int startTime,
				   int fadeInTime,
				   int leFlags,
				   qhandle_t hShader ) {
	static int	seed = 0x92;
	localEntity_t	*le;
	refEntity_t		*re;
//	int fadeInTime = startTime + duration / 2;

	le = CG_AllocLocalEntity();
	le->leFlags = leFlags;
	le->radius = radius;

	re = &le->refEntity;
	re->rotation = Q_random( &seed ) * 360;
	re->radius = radius;

	if ( intShaderTime )
		re->u.intShaderTime = startTime;
	else
		re->u.shaderTime = startTime / 1000.0f;

	le->leType = LE_MOVE_SCALE_FADE;
	le->startTime = startTime;
	le->fadeInTime = fadeInTime;
	le->endTime = startTime + duration;
	if ( fadeInTime > startTime ) {
		le->lifeRate = 1.0 / ( le->endTime - le->fadeInTime );
	}
	else {
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	}
	le->color[0] = r;
	le->color[1] = g; 
	le->color[2] = b;
	le->color[3] = a;


	le->pos.trType = TR_LINEAR;
	le->pos.trTime = startTime;
	VectorCopy( vel, le->pos.trDelta );
	VectorCopy( p, le->pos.trBase );

	VectorCopy( p, re->origin );
	re->customShader = hShader;

	// rage pro can't alpha fade, so use a different shader
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		re->customShader = cgs.media.smokePuffRageProShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;
	} else {
		re->shaderRGBA[0] = le->color[0] * 0xff;
		re->shaderRGBA[1] = le->color[1] * 0xff;
		re->shaderRGBA[2] = le->color[2] * 0xff;
		re->shaderRGBA[3] = 0xff;
	}

	re->reType = RT_SPRITE;
	re->radius = le->radius;

	return le;
}

/*
==================
CG_SpawnEffect

Player teleporting in or out
==================
*/
void CG_SpawnEffect( const vec3_t origin ) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + 500;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;

	re = &le->refEntity;

	re->reType = RT_MODEL;

	if ( intShaderTime )
		re->u.intShaderTime = cg.time;
	else
		re->u.shaderTime = cg.time / 1000.0f;

#ifndef MISSIONPACK
	re->customShader = cgs.media.teleportEffectShader;
#endif
	re->hModel = cgs.media.teleportEffectModel;
	AxisClear( re->axis );

	VectorCopy( origin, re->origin );

#ifdef MISSIONPACK
	re->origin[2] += 16;
#else
	re->origin[2] -= 24;
#endif
}


#ifdef MISSIONPACK
/*
===============
CG_LightningBoltBeam
===============
*/
void CG_LightningBoltBeam( vec3_t start, vec3_t end ) {
	localEntity_t	*le;
	refEntity_t		*beam;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_SHOWREFENTITY;
	le->startTime = cg.time;
	le->endTime = cg.time + 50;

	beam = &le->refEntity;

	VectorCopy( start, beam->origin );
	// this is the end point
	VectorCopy( end, beam->oldorigin );

	beam->reType = RT_LIGHTNING;
	beam->customShader = cgs.media.lightningShader;
}


/*
==================
CG_KamikazeEffect
==================
*/
void CG_KamikazeEffect( vec3_t org ) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_KAMIKAZE;
	le->startTime = cg.time;
	le->endTime = cg.time + 3000;//2250;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;

	VectorClear(le->angles.trBase);

	re = &le->refEntity;

	re->reType = RT_MODEL;

	if ( intShaderTime )
		re->intShaderTime = cg.time;
	else
		re->shaderTime = cg.time / 1000.0f;

	re->hModel = cgs.media.kamikazeEffectModel;

	VectorCopy( org, re->origin );

}

/*
==================
CG_ObeliskExplode
==================
*/
void CG_ObeliskExplode( vec3_t org, int entityNum ) {
	localEntity_t	*le;
	vec3_t origin;

	// create an explosion
	VectorCopy( org, origin );
	origin[2] += 64;
	le = CG_MakeExplosion( origin, vec3_origin,
						   cgs.media.dishFlashModel,
						   cgs.media.rocketExplosionShader,
						   600, qtrue );
	le->light = 300;
	le->lightColor[0] = 1;
	le->lightColor[1] = 0.75;
	le->lightColor[2] = 0.0;
}

/*
==================
CG_ObeliskPain
==================
*/
void CG_ObeliskPain( vec3_t org ) {
	float r;
	sfxHandle_t sfx;

	// hit sound
	r = rand() & 3;
	if ( r < 2 ) {
		sfx = cgs.media.obeliskHitSound1;
	} else if ( r == 2 ) {
		sfx = cgs.media.obeliskHitSound2;
	} else {
		sfx = cgs.media.obeliskHitSound3;
	}
	trap_S_StartSound ( org, ENTITYNUM_NONE, CHAN_BODY, sfx );
}


/*
==================
CG_InvulnerabilityImpact
==================
*/
void CG_InvulnerabilityImpact( vec3_t org, vec3_t angles ) {
	localEntity_t	*le;
	refEntity_t		*re;
	int				r;
	sfxHandle_t		sfx;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_INVULIMPACT;
	le->startTime = cg.time;
	le->endTime = cg.time + 1000;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;

	re = &le->refEntity;

	re->reType = RT_MODEL;

	if ( intShaderTime )
		re->u.intShaderTime = cg.time;
	else
		re->u.shaderTime = cg.time / 1000.0f;

	re->hModel = cgs.media.invulnerabilityImpactModel;

	VectorCopy( org, re->origin );
	AnglesToAxis( angles, re->axis );

	r = rand() & 3;
	if ( r < 2 ) {
		sfx = cgs.media.invulnerabilityImpactSound1;
	} else if ( r == 2 ) {
		sfx = cgs.media.invulnerabilityImpactSound2;
	} else {
		sfx = cgs.media.invulnerabilityImpactSound3;
	}
	trap_S_StartSound (org, ENTITYNUM_NONE, CHAN_BODY, sfx );
}

/*
==================
CG_InvulnerabilityJuiced
==================
*/
void CG_InvulnerabilityJuiced( vec3_t org ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			angles;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_INVULJUICED;
	le->startTime = cg.time;
	le->endTime = cg.time + 10000;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;

	re = &le->refEntity;

	re->reType = RT_MODEL;

	if ( intShaderTime )
		re->u.intShaderTime = cg.time;
	else
		re->u.shaderTime = cg.time / 1000.0f;

	re->hModel = cgs.media.invulnerabilityJuicedModel;

	VectorCopy( org, re->origin );
	VectorClear(angles);
	AnglesToAxis( angles, re->axis );

	trap_S_StartSound (org, ENTITYNUM_NONE, CHAN_BODY, cgs.media.invulnerabilityJuicedSound );
}
#endif


/*
==================
CG_ScorePlum
==================
*/
void CG_ScorePlum( int client, const vec3_t origin, int score ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			angles;
	static vec3_t lastPos;

	// only visualize for the client that scored
	if (client != cg.predictedPlayerState.clientNum || cg_scorePlum.integer == 0) {
		return;
	}

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_SCOREPLUM;
	le->startTime = cg.time;
	le->endTime = cg.time + 4000;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	
	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;
	le->radius = score;
	
	VectorCopy( origin, le->pos.trBase );
	if ( origin[2] >= lastPos[2] - 20 && origin[2] <= lastPos[2] + 20 ) {
		le->pos.trBase[2] -= 20;
	}

	//CG_Printf( "Plum origin %i %i %i -- %i\n", (int)org[0], (int)org[1], (int)org[2], (int)Distance(org, lastPos));
	VectorCopy(origin, lastPos);

	re = &le->refEntity;

	re->reType = RT_SPRITE;
	re->radius = 16;

	VectorClear(angles);
	AnglesToAxis( angles, re->axis );
}


/*
====================
CG_MakeExplosion
====================
*/
localEntity_t *CG_MakeExplosion( const vec3_t origin, const vec3_t dir,
								qhandle_t hModel, qhandle_t shader,
								int msec, qboolean isSprite ) {
	float			ang;
	localEntity_t	*ex;
	int				offset;
	vec3_t			tmpVec, newOrigin;

	if ( msec <= 0 ) {
		CG_Error( "CG_MakeExplosion: msec = %i", msec );
	}

	// skew the time a bit so they aren't all in sync
	offset = rand() & 63;

	ex = CG_AllocLocalEntity();
	if ( isSprite ) {
		ex->leType = LE_SPRITE_EXPLOSION;

		// randomly rotate sprite orientation
		ex->refEntity.rotation = rand() % 360;
		VectorScale( dir, 16, tmpVec );
		VectorAdd( tmpVec, origin, newOrigin );
	} else {
		ex->leType = LE_EXPLOSION;
		VectorCopy( origin, newOrigin );

		// set axis with random rotate
		if ( !dir ) {
			AxisClear( ex->refEntity.axis );
		} else {
			ang = rand() % 360;
			VectorCopy( dir, ex->refEntity.axis[0] );
			RotateAroundDirection( ex->refEntity.axis, ang );
		}
	}

	ex->startTime = cg.time - offset;
	ex->endTime = ex->startTime + msec;

	// bias the time so all shader effects start correctly
	if ( intShaderTime )
		ex->refEntity.u.intShaderTime = ex->startTime;
	else
		ex->refEntity.u.shaderTime = ex->startTime / 1000.0f;

	ex->refEntity.hModel = hModel;
	ex->refEntity.customShader = shader;

	// set origin
	VectorCopy( newOrigin, ex->refEntity.origin );
	VectorCopy( newOrigin, ex->refEntity.oldorigin );

	ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

	return ex;
}


/*
=================
CG_Bleed

This is the spurt of blood when a character gets hit
=================
*/
void CG_Bleed( const vec3_t origin, int entityNum ) {
	localEntity_t	*ex;

	if ( !cg_blood.integer ) {
		return;
	}

	ex = CG_AllocLocalEntity();
	ex->leType = LE_EXPLOSION;

	ex->startTime = cg.time;
	ex->endTime = ex->startTime + 500;
	
	VectorCopy ( origin, ex->refEntity.origin);
	ex->refEntity.reType = RT_SPRITE;
	ex->refEntity.rotation = rand() % 360;
	ex->refEntity.radius = 24;

	ex->refEntity.customShader = cgs.media.bloodExplosionShader;

	// don't show player's own blood in view
	if ( entityNum == cg.snap->ps.clientNum ) {
		ex->refEntity.renderfx |= RF_THIRD_PERSON;
	}
}



/*
==================
CG_LaunchGib
==================
*/
static void CG_LaunchGib( const vec3_t origin, const vec3_t angles,
						const vec3_t velocity, qhandle_t hModel ) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + 5000 + random() * 3000;

	VectorCopy( origin, re->origin );
	AnglesToAxis( angles, re->axis );
	re->hModel = hModel;

	le->pos.trType = TR_GRAVITY;
	VectorCopy( origin, le->pos.trBase );
	VectorCopy( velocity, le->pos.trDelta );
	le->pos.trTime = cg.time;

	le->bounceFactor = cg_oldGibs.integer ? 0.6f : cg_gibsBounceFactor.value;

	if (!cg_oldGibs.integer) {
		// `VectorLength` would be more precise, but this is faster
		// and good enough for randomness.
		float speedIsh = fabs(velocity[0]) + fabs(velocity[1]) + fabs(velocity[2]);
		int i;
		int mainRotationAxis = rand() % 3;

		le->leFlags = LEF_TUMBLE;
		le->angles.trType = TR_LINEAR;
		le->angles.trTime = cg.time;
		VectorCopy( angles, le->angles.trBase );
		// Just a few degrees of randomness.
		le->angles.trBase[PITCH] += rand()&7;
		le->angles.trBase[YAW] += rand()&7;
		le->angles.trBase[ROLL] += rand()&7;
		// TODO the tumble speed should probably depend on damage instead,
		// or at least on random velocity.
		for ( i = 0; i < 3; i++ ) {
			// The numbers are not based on science, but it looks like
			// having one axis be bigger than others makes rotation look natural.
			float axisMul = mainRotationAxis == i ? 1 : 0.25;
			le->angles.trDelta[i] = speedIsh * axisMul * 0.5 *
				cg_gibsRotationFactor.value * crandom();
		}
	}

	le->leBounceSoundType = LEBS_BLOOD;
	le->leMarkType = LEMT_BLOOD;
}

// If it's a dead body playing a death animation,
// gradually transition the body position and angles from upright
// to "lying flat on the ground".
void AdjustPositionIfDeathAnimation( const lerpFrame_t *anim, vec3_t origin,
	vec3_t bodyAngles, vec3_t lookDirAngles ) {
	// 0 means that the body is fully erect,
	// 1 means it's lying flat on the ground.
	float deathAnimationProgress = 0;
	if (
		// Is this a death / dead animation?
		(anim->animationNumber & ~ANIM_TOGGLEBIT) <= BOTH_DEAD3 &&
		(anim->animationNumber & ~ANIM_TOGGLEBIT) >= BOTH_DEATH1 &&
		// More sanity checks
		anim->animation &&
		anim->animation->numFrames > 0
	) {
		const int frameOfAnimation = anim->frame - anim->animation->firstFrame;
		if (
			frameOfAnimation < 0 ||
			frameOfAnimation >= anim->animation->numFrames
		) {
			// Out of range. This seems to happen
			// when we haven't yet managed to start the death animation.
			// Maybe we're looking at the wrong things,
			// but this works fine.
			deathAnimationProgress = 0;
		} else {
			deathAnimationProgress =
				(float)(frameOfAnimation + 1) / anim->animation->numFrames;
		}
	}

	// TODO fix: with body sinking, gibs get stuck in the floor.
	origin[2] += deathAnimationProgress * (MINS_Z + PLAYER_WIDTH / 1.8f);
	// From upright to facing up.
	// TODO fix: but sometimes the "dead" animation is such that
	// the player is facing down.
	bodyAngles[PITCH] = 360 - deathAnimationProgress * 90;
	lookDirAngles[PITCH] += - deathAnimationProgress * 90;
	// Normalize. Doesn't seem to be necessary, but let's do it.
	if (lookDirAngles[PITCH] < 0) {
		lookDirAngles[PITCH] += 360;
	}
}
/*
===================
CG_GibPlayer

Generated a bunch of gibs launching out from the bodies location
===================
*/
#define	DEFAULT_NUM_GIBS	10
#define	GIB_VELOCITY		250
#define	GIB_JUMP			250
void CG_GibPlayer( const vec3_t playerOrigin, const vec3_t playerAngles,
					const vec3_t playerVelocity,
					const lerpFrame_t *bodyAnimation ) {
	vec3_t	baseOrigin, origin, velocity;
	// Generally only the head should have pitch,
	// the rest of the body is upright.
	vec3_t	bodyAngles;
	vec3_t	lookDirAngles, angles;
	vec3_t	forward, right, up;
	// See `playerMins`, `playerMaxs`.
	// TODO we could try to check the actual `mins` and `maxs`
	// (do we have them available on the client though?),
	// to account for crounching.
	float playerHeight = 32 - MINS_Z;
	float playerRadius = PLAYER_WIDTH;
	float baseRandomVelocity = cg_gibsExtraRandomVelocity.value;
	vec3_t playerVelocityScaled;
	float jump = cg_gibsExtraVerticalVelocity.value;
	int numGibs = cg_gibs.value * DEFAULT_NUM_GIBS;
	qboolean skullLaunched = qfalse; // launch only one skull.

	if ( !cg_blood.integer ) {
		return;
	}

	VectorCopy( playerOrigin, baseOrigin );
	VectorCopy( playerAngles, lookDirAngles );
	VectorCopy( playerAngles, bodyAngles );
	if ( bodyAnimation ) {
		AdjustPositionIfDeathAnimation( bodyAnimation, baseOrigin, bodyAngles, lookDirAngles );
	} else {
		bodyAngles[PITCH] = 0;
	}
	// TODO fix: if `AdjustPositionIfDeathAnimation()` ran,
	// `forward` could potentially be pointing up,
	// so some of our velocity calculations below are not quite right.
	AngleVectors( bodyAngles, forward, right, up );

	VectorScale( playerVelocity, cg_gibsInheritPlayerVelocity.value, playerVelocityScaled );

	do {
		// Note that one gib will get launched even if `numGibs == 0`.
		// This is in line with the original behavior of `CG_GibPlayer`.

		VectorCopy( baseOrigin, origin );
		VectorMA(origin, MINS_Z + 0.95 * playerHeight, up, origin);
		VectorClear( velocity );
		velocity[0] = crandom()*baseRandomVelocity;
		velocity[1] = crandom()*baseRandomVelocity;
		// For the skull / brain we want the random velocity
		// to never have downwards (inwards) component,
		// so we use `random` instead of `crandom`.
		// We also do the same for other gibs,
		// but for the left / right velocity components.
		velocity[2] = jump + random()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		if ( !skullLaunched && (rand() & 1) ) {
			CG_LaunchGib( origin, lookDirAngles, velocity, cgs.media.gibSkull );
			skullLaunched = qtrue;
		} else {
			CG_LaunchGib( origin, lookDirAngles, velocity, cgs.media.gibBrain );
		}
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.65 * playerHeight, up, origin );
		VectorClear( velocity );
		velocity[0] = crandom()*baseRandomVelocity;
		velocity[1] = crandom()*baseRandomVelocity;
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		CG_LaunchGib( origin, bodyAngles, velocity, cgs.media.gibAbdomen );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.78 * playerHeight, up, origin );
		VectorMA( origin, 0.8 * playerRadius, right, origin );
		VectorMA( origin, -0.3 * playerRadius, forward, origin );
		VectorClear( velocity );
		VectorMA( velocity, +random()*baseRandomVelocity, right, velocity );
		VectorMA( velocity, crandom()*baseRandomVelocity, forward, velocity );
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		VectorCopy( bodyAngles, angles );
		angles[ROLL] += 70;
		angles[PITCH] += 45;
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibArm );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.80 * playerHeight, up, origin );
		VectorClear( velocity );
		// Chest is a more "central" and "heavier" piece,
		// so it gets less random velocity.
		velocity[0] = 0.5*crandom()*baseRandomVelocity;
		velocity[1] = 0.5*crandom()*baseRandomVelocity;
		velocity[2] = jump + 0.5*crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		CG_LaunchGib( origin, bodyAngles, velocity, cgs.media.gibChest );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.66 * playerHeight, up, origin );
		VectorMA( origin, 0.8 * playerRadius, right, origin );
		VectorMA( origin, 0.2 * playerRadius, forward, origin );
		VectorClear( velocity );
		velocity[0] = crandom()*baseRandomVelocity;
		velocity[1] = crandom()*baseRandomVelocity;
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		VectorCopy( bodyAngles, angles );
		angles[PITCH] -= 80;
		angles[YAW] += 50;
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibFist );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.05 * playerHeight, up, origin );
		VectorMA( origin, -0.5 * playerRadius, right, origin );
		VectorMA( origin, -0.5 * playerRadius, forward, origin );
		VectorClear( velocity );
		velocity[0] = crandom()*baseRandomVelocity;
		velocity[1] = crandom()*baseRandomVelocity;
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		CG_LaunchGib( origin, bodyAngles, velocity, cgs.media.gibFoot );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.65 * playerHeight, up, origin );
		VectorMA( origin, -0.6 * playerRadius, right, origin );
		VectorMA( origin, +0.2 * playerRadius, forward, origin );
		VectorClear( velocity );
		VectorMA( velocity, -random()*baseRandomVelocity, right, velocity );
		VectorMA( velocity, crandom()*baseRandomVelocity, forward, velocity );
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		VectorCopy( bodyAngles, angles );
		angles[ROLL] -= 90;
		angles[PITCH] -= 75;
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibForearm );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.57 * playerHeight, up, origin );
		VectorClear( velocity );
		velocity[0] = crandom()*baseRandomVelocity;
		velocity[1] = crandom()*baseRandomVelocity;
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		CG_LaunchGib( origin, bodyAngles, velocity, cgs.media.gibIntestine );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.42 * playerHeight, up, origin );
		VectorMA( origin, 0.5 * playerRadius, right, origin );
		VectorMA( origin, 0.1 * playerRadius, forward, origin );
		VectorClear( velocity );
		VectorMA( velocity, +random()*baseRandomVelocity, right, velocity );
		VectorMA( velocity, crandom()*baseRandomVelocity, forward, velocity );
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		VectorCopy( bodyAngles, angles );
		angles[ROLL] -= 30;
		angles[PITCH] -= 15;
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibLeg );
		if (--numGibs <= 0) {
			return;
		}

		VectorCopy( baseOrigin, origin );
		VectorMA( origin, MINS_Z + 0.44 * playerHeight, up, origin );
		VectorMA( origin, -0.5 * playerRadius, right, origin );
		VectorMA( origin, -0.2 * playerRadius, forward, origin );
		VectorClear( velocity );
		VectorMA( velocity, -random()*baseRandomVelocity, right, velocity );
		VectorMA( velocity, crandom()*baseRandomVelocity, forward, velocity );
		velocity[2] = jump + crandom()*baseRandomVelocity;
		VectorAdd( velocity, playerVelocityScaled, velocity );
		VectorCopy( bodyAngles, angles );
		angles[PITCH] += 15;
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibLeg );
		if (--numGibs <= 0) {
			return;
		}
	} while (numGibs > 0);
}
void CG_GibPlayerOld( const vec3_t playerOrigin ) {
	vec3_t	origin, angles, velocity;

	if ( !cg_blood.integer ) {
		return;
	}

	VectorClear(angles);

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	if ( rand() & 1 ) {
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibSkull );
	} else {
		CG_LaunchGib( origin, angles, velocity, cgs.media.gibBrain );
	}

	// allow gibs to be turned off for speed
	if ( !cg_gibs.integer ) {
		return;
	}

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibAbdomen );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibArm );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibChest );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibFist );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibFoot );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibForearm );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibIntestine );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibLeg );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*GIB_VELOCITY;
	velocity[1] = crandom()*GIB_VELOCITY;
	velocity[2] = GIB_JUMP + crandom()*GIB_VELOCITY;
	CG_LaunchGib( origin, angles, velocity, cgs.media.gibLeg );
}

/*
==================
CG_LaunchExplode
==================
*/
void CG_LaunchExplode( vec3_t origin, vec3_t velocity, qhandle_t hModel ) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + 10000 + random() * 6000;

	VectorCopy( origin, re->origin );
	AxisCopy( axisDefault, re->axis );
	re->hModel = hModel;

	le->pos.trType = TR_GRAVITY;
	VectorCopy( origin, le->pos.trBase );
	VectorCopy( velocity, le->pos.trDelta );
	le->pos.trTime = cg.time;

	le->bounceFactor = 0.1f;

	le->leBounceSoundType = LEBS_BRASS;
	le->leMarkType = LEMT_NONE;
}

#define	EXP_VELOCITY	100
#define	EXP_JUMP		150
/*
===================
CG_BigExplode

Generated a bunch of gibs launching out from the bodies location
===================
*/
void CG_BigExplode( vec3_t playerOrigin ) {
	vec3_t	origin, velocity;

	if ( !cg_blood.integer ) {
		return;
	}

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*EXP_VELOCITY;
	velocity[1] = crandom()*EXP_VELOCITY;
	velocity[2] = EXP_JUMP + crandom()*EXP_VELOCITY;
	CG_LaunchExplode( origin, velocity, cgs.media.smoke2 );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*EXP_VELOCITY;
	velocity[1] = crandom()*EXP_VELOCITY;
	velocity[2] = EXP_JUMP + crandom()*EXP_VELOCITY;
	CG_LaunchExplode( origin, velocity, cgs.media.smoke2 );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*EXP_VELOCITY*1.5;
	velocity[1] = crandom()*EXP_VELOCITY*1.5;
	velocity[2] = EXP_JUMP + crandom()*EXP_VELOCITY;
	CG_LaunchExplode( origin, velocity, cgs.media.smoke2 );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*EXP_VELOCITY*2.0;
	velocity[1] = crandom()*EXP_VELOCITY*2.0;
	velocity[2] = EXP_JUMP + crandom()*EXP_VELOCITY;
	CG_LaunchExplode( origin, velocity, cgs.media.smoke2 );

	VectorCopy( playerOrigin, origin );
	velocity[0] = crandom()*EXP_VELOCITY*2.5;
	velocity[1] = crandom()*EXP_VELOCITY*2.5;
	velocity[2] = EXP_JUMP + crandom()*EXP_VELOCITY;
	CG_LaunchExplode( origin, velocity, cgs.media.smoke2 );
}

