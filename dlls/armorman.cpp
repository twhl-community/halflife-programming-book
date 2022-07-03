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
// Chapter 4 - Monsters Programming
//
// by Shepard/Shepard62FR/Shepard62700FR
//
// This file is part of the mentioned book. It is a re-creation of the Armored
// Man from Half-Rats: Parasomnia.
//
// Important legal/copyright stuff:
//
// Heath Games, the developers of Half-Rats: Parasomnia allow usage of its
// assets and derivatives in other projects as long as:
//
// 1) You do not use the assets and any derivatives for commercial purposes.
//
// 2) You give the proper credits to Heath Games; do not claim you made the
// assets.
// ============================================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "monsters.h"
#include "soundent.h"
#include "weapons.h"

#include "armorman.h"



Task_t tlArmorManReload[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_FACE_ENEMY, 0.0f},
		{TASK_PLAY_SEQUENCE, ACT_RELOAD},
};

Task_t tlArmorManShield[] =
	{
		{TASK_STOP_MOVING, 0.0f},
		{TASK_PLAY_SEQUENCE_FACE_ENEMY, ACT_EXCITED},
};

Schedule_t slArmorManReload[] =
	{
		{tlArmorManReload,
			ARRAYSIZE(tlArmorManReload),
			bits_COND_HEAVY_DAMAGE | bits_COND_HEAR_SOUND,
			bits_SOUND_DANGER,
			"ArmorMan Reload"},
};

Schedule_t slArmorManShield[] =
	{
		{tlArmorManShield,
			ARRAYSIZE(tlArmorManShield),
			0,
			0,
			"ArmorMan Shield"},
};

DEFINE_CUSTOM_SCHEDULES(CArmorMan) {
	slArmorManReload,
	slArmorManShield,
};
IMPLEMENT_CUSTOM_SCHEDULES(CArmorMan, CBaseMonster);

TYPEDESCRIPTION CArmorMan::m_SaveData[] =
	{
		DEFINE_FIELD(CArmorMan, m_bLastCheckAttackResult, FIELD_BOOLEAN),
		DEFINE_FIELD(CArmorMan, m_flNextCheckAttackTime, FIELD_TIME),

		DEFINE_FIELD(CArmorMan, m_flNextShieldTime, FIELD_TIME),
};
IMPLEMENT_SAVERESTORE(CArmorMan, CBaseMonster);

LINK_ENTITY_TO_CLASS(monster_armorman, CArmorMan);

const char* CArmorMan::pAlertSounds[] =
	{
		"armorman/am_alert1.wav",
		"armorman/am_alert2.wav",
		"armorman/am_alert3.wav"};

const char* CArmorMan::pDeathSounds[] =
	{
		"armorman/am_die1.wav",
		"armorman/am_die2.wav"};

const char* CArmorMan::pIdleSounds[] =
	{
		"armorman/am_idle1.wav",
		"armorman/am_idle2.wav",
		"armorman/am_idle3.wav"};

const char* CArmorMan::pPainSounds[] =
	{
		"armorman/am_pain1.wav",
		"armorman/am_pain2.wav"};

const char* CArmorMan::pStepSounds[] =
	{
		"armorman/am_step1.wav",
		"armorman/am_step2.wav",
		"armorman/am_step3.wav",
		"armorman/am_step4.wav"};



void CArmorMan::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/armorman.mdl");

	UTIL_SetSize(pev, Vector(-24.0f, -24.0f, 0.0f), Vector(24.0f, 24.0f, 108.0f));
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;
	pev->health = gSkillData.armormanHealth;

	pev->view_ofs = Vector(0.0f, -10.0f, 110.0f);
	m_flFieldOfView = VIEW_FIELD_WIDE;
	m_MonsterState = MONSTERSTATE_NONE;
	m_afCapability = bits_CAP_HEAR | bits_CAP_DOORS_GROUP | bits_CAP_TURN_HEAD | bits_CAP_RANGE_ATTACK1;

	m_cAmmoLoaded = 2;

	MonsterInit();
}

void CArmorMan::Precache()
{
	PRECACHE_MODEL("models/armorman.mdl");

	PRECACHE_SOUND("weapons/dbarrel1.wav");
	PRECACHE_SOUND("weapons/sbarrel1.wav");

	PRECACHE_SOUND_ARRAY(pAlertSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pStepSounds);
}

int CArmorMan::Classify()
{
	return CLASS_HUMAN_MILITARY;
}



void CArmorMan::SetYawSpeed()
{
	pev->yaw_speed = 200.0f;
}



