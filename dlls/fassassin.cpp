/***
 *
 *	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
 *
 *	This product contains software technology licensed from Id
 *	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
 *	All Rights Reserved.
 *
 *   This source code contains proprietary and confidential information of
 *   Valve LLC and its suppliers.  Access to this code is restricted to
 *   persons who have executed a written SDK license with Valve.  Any access,
 *   use or distribution of this code by or to any unlicensed person is illegal.
 *
 ****/

// ============================================================================
// The Whole Half-Life (TWHL) - Programming book
// Chapter 5 - Monsters Programming
//
// by Shepard/Shepard62FR/Shepard62700FR
//
// This file is part of the mentioned book. It is a "copy" of the Female Assassin
// but being friendly minus her grenade attack which is replaced by a healing dart.
// She also does not jump everywhere like her enemy counterpart.
// ============================================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "monsters.h"
#include "soundent.h"
#include "weapons.h"

#include "fassassin.h"



Task_t tlFAssassinFail[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_SET_ACTIVITY, (float)ACT_IDLE},
		{TASK_WAIT_FACE_ENEMY, 2.0f},
		{TASK_SET_SCHEDULE, (float)SCHED_CHASE_ENEMY},
};

Schedule_t slFAssassinFail[] =
	{
		{tlFAssassinFail,
			ARRAYSIZE(tlFAssassinFail),
			bits_COND_LIGHT_DAMAGE |
				bits_COND_HEAVY_DAMAGE |
				bits_COND_PROVOKED |
				bits_COND_CAN_RANGE_ATTACK1 |
				bits_COND_CAN_RANGE_ATTACK2 |
				bits_COND_HEAR_SOUND,

			bits_SOUND_DANGER |
				bits_SOUND_PLAYER,
			"FAssassinFail"},
};

Task_t tlFAssassinHide[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_SET_ACTIVITY, (float)ACT_IDLE},
		{TASK_WAIT, 2.0f},
		{TASK_SET_SCHEDULE, (float)SCHED_CHASE_ENEMY},
};

Schedule_t slFAssassinHide[] =
	{
		{tlFAssassinHide,
			ARRAYSIZE(tlFAssassinHide),
			bits_COND_NEW_ENEMY |
				bits_COND_SEE_ENEMY |
				bits_COND_SEE_FEAR |
				bits_COND_LIGHT_DAMAGE |
				bits_COND_HEAVY_DAMAGE |
				bits_COND_PROVOKED |
				bits_COND_HEAR_SOUND,

			bits_SOUND_DANGER,
			"FAssassinHide"},
};

Task_t tlFAssassinHunt[] =
	{
		{TASK_GET_PATH_TO_ENEMY, 0.0f},
		{TASK_RUN_PATH, 0.0f},
		{TASK_WAIT_FOR_MOVEMENT, 0.0f},
};

Schedule_t slFAssassinHunt[] =
	{
		{tlFAssassinHunt,
			ARRAYSIZE(tlFAssassinHunt),
			bits_COND_NEW_ENEMY |
				bits_COND_CAN_RANGE_ATTACK1 |
				bits_COND_HEAR_SOUND,

			bits_SOUND_DANGER,
			"FAssassinHunt"},
};

Task_t tlFAssassinTakeCoverFromBestSound[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_FIND_COVER_FROM_BEST_SOUND, 0.0f},
		{TASK_RUN_PATH, 0.0f},
		{TASK_WAIT_FOR_MOVEMENT, 0.0f},
		{TASK_REMEMBER, (float)bits_MEMORY_INCOVER},
		{TASK_TURN_LEFT, 179.0f},
};

Schedule_t slFAssassinTakeCoverFromBestSound[] =
	{
		{tlFAssassinTakeCoverFromBestSound,
			ARRAYSIZE(tlFAssassinTakeCoverFromBestSound),
			bits_COND_NEW_ENEMY,
			0,
			"FAssassinTakeCoverFromBestSound"},
};

Task_t tlFAssassinTakeCoverFromEnemy[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_WAIT, 0.2f},
		{TASK_SET_FAIL_SCHEDULE, (float)SCHED_RANGE_ATTACK1},
		{TASK_FIND_COVER_FROM_ENEMY, 0.0f},
		{TASK_RUN_PATH, 0.0f},
		{TASK_WAIT_FOR_MOVEMENT, 0.0f},
		{TASK_REMEMBER, (float)bits_MEMORY_INCOVER},
		{TASK_FACE_ENEMY, 0.0f},
};

