#pragma once

#include <string>

#include "ItemBase.h"
#include "Correction.h"

enum class WeaponType {
	Normal,
	Fly
};

enum class WeaponMode {
	Circle,
	Line,
	OnlyEnemy,
	OnlyAlly,
};

struct Weapon :ItemBase {
	// effect
	bool bAnimationAtTarget = false;
	size_t AnimationEffectID;

	size_t AnimationUnitID;
	std::wstring soundEffectFileName;

	size_t AnimationLaunchID;
	std::wstring launchSoundEffectFileName;

	// param
	WeaponType weaponType;
	WeaponMode weaponMode;

	size_t attackRange;
	size_t attackInterval;

	// correction
	Correction correction;

	// attribute
	bool bHeal = false;
	bool bMagic = false;

	bool bAOE = false;
	int AOEType;

	bool bRepel = false;
	int repelRange;

	bool bRemoteInfiltration = false;
	bool bRemoteWithMelee = false;

	bool bAttachState = false;
	size_t attachStateID;
	double attachStatePossibility;

	Weapon() = default;
	Weapon(IniInterface* pIni) :ItemBase(pIni) {

	}
};