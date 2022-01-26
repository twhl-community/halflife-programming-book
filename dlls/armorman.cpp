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

#include "armorman.h"



LINK_ENTITY_TO_CLASS(monster_armorman, CArmorMan);



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

	MonsterInit();
}

void CArmorMan::Precache()
{
	PRECACHE_MODEL("models/armorman.mdl");
}

int CArmorMan::Classify()
{
	return CLASS_HUMAN_MILITARY;
}



void CArmorMan::SetYawSpeed()
{
	pev->yaw_speed = 200.0f;
}