bool CArmorMan::CheckRangeAttack1(float flDot, float flDist)
{
	if (flDist > 1024.0f || flDot < 0.5f)
		return false;

	if (gpGlobals->time <= m_flNextCheckAttackTime)
		return m_bLastCheckAttackResult;

	const Vector vecShootOrigin = pev->origin + ARMORMAN_SHOTGUN_OFFSET;
	CBaseEntity* pEnemy = m_hEnemy;
	const Vector vecShootTarget = pEnemy->BodyTarget(vecShootOrigin) - pEnemy->pev->origin + m_vecEnemyLKP;

	TraceResult tr;
	UTIL_TraceLine(vecShootOrigin, vecShootTarget, dont_ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction == 1.0f || tr.pHit && Instance(tr.pHit) == pEnemy)
		m_bLastCheckAttackResult = true;
	else
		m_bLastCheckAttackResult = false;

	m_flNextCheckAttackTime = gpGlobals->time + 1.5f;
	return m_bLastCheckAttackResult;
}



void CArmorMan::CheckAmmo()
{
	if (m_cAmmoLoaded <= 0)
		SetConditions(bits_COND_NO_AMMO_LOADED);
}

void CArmorMan::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
	case ARMORMAN_AE_SHOOT:
	{
		UTIL_MakeVectors(pev->angles);
		const Vector vecShootOrigin = pev->origin + ARMORMAN_SHOTGUN_OFFSET;
		const Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

		const Vector vecAngDir = UTIL_VecToAngles(vecShootDir);
		SetBlending(0, vecAngDir.x);
		pev->effects = EF_MUZZLEFLASH;

		if (m_cAmmoLoaded >= 2 && m_hEnemy && (m_hEnemy->pev->origin - pev->origin).Length() <= 256.0f)
		{
			FireBullets(12, vecShootOrigin, vecShootDir, VECTOR_CONE_15DEGREES, 1024.0f, BULLET_PLAYER_BUCKSHOT);
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/dbarrel1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			m_cAmmoLoaded -= 2;
		}
		else
		{
			FireBullets(6, vecShootOrigin, vecShootDir, VECTOR_CONE_10DEGREES, 1024.0f, BULLET_PLAYER_BUCKSHOT);
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/sbarrel1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			m_cAmmoLoaded--;
		}
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3f);
	}
	break;
	case ARMORMAN_AE_RELOAD:
		m_cAmmoLoaded += 2;
		ClearConditions(bits_COND_NO_AMMO_LOADED);
		break;
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
	}
}



Schedule_t* CArmorMan::GetSchedule()
{
	if (m_MonsterState != MONSTERSTATE_COMBAT)
		return CBaseMonster::GetSchedule();

	if (HasConditions(bits_COND_NO_AMMO_LOADED))
		return GetScheduleOfType(SCHED_RELOAD);

	if (m_hEnemy && pev->health <= (gSkillData.armormanHealth / 2) && gpGlobals->time > m_flNextShieldTime)
	{
		m_flNextShieldTime = gpGlobals->time + 30.0f;
		return GetScheduleOfType(SCHED_COWER);
	}

	return CBaseMonster::GetSchedule();
}

Schedule_t* CArmorMan::GetScheduleOfType(int Type)
{
	switch (Type)
	{
	case SCHED_RELOAD:
		return slArmorManReload;
	case SCHED_COWER:
		return slArmorManShield;
	default:
		return CBaseMonster::GetScheduleOfType(Type);
	}
}

void CArmorMan::RunTask(Task_t* pTask)
{
	if (pTask->iTask == TASK_RELOAD)
	{
		m_IdealActivity = ACT_RELOAD;
		return;
	}

	CBaseMonster::RunTask(pTask);
}



bool CArmorMan::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	// HACK HACK -- until we fix this.
	if (IsAlive())
		PainSound();

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

void CArmorMan::TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType)
{
	if (ptr->iHitgroup == 1)
	{
		if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB))
		{
			UTIL_Ricochet(ptr->vecEndPos, 1.0f);
			flDamage = 0.01f;
		}

		ptr->iHitgroup = HITGROUP_LEFTARM;
	}

	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}



void CArmorMan::AlertSound()
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pAlertSounds[RANDOM_LONG(0, ARRAYSIZE(pAlertSounds) - 1)], 1.0f, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}

void CArmorMan::DeathSound()
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pDeathSounds[RANDOM_LONG(0, ARRAYSIZE(pDeathSounds) - 1)], 1.0f, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}

void CArmorMan::IdleSound()
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pIdleSounds[RANDOM_LONG(0, ARRAYSIZE(pIdleSounds) - 1)], 1.0f, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}

void CArmorMan::PainSound()
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pPainSounds[RANDOM_LONG(0, ARRAYSIZE(pPainSounds) - 1)], 1.0f, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}