Schedule_t slFAssassinTakeCoverFromEnemy[] =
	{
		{tlFAssassinTakeCoverFromEnemy,
			ARRAYSIZE(tlFAssassinTakeCoverFromEnemy),
			bits_COND_NEW_ENEMY |
				bits_COND_HEAR_SOUND,

			bits_SOUND_DANGER,
			"FAssassinTakeCoverFromEnemy"},
};

Task_t tlFAssassinTakeCoverFromEnemy2[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_WAIT, 0.2f},
		{TASK_FACE_ENEMY, 0.0f},
		{TASK_RANGE_ATTACK1, 0.0f},
		{TASK_FIND_FAR_NODE_COVER_FROM_ENEMY, 384.0f},
		{TASK_RUN_PATH, 0.0f},
		{TASK_WAIT_FOR_MOVEMENT, 0.0f},
		{TASK_REMEMBER, (float)bits_MEMORY_INCOVER},
		{TASK_FACE_ENEMY, 0.0f},
};

Schedule_t slFAssassinTakeCoverFromEnemy2[] =
	{
		{tlFAssassinTakeCoverFromEnemy2,
			ARRAYSIZE(tlFAssassinTakeCoverFromEnemy2),
			bits_COND_NEW_ENEMY |
				bits_COND_HEAR_SOUND,

			bits_SOUND_DANGER,
			"FAssassinTakeCoverFromEnemy2"},
};

DEFINE_CUSTOM_SCHEDULES(CFAssassin) {
	slFAssassinFail,
	slFAssassinHide,
	slFAssassinHunt,
	slFAssassinTakeCoverFromBestSound,
	slFAssassinTakeCoverFromEnemy,
	slFAssassinTakeCoverFromEnemy2,
};

IMPLEMENT_CUSTOM_SCHEDULES(CFAssassin, CBaseMonster);

TYPEDESCRIPTION CFAssassin::m_SaveData[] =
	{
		DEFINE_FIELD(CFAssassin, m_flDiviation, FIELD_FLOAT),
		DEFINE_FIELD(CFAssassin, m_flLastShot, FIELD_TIME),

		DEFINE_FIELD(CFAssassin, m_pHealTarget, FIELD_EHANDLE),
		DEFINE_FIELD(CFAssassin, m_flLastHealTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CFAssassin, CBaseMonster);

LINK_ENTITY_TO_CLASS(monster_fassassin, CFAssassin);



void CFAssassin::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/fassassin.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;
	pev->health = gSkillData.hassassinHealth;

	m_flFieldOfView = VIEW_FIELD_WIDE;
	m_MonsterState = MONSTERSTATE_NONE;
	m_afCapability = bits_CAP_HEAR | bits_CAP_DOORS_GROUP | bits_CAP_TURN_HEAD | bits_CAP_RANGE_ATTACK1 | bits_CAP_RANGE_ATTACK2;

	m_HackedGunPos = Vector(0.0f, 24.0f, 48.0f);

	MonsterInit();
}

void CFAssassin::Precache()
{
	PRECACHE_MODEL("models/fassassin.mdl");

	PRECACHE_SOUND("weapons/pl_gun1.wav");
	PRECACHE_SOUND("weapons/pl_gun2.wav");

	m_iShell = PRECACHE_MODEL("models/shell.mdl");

	UTIL_PrecacheOther("healing_dart");
}



int CFAssassin::Classify()
{
	return CLASS_PLAYER_ALLY;
}

int CFAssassin::ISoundMask()
{
	return bits_SOUND_WORLD |
		bits_SOUND_COMBAT |
		bits_SOUND_CARCASS |
		bits_SOUND_MEAT |
		bits_SOUND_GARBAGE |
		bits_SOUND_DANGER |
		bits_SOUND_PLAYER;
}

void CFAssassin::SetYawSpeed()
{
	pev->yaw_speed = 360.0f;
}



bool CFAssassin::CheckRangeAttack1(float flDot, float flDist)
{
	if (HasConditions(bits_COND_ENEMY_OCCLUDED) || flDist <= 64.0f || flDist > 2048.0f)
		return false;

	TraceResult tr;
	Vector vecSrc = GetGunPosition();
	UTIL_TraceLine(vecSrc, m_hEnemy->BodyTarget(vecSrc), dont_ignore_monsters, ENT(pev), &tr);
	return tr.flFraction == 1.0f || tr.pHit == m_hEnemy->edict();
}



