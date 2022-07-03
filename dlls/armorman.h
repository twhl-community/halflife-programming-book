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

#pragma once

constexpr int ARMORMAN_AE_SHOOT = 3;
constexpr int ARMORMAN_AE_RELOAD = 5;

#define ARMORMAN_SHOTGUN_OFFSET Vector(0.0f, 0.0f, 55.0f)

class CArmorMan : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;

	int Classify() override;

	void SetYawSpeed() override;

	bool CheckRangeAttack1(float flDot, float flDist) override;

	void CheckAmmo() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;

	Schedule_t* GetSchedule() override;
	Schedule_t* GetScheduleOfType(int Type) override;
	void RunTask(Task_t* pTask) override;

	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	CUSTOM_SCHEDULES;
private:
	bool m_bLastCheckAttackResult;
	float m_flNextCheckAttackTime;

	float m_flNextShieldTime;
};
