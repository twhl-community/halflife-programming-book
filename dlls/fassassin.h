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

#pragma once

constexpr int FASSASSIN_AE_SHOOT = 1;
constexpr int FASSASSIN_AE_SHOOT_HEALING_DART = 2;

class CFAssassin : public CTalkMonster
{
public:
	void Spawn() override;
	void Precache() override;

	int Classify() override;
	int ISoundMask() override;
	int ObjectCaps() override;
	void SetYawSpeed() override;

	bool CheckRangeAttack1(float flDot, float flDist) override;

	void HandleAnimEvent(MonsterEvent_t* pEvent) override;

	void DeclineFollowing() override;
	Schedule_t* GetSchedule() override;
	Schedule_t* GetScheduleOfType(int Type) override;

	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void Killed(entvars_t* pevAttacker, int iGib) override;

	void AlertSound() override;
	void DeathSound() override;
	void PainSound() override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	CUSTOM_SCHEDULES;
private:
	CBaseEntity* CheckHealTarget();

	float m_flDiviation;
	float m_flLastShot;

	EHANDLE m_pHealTarget;
	float m_flLastHealTime;

	int m_iShell;

	float m_flPainTime;
};



class CHealingDart : public CBaseEntity
{
public:
	static CHealingDart* DartCreate();

	void Spawn() override;
	void Precache() override;

	int Classify() override;

	void EXPORT DartTouch(CBaseEntity* pOther);
	void EXPORT BubbleThink();

private:
	int m_iTrail;
};