void CFAssassin::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
	case FASSASSIN_AE_SHOOT:
	{
		Vector vecOrigin = GetGunPosition();
		Vector vecDir = ShootAtEnemy(vecOrigin);

		if (m_flLastShot + 2.0f < gpGlobals->time)
		{
			m_flDiviation = 0.10f;
		}
		else
		{
			m_flDiviation -= 0.01f;
			if (m_flDiviation < 0.02f)
				m_flDiviation = 0.02f;
		}
		m_flLastShot = gpGlobals->time;

		UTIL_MakeVectors(pev->angles);

		Vector vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f) + gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f) + gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f);
		EjectBrass(pev->origin + gpGlobals->v_up * 32.0f + gpGlobals->v_forward * 12.0f, vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);
		FireBullets(1, vecOrigin, vecDir, Vector(m_flDiviation, m_flDiviation, m_flDiviation), 2048.0f, BULLET_MONSTER_9MM);

		switch (RANDOM_LONG(0, 1))
		{
		case 0:
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/pl_gun1.wav", RANDOM_FLOAT(0.6f, 0.8f), ATTN_NORM);
			break;
		case 1:
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/pl_gun2.wav", RANDOM_FLOAT(0.6f, 0.8f), ATTN_NORM);
			break;
		}

		pev->effects |= EF_MUZZLEFLASH;

		Vector angDir = UTIL_VecToAngles(vecDir);
		SetBlending(0, angDir.x);
	}
	break;
	case FASSASSIN_AE_SHOOT_HEALING_DART:
	{
		Vector vecOrigin = GetGunPosition();
		Vector vecDir = gpGlobals->v_forward;
		if (m_pHealTarget)
		{
			MakeIdealYaw(m_pHealTarget->pev->origin);
			ChangeYaw(pev->yaw_speed);
			vecDir = ((m_pHealTarget->BodyTarget(vecOrigin) - m_pHealTarget->pev->origin) + m_pHealTarget->pev->origin - vecOrigin).Normalize();
		}

		CHealingDart* pDart = CHealingDart::DartCreate();
		pDart->pev->origin = vecOrigin;
		pDart->pev->angles = UTIL_VecToAngles(vecDir);
		pDart->pev->owner = edict();
		pDart->pev->velocity = vecDir * 2048.0f;
		pDart->pev->speed = 2048.0f;
		pDart->pev->avelocity.z = 10.0f;

		switch (RANDOM_LONG(0, 1))
		{
		case 0:
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/pl_gun1.wav", VOL_NORM, ATTN_NORM, 0, 150);
			break;
		case 1:
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/pl_gun1.wav", VOL_NORM, ATTN_NORM, 0, 150);
			break;
		}

		pev->effects |= EF_MUZZLEFLASH;

		Vector angDir = UTIL_VecToAngles(vecDir);
		SetBlending(0, angDir.x);
		m_flLastHealTime = gpGlobals->time + 10.0f;
		m_pHealTarget = nullptr;
	}
	break;
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
	}
}



Schedule_t* CFAssassin::GetSchedule()
{
	if (HasConditions(bits_COND_HEAR_SOUND))
	{
		CSound* pSound = PBestSound();
		if (pSound && (pSound->m_iType & bits_SOUND_DANGER) != 0)
			return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
	}

	m_pHealTarget = CheckHealTarget();
	if (m_pHealTarget)
		return GetScheduleOfType(SCHED_RANGE_ATTACK2);

	if (m_MonsterState != MONSTERSTATE_COMBAT || HasConditions(bits_COND_ENEMY_DEAD))
		return CBaseMonster::GetSchedule();

	if (HasConditions(bits_COND_CAN_RANGE_ATTACK1))
		return GetScheduleOfType(SCHED_RANGE_ATTACK1);

	if (HasConditions(bits_COND_SEE_ENEMY))
		return GetScheduleOfType(SCHED_COMBAT_FACE);

	if (HasConditions(bits_COND_NEW_ENEMY))
		return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);

	return GetScheduleOfType(SCHED_ALERT_STAND);
}

