//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __OTSERV_VOCATION_H__
#define __OTSERV_VOCATION_H__

#include "definitions.h"
#include "enums.h"
#include "const.h"
#include <string>
#include <map>

class Vocation
{
public:
	~Vocation();

	const std::string& getName() const {return name;}
	const std::string& getDescription() const {return description;}

	uint64_t getReqSkillTries(int32_t skill, int32_t level);
	uint64_t getReqMana(int32_t magLevel);
	uint32_t getHPGain() const {return gainHP;};
	uint32_t getManaGain() const {return gainMana;};
	uint32_t getCapGain() const {return gainCap;};
	uint32_t getManaGainTicks() const {return gainManaTicks;};
	uint32_t getManaGainAmount() const {return gainManaAmount;};
	uint32_t getHealthGainTicks() const {return gainHealthTicks;};
	uint32_t getHealthGainAmount() const {return gainHealthAmount;};
	uint16_t getSoulMax() const {return maxSoul;};
	uint16_t getSoulGainTicks() const {return gainSoulTicks;};
	uint32_t getAttackSpeed() const {return attackSpeed;};

	float getMeleeBaseDamage(WeaponType_t weaponType) const
	{
		if(weaponType == WEAPON_SWORD)
			return swordBaseDamage;
		else if(weaponType == WEAPON_AXE)
			return axeBaseDamage;
		else if(weaponType == WEAPON_CLUB)
			return clubBaseDamage;
		else if(weaponType == WEAPON_DIST)
			return distBaseDamage;
		else
			return fistBaseDamage;
	};

	float getMagicBaseDamage() const {return magicBaseDamage;};
	float getWandBaseDamage() const {return wandBaseDamage;};
	float getHealingBaseDamage() const {return healingBaseDamage;};
	float getBaseDefense() const {return baseDefense;};
	float getArmorDefense() const {return armorDefense;};

	void debugVocation();

protected:
	friend class Vocations;
	Vocation();

	std::string name;
	std::string description;

	uint32_t gainHealthTicks;
	uint32_t gainHealthAmount;
	uint32_t gainManaTicks;
	uint32_t gainManaAmount;
	uint32_t gainCap;
	uint32_t gainMana;
	uint32_t gainHP;
	uint16_t maxSoul;
	uint16_t gainSoulTicks;

	uint32_t skillBases[SKILL_LAST + 1];
	float skillMultipliers[SKILL_LAST + 1];
	float manaMultiplier;
	uint32_t attackSpeed;

	float swordBaseDamage;
	float axeBaseDamage;
	float clubBaseDamage;
	float distBaseDamage;
	float fistBaseDamage;
	float magicBaseDamage;
	float wandBaseDamage;
	float healingBaseDamage;
	float baseDefense;
	float armorDefense;

	typedef std::map<uint32_t, uint64_t> manaCacheMap;
	manaCacheMap cacheMana;

	typedef std::map<uint32_t, uint32_t> skillCacheMap;
	skillCacheMap cacheSkill[SKILL_LAST + 1];
};


class Vocations
{
public:
	Vocations();
	~Vocations();

	bool loadFromXml(const std::string& datadir);
	bool getVocation(const uint32_t& vocationId, Vocation*& vocation);
	bool getVocationId(const std::string& name, int32_t& vocationId) const;

private:
	typedef std::map<uint32_t, Vocation*> VocationsMap;
	VocationsMap vocationsMap;
};

#endif
