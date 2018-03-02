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

#ifndef __OTSERV_MONSTERS_H__
#define __OTSERV_MONSTERS_H__

#include "definitions.h"
#include "creature.h"
#include <string>

#define MAX_LOOTCHANCE 100000
#define MAX_STATICWALK 100

struct LootBlock{
	uint16_t id;
	uint32_t countmax;
	uint32_t chance;

	//optional
	int subType;
	int actionId;
	std::string text;

	typedef std::list<LootBlock> LootItems;
	LootItems childLoot;
	bool dropEmpty;

	LootBlock(){
		id = 0;
		countmax = 0;
		chance = 0;

		subType = -1;
		actionId = -1;
		text = "";
		dropEmpty = false;
	}
};

struct summonBlock_t{
	std::string name;
	uint32_t chance;
	uint32_t speed;
	uint32_t max;
};

class BaseSpell;

struct spellBlock_t{
	BaseSpell* spell;
	uint32_t chance;
	uint32_t speed;
	uint32_t range;
	int32_t minCombatValue;
	int32_t maxCombatValue;
	bool combatSpell;
	bool isMelee;
};

struct voiceBlock_t{
	std::string text;
	bool yellText;
};

typedef std::list<LootBlock> LootItems;
typedef std::list<summonBlock_t> SummonList;
typedef std::list<spellBlock_t> SpellList;
typedef std::vector<voiceBlock_t> VoiceVector;
typedef std::list<std::string> MonsterScriptList;
typedef std::map<CombatType_t, int32_t> ElementMap;

class MonsterType{
public:
	MonsterType();
	~MonsterType();

	void reset();

	std::string name;
	std::string nameDescription;
	uint64_t experience;

	int defense;
	int armor;

	bool canPushItems;
	bool canPushCreatures;
	uint32_t staticAttackChance;
	int maxSummons;
	int targetDistance;
	int runAwayHealth;
	bool pushable;
	int base_speed;
	int health;
	int health_max;

	Outfit_t outfit;
	uint16_t lookCorpse;
	int conditionImmunities;
	int damageImmunities;
	RaceType_t race;
	bool isSummonable;
	bool isIllusionable;
	bool isConvinceable;
	bool isAttackable;
	bool isHostile;
	bool isLureable;

	ParametersMap m_parameters;
	ParametersMap m_spellParameters;

	int lightLevel;
	int lightColor;

	uint32_t manaCost;
	SummonList summonList;
	LootItems lootItems;
	ElementMap elementMap;
	SpellList spellAttackList;
	SpellList spellDefenseList;

	uint32_t yellChance;
	uint32_t yellSpeedTicks;
	VoiceVector voiceVector;

	int32_t changeTargetSpeed;
	int32_t changeTargetChance;
	
	int32_t targetStrategiesNearestPercent;
	int32_t targetStrategiesLowerHPPercent;
	int32_t targetStrategiesMostDamagePercent;
	int32_t targetStrategiesRandom;	

	MonsterScriptList scriptList;

	void createLoot(Container* corpse);
	void createLootContainer(Container* parent, const LootBlock& lootblock);
	std::list<Item*> createLootItem(const LootBlock& lootblock);

	bool getParameter(const std::string key, std::string& value);
};

class Monsters{
public:
	Monsters();
	~Monsters();

	bool loadFromXml(const std::string& _datadir, bool reloading = false);
	bool isLoaded(){return loaded;}
	bool reload();

	MonsterType* getMonsterType(const std::string& name);
	MonsterType* getMonsterType(uint32_t mid);
	uint32_t getIdByName(const std::string& name);

	static uint32_t getLootRandom();

	void pushSpellParameters(const std::string name, LuaScriptInterface* env);

private:
	ConditionDamage* getDamageCondition(ConditionType_t conditionType,
		int32_t maxDamage, int32_t minDamage, int32_t startDamage, uint32_t tickInterval);
	bool deserializeSpell(xmlNodePtr node, spellBlock_t& sb, MonsterType* mType, const std::string& description = "");
	void deserializeParameters(xmlNodePtr node, MonsterType* mType, bool fromSpell = false);

	bool loadMonster(const std::string& file, const std::string& monster_name, bool reloading = false);

	bool loadLootContainer(xmlNodePtr, LootBlock&);
	bool loadLootItem(xmlNodePtr, LootBlock&);

	typedef std::map<std::string, uint32_t> MonsterNameMap;
	MonsterNameMap monsterNames;

	typedef std::map<uint32_t, MonsterType*> MonsterMap;
	MonsterMap monsters;

	bool loaded;
	std::string datadir;

};

#endif