Schedule_t* CFAssassin::GetScheduleOfType(int Type)
{
	switch (Type)
	{
	case SCHED_TAKE_COVER_FROM_ENEMY:
		return pev->health > 30 ? slFAssassinTakeCoverFromEnemy : slFAssassinTakeCoverFromEnemy2;
	case SCHED_TAKE_COVER_FROM_BEST_SOUND:
		return slFAssassinTakeCoverFromBestSound;
	case SCHED_FAIL:
		if (m_MonsterState == MONSTERSTATE_COMBAT)
			return slFAssassinFail;
		break;
	case SCHED_ALERT_STAND:
		if (m_MonsterState == MONSTERSTATE_COMBAT)
			return slFAssassinHide;
		break;
	case SCHED_CHASE_ENEMY:
		return slFAssassinHunt;
	}

	return CBaseMonster::GetScheduleOfType(Type);
}



void CFAssassin::Killed(entvars_t* pevAttacker, int iGib)
{
	if (pev->body <= 0)
	{
		pev->body = 1;

		Vector vecOrigin;
		Vector vecAngles;
		GetAttachment(0, vecOrigin, vecAngles);
		DropItem("weapon_9mmhandgun", vecOrigin, vecAngles);
	}

	CBaseMonster::Killed(pevAttacker, iGib);
}



CBaseEntity* CFAssassin::CheckHealTarget()
{
	if (m_flLastHealTime > gpGlobals->time)
		return nullptr;

	CBaseEntity* pTarget = nullptr;
	while ((pTarget = UTIL_FindEntityInSphere(pTarget, pev->origin, 256.0f)) != nullptr)
	{
		if (pTarget->MyMonsterPointer() == nullptr || !pTarget->IsAlive() || IRelationship(pTarget) != R_AL || pTarget->pev->velocity.Length() > 64.0f)
			continue;

		if (pTarget->IsPlayer() && pTarget->pev->health <= pTarget->pev->max_health / 2.0f)
			return pTarget;

		if (!pTarget->IsPlayer() && pTarget->pev->health <= 10)
			return pTarget;
	}
	return nullptr;
}



LINK_ENTITY_TO_CLASS(healing_dart, CHealingDart);

CHealingDart* CHealingDart::DartCreate()
{
	CHealingDart* pDart = GetClassPtr((CHealingDart*)nullptr);
	pDart->pev->classname = MAKE_STRING("healing_dart");
	pDart->Spawn();
	return pDart;
}



void CHealingDart::Spawn()
{
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->gravity = 0.5f;

	SET_MODEL(ENT(pev), "models/crossbow_bolt.mdl");

	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));

	SetTouch(&CHealingDart::DartTouch);
	SetThink(&CHealingDart::BubbleThink);
	pev->nextthink = gpGlobals->time + 0.2f;
}


void CHealingDart::Precache()
{
	PRECACHE_MODEL("models/crossbow_bolt.mdl");

	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");

	m_iTrail = PRECACHE_MODEL("sprites/streak.spr");
}



int CHealingDart::Classify()
{
	return CLASS_NONE;
}



void CHealingDart::DartTouch(CBaseEntity* pOther)
{
	SetTouch(nullptr);
	SetThink(nullptr);

	if (0 != pOther->pev->takedamage)
	{
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t* pevOwner = VARS(pev->owner);

		if (pOther->IsAlive())
			pOther->TakeHealth(25.0f, DMG_GENERIC);

		pev->velocity = Vector(0.0f, 0.0f, 0.0f);
		switch (RANDOM_LONG(0, 1))
		{
		case 0:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod1.wav", VOL_NORM, ATTN_NORM);
			break;
		case 1:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod2.wav", VOL_NORM, ATTN_NORM);
			break;
		}

		Killed(pev, GIB_NEVER);
	}
	else
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 98 + RANDOM_LONG(0, 7));

		SetThink(&CHealingDart::SUB_Remove);
		pev->nextthink = gpGlobals->time;

		if (FClassnameIs(pOther->pev, "worldspawn"))
		{
			Vector vecDir = pev->velocity.Normalize();
			UTIL_SetOrigin(pev, pev->origin - vecDir * 12.0f);
			pev->angles = UTIL_VecToAngles(vecDir);
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector(0.0f, 0.0f, 0.0f);
			pev->avelocity.z = 0.0f;
			pev->angles.z = RANDOM_LONG(0.0f, 360.0f);
			pev->nextthink = gpGlobals->time + 10.0f;
		}

		if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
			UTIL_Sparks(pev->origin);
	}
}

void CHealingDart::BubbleThink()
{
	pev->nextthink = gpGlobals->time + 0.1f;
	if (pev->waterlevel == 0)
		return;

	UTIL_BubbleTrail(pev->origin - pev->velocity * 0.1f, pev->origin, 1);
}
