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
#include "otpch.h"

#include "monsters.h"
#include "monster.h"
#include "container.h"
#include "tools.h"
#include "spells.h"
#include "combat.h"
#include "luascript.h"
#include "weapons.h"
#include "configmanager.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

extern Spells* g_spells;
extern Monsters g_monsters;
extern ConfigManager g_config;

MonsterType::MonsterType()
{
	reset();
}

void MonsterType::reset()
{
	experience = 0;

	defense = 0;
	armor = 0;

	canPushItems = false;
	canPushCreatures = false;
	staticAttackChance = 95;
	maxSummons = 0;
	targetDistance = 1;
	runAwayHealth = 0;
	pushable = true;
	base_speed = 200;
	health = 100;
	health_max = 100;

	outfit.lookHead   = 0;
	outfit.lookBody   = 0;
	outfit.lookLegs   = 0;
	outfit.lookFeet   = 0;
	outfit.lookType   = 0;
	outfit.lookTypeEx = 0;

	lookCorpse = 0;

	conditionImmunities = 0;
	damageImmunities = 0;
	race = RACE_BLOOD;
	isSummonable = false;
	isIllusionable = false;
	isConvinceable = false;
	isAttackable = true;
	isHostile = true;
	isLureable = false;

	lightLevel = 0;
	lightColor = 0;

	manaCost = 0;
	summonList.clear();
	lootItems.clear();
	elementMap.clear();
	m_parameters.clear();
	m_spellParameters.clear();

	for(SpellList::iterator it = spellAttackList.begin(); it != spellAttackList.end(); ++it){
		if(it->combatSpell){
			delete it->spell;
			it->spell = NULL;
		}
	}

	spellAttackList.clear();

	for(SpellList::iterator it = spellDefenseList.begin(); it != spellDefenseList.end(); ++it){
		if(it->combatSpell){
			delete it->spell;
			it->spell = NULL;
		}
	}

	spellDefenseList.clear();

	yellSpeedTicks = 0;
	yellChance = 0;
	voiceVector.clear();

	changeTargetSpeed = 0;
	changeTargetChance = 0;
	
	targetStrategiesNearestPercent = 100;
	targetStrategiesLowerHPPercent = 0;
	targetStrategiesMostDamagePercent = 0;
	targetStrategiesRandom = 0;	

	scriptList.clear();
}

MonsterType::~MonsterType()
{
	reset();
}

uint32_t Monsters::getLootRandom()
{
	return random_range(0, MAX_LOOTCHANCE)/g_config.getNumber(ConfigManager::RATE_LOOT);
}

void Monsters::pushSpellParameters(const std::string name, LuaScriptInterface* env)
{
	lua_State* L = env->getLuaState();

	MonsterType* mType = getMonsterType(name);
	if(!mType || mType->m_spellParameters.empty()){
		lua_pushnil(L);
		return;
	}

	lua_newtable(L);
	ParametersMap::const_iterator it = mType->m_spellParameters.begin();
	while(it != mType->m_spellParameters.end()){
		env->setField(L, it->first.c_str(), it->second);
		++it;
	}
}

void MonsterType::createLoot(Container* corpse)
{
	for(LootItems::const_iterator it = lootItems.begin(); it != lootItems.end() && (corpse->capacity() - corpse->size() > 0); ++it){
		std::list<Item*> itemList = createLootItem(*it);
		if(!itemList.empty()){
			for(std::list<Item*>::iterator iit = itemList.begin(); iit != itemList.end(); ++iit){
				Item* tmpItem = *iit;

				//check containers
				if(Container* container = tmpItem->getContainer()){
					createLootContainer(container, *it);
					if(container->size() == 0 && !it->dropEmpty){
						delete container;
					}
					else{
						corpse->__internalAddThing(tmpItem);
					}
				}
				else{
					corpse->__internalAddThing(tmpItem);
				}
			}
		}
	}

	corpse->__startDecaying();
}

std::list<Item*> MonsterType::createLootItem(const LootBlock& lootBlock)
{
	Item* tmpItem = NULL;
	int32_t itemCount = 0;

	uint32_t randvalue = Monsters::getLootRandom();
	if(randvalue < lootBlock.chance){
		if(Item::items[lootBlock.id].stackable){
			itemCount = randvalue % lootBlock.countmax + 1;
		}
		else{
			itemCount = 1;
		}
	}

	std::list<Item*> itemList;


	
	while(itemCount > 0){
		uint16_t n = (uint16_t)std::min(itemCount, (int32_t)100);
		itemCount -= n;
		
		if((tmpItem = Item::CreateItem(lootBlock.id, n))){
			if(lootBlock.subType != -1){
				tmpItem->setSubType(lootBlock.subType);
			}

			if(lootBlock.actionId != -1){
				tmpItem->setActionId(lootBlock.actionId);
			}

			if(lootBlock.text != ""){
				tmpItem->setText(lootBlock.text);
			}

			itemList.push_back(tmpItem);
		}
	}

	return itemList;
}

void MonsterType::createLootContainer(Container* parent, const LootBlock& lootblock)
{
	if(parent->size() < parent->capacity()){
		LootItems::const_iterator it;
		for(it = lootblock.childLoot.begin(); it != lootblock.childLoot.end(); ++it){
			std::list<Item*> itemList = createLootItem(*it);
			if(!itemList.empty()){
				for(std::list<Item*>::iterator iit = itemList.begin(); iit != itemList.end(); ++iit){
					Item* tmpItem = *iit;
					if(Container* container = tmpItem->getContainer()){
						createLootContainer(container, *it);
						if(container->size() == 0 && !it->dropEmpty){
							delete container;
						}
						else{
							parent->__internalAddThing(container);
						}
					}
					else{
						parent->__internalAddThing(tmpItem);
					}
				}
			}
		}
	}
}

bool MonsterType::getParameter(const std::string key, std::string& value)
{
	ParametersMap::const_iterator it = m_parameters.find(key);
	if(it != m_parameters.end()){
		value = it->second;
		return true;
	}

	return false;
}

Monsters::Monsters()
{
	loaded = false;
}

bool Monsters::loadFromXml(const std::string& _datadir, bool reloading /*= false*/)
{
	loaded = false;
	datadir = _datadir;

	std::string filename = datadir + "monster/monsters.xml";

	xmlDocPtr doc = xmlParseFile(filename.c_str());
	if(doc){
		loaded = true;
		xmlNodePtr root, p;
		root = xmlDocGetRootElement(doc);

		if(xmlStrcmp(root->name,(const xmlChar*)"monsters") != 0){
			xmlFreeDoc(doc);
			loaded = false;
			return false;
		}

		p = root->children;
		while(p){
			if(p->type != XML_ELEMENT_NODE){
				p = p->next;
				continue;
			}

			if(xmlStrcmp(p->name, (const xmlChar*)"monster") == 0){
				std::string file;
				std::string name;

				if(readXMLString(p, "file", file) && readXMLString(p, "name", name)){
					file = datadir + "monster/" + file;
					loadMonster(file, name, reloading);
				}
			}
			else{
				std::cout << "Warning: [Monsters::loadFromXml]. Unknown node name. " << p->name << std::endl;
			}
			p = p->next;
		}

		xmlFreeDoc(doc);
	}

	return loaded;
}

bool Monsters::reload()
{
	return loadFromXml(datadir, true);
}

ConditionDamage* Monsters::getDamageCondition(ConditionType_t conditionType,
	int32_t maxDamage, int32_t minDamage, int32_t startDamage, uint32_t tickInterval)
{
	ConditionDamage* condition = dynamic_cast<ConditionDamage*>(Condition::createCondition(CONDITIONID_COMBAT, conditionType, 0, 0));
	condition->setParam(CONDITIONPARAM_TICKINTERVAL, tickInterval);
	condition->setParam(CONDITIONPARAM_MINVALUE, minDamage);
	condition->setParam(CONDITIONPARAM_MAXVALUE, maxDamage);
	condition->setParam(CONDITIONPARAM_STARTVALUE, startDamage);
	condition->setParam(CONDITIONPARAM_DELAYED, 1);

	return condition;
}

bool Monsters::deserializeSpell(xmlNodePtr node, spellBlock_t& sb, MonsterType* mType, const std::string& description /*= ""*/)
{
	sb.chance = 100;
	sb.speed = 2000;
	sb.range = 0;
	sb.minCombatValue = 0;
	sb.maxCombatValue = 0;
	sb.combatSpell = false;
	sb.isMelee = false;

	std::string name = "";
	std::string scriptName = "";
	bool isScripted = false;

	if(readXMLString(node, "script", scriptName)){
		isScripted = true;
	}
	else if(!readXMLString(node, "name", name)){
		return false;
	}

	int intValue;
	std::string strValue;
	if(readXMLInteger(node, "speed", intValue) || readXMLInteger(node, "interval", intValue)){
		sb.speed = std::max(1, intValue);
	}

	if(readXMLInteger(node, "chance", intValue)){
		if(intValue < 0 || intValue > 100){
			intValue = 100;
		}

		sb.chance = intValue;
	}

	if(readXMLInteger(node, "range", intValue)){
		if(intValue < 0 ){
			intValue = 0;
		}

		if(intValue > Map::maxViewportX * 2){
			intValue = Map::maxViewportX * 2;
		}

		sb.range = intValue;
	}

	if(readXMLInteger(node, "min", intValue)){
		sb.minCombatValue = intValue;
	}

	if(readXMLInteger(node, "max", intValue)){
		sb.maxCombatValue = intValue;

		//normalize values
		if(std::abs(sb.minCombatValue) > std::abs(sb.maxCombatValue)){
			int32_t value = sb.maxCombatValue;
			sb.maxCombatValue = sb.minCombatValue;
			sb.minCombatValue = value;
		}
	}

	if((sb.spell = g_spells->getSpellByName(name))){
		deserializeParameters(node->children, mType, true);
		return true;
	}

	CombatSpell* combatSpell = NULL;
	bool needTarget = false;
	bool needDirection = false;

	if(isScripted){
		if(readXMLInteger(node, "direction", intValue)){
			needDirection = (intValue == 1);
		}

		if(readXMLInteger(node, "target", intValue)){
			needTarget = (intValue != 0);
		}

		combatSpell = new CombatSpell(NULL, needTarget, needDirection);

		std::string datadir = g_config.getString(ConfigManager::DATA_DIRECTORY);
		if(!combatSpell->loadScript(datadir + g_spells->getScriptBaseName() + "/scripts/" + scriptName)){
			return false;
		}

		if(!combatSpell->loadScriptCombat()){
			return false;
		}

		deserializeParameters(node->children, mType, true);
		combatSpell->getCombat()->setPlayerCombatValues(FORMULA_VALUE, sb.minCombatValue, 0, sb.maxCombatValue, 0);
	}
	else{
		Combat* combat = new Combat;
		sb.combatSpell = true;

		if(readXMLInteger(node, "length", intValue)){
			int32_t length = intValue;

			if(length > 0){
				int32_t spread = 3;

				//need direction spell
				if(readXMLInteger(node, "spread", intValue)){
					spread = std::max(0, intValue);
				}

				AreaCombat* area = new AreaCombat();
				area->setupArea(length, spread);
				combat->setArea(area);

				needDirection = true;
			}
		}
		
		if(readXMLInteger(node, "checkshield", intValue)){
			combat->setParam(COMBATPARAM_BLOCKEDBYSHIELD, 1);
		}

		if(readXMLInteger(node, "radius", intValue)){
			int32_t radius = intValue;

			//target spell
			if(readXMLInteger(node, "target", intValue)){
				needTarget = (intValue != 0);
			}

			AreaCombat* area = new AreaCombat();
			area->setupArea(radius);
			combat->setArea(area);
		}

		if(asLowerCaseString(name) == "melee"){
			int attack = 0;
			int skill = 0;
			sb.isMelee = true;
			if(readXMLInteger(node, "attack", attack)){
				if(readXMLInteger(node, "skill", skill)){
					sb.minCombatValue = 0;
					sb.maxCombatValue = -Weapons::getMaxMeleeDamage(skill, attack);
				}
			}

			ConditionType_t conditionType = CONDITION_NONE;
			int32_t minDamage = 0;
			int32_t maxDamage = 0;
			int32_t startDamage = 0;
			uint32_t tickInterval = 2000;

			if(readXMLInteger(node, "fire", intValue)){
				conditionType = CONDITION_FIRE;

				minDamage = intValue;
				maxDamage = intValue;
				tickInterval = 10000;
			}
			else if(readXMLInteger(node, "poison", intValue)){
				conditionType = CONDITION_POISON;

				minDamage = intValue;
				maxDamage = intValue;
				tickInterval = 5000;
			}
			else if(readXMLInteger(node, "energy", intValue)){
				conditionType = CONDITION_ENERGY;

				minDamage = intValue;
				maxDamage = intValue;
				tickInterval = 10000;
			}
			else if(readXMLInteger(node, "earth", intValue)){
				conditionType = CONDITION_POISON;

				minDamage = intValue;
				maxDamage = intValue;
				tickInterval = 5000;
			}			

			if(readXMLInteger(node, "tick", intValue) && intValue > 0){
				tickInterval = intValue;
			}

			if(conditionType != CONDITION_NONE){
				Condition* condition = getDamageCondition(conditionType, maxDamage, minDamage, startDamage, tickInterval);
				combat->setCondition(condition);
			}

			sb.range = 1;
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_PHYSICALDAMAGE);
			combat->setParam(COMBATPARAM_BLOCKEDBYARMOR, 1);
			combat->setParam(COMBATPARAM_BLOCKEDBYSHIELD, 1);
		}
		else if(asLowerCaseString(name) == "physical"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_PHYSICALDAMAGE);
			combat->setParam(COMBATPARAM_BLOCKEDBYARMOR, 1);
		}
		else if(asLowerCaseString(name) == "poison" || asLowerCaseString(name) == "earth"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_EARTHDAMAGE);
		}
		else if(asLowerCaseString(name) == "fire"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_FIREDAMAGE);
		}
		else if(asLowerCaseString(name) == "energy"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_ENERGYDAMAGE);
		}
		else if(asLowerCaseString(name) == "lifedrain"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_LIFEDRAIN);
		}
		else if(asLowerCaseString(name) == "manadrain"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_MANADRAIN);
		}
		else if(asLowerCaseString(name) == "healing"){
			combat->setParam(COMBATPARAM_COMBATTYPE, COMBAT_HEALING);
			combat->setParam(COMBATPARAM_AGGRESSIVE, 0);
		}
		else if(asLowerCaseString(name) == "speed"){
			int32_t speedChange = 0;
			int32_t duration = 10000;

			if(readXMLInteger(node, "duration", intValue)){
				duration = intValue;
			}

			if(readXMLInteger(node, "speedchange", intValue)){
				speedChange = intValue;

				if(speedChange < -1000){
					//cant be slower than 100%
					speedChange = -1000;
				}
			}

			ConditionType_t conditionType;
			if(speedChange > 0){
				conditionType = CONDITION_HASTE;
				combat->setParam(COMBATPARAM_AGGRESSIVE, 0);
			}
			else{
				conditionType = CONDITION_PARALYZE;
			}

			ConditionSpeed* condition = dynamic_cast<ConditionSpeed*>(Condition::createCondition(CONDITIONID_COMBAT, conditionType, duration, 0));
			condition->setFormulaVars(speedChange / 1000.0, 0, speedChange / 1000.0, 0);
			combat->setCondition(condition);
		}
		else if(asLowerCaseString(name) == "outfit"){
			int32_t duration = 10000;

			if(readXMLInteger(node, "duration", intValue)){
				duration = intValue;
			}

			if(readXMLString(node, "monster", strValue)){
				MonsterType* mType = g_monsters.getMonsterType(strValue);
				if(mType){
					ConditionOutfit* condition = dynamic_cast<ConditionOutfit*>(Condition::createCondition(CONDITIONID_COMBAT, CONDITION_OUTFIT, duration, 0));
					condition->addOutfit(mType->outfit);
					combat->setParam(COMBATPARAM_AGGRESSIVE, 0);
					combat->setCondition(condition);
				}
			}
			else if(readXMLInteger(node, "item", intValue)){
				Outfit_t outfit;
				outfit.lookTypeEx = intValue;

				ConditionOutfit* condition = dynamic_cast<ConditionOutfit*>(Condition::createCondition(CONDITIONID_COMBAT, CONDITION_OUTFIT, duration, 0));
				condition->addOutfit(outfit);
				combat->setParam(COMBATPARAM_AGGRESSIVE, 0);
				combat->setCondition(condition);
			}
		}
		else if(asLowerCaseString(name) == "invisible" || asLowerCaseString(name) == "invisibility"){
			int32_t duration = 10000;

			if(readXMLInteger(node, "duration", intValue)){
				duration = intValue;
			}

			Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_INVISIBLE, duration, 0);
			combat->setParam(COMBATPARAM_AGGRESSIVE, 0);
			combat->setCondition(condition);
		}
		else if(asLowerCaseString(name) == "drunk"){
			int32_t duration = 10000;

			if(readXMLInteger(node, "duration", intValue)){
				duration = intValue;
			}

			Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_DRUNK, duration, 0);
			combat->setCondition(condition);
		}
		else if(asLowerCaseString(name) == "skills" || asLowerCaseString(name) == "attributes"){
			uint32_t duration = 10000, subId = 0;
			if(readXMLInteger(node, "duration", intValue)){
				duration = intValue;
			}

			if(readXMLInteger(node, "subid", intValue)){
				subId = intValue;
			}

			intValue = 0;
			ConditionParam_t param = CONDITIONPARAM_BUFF_SPELL; //to know was it loaded
			if(readXMLInteger(node, "melee", intValue)){
				param = CONDITIONPARAM_SKILL_MELEE;
			}
			else if(readXMLInteger(node, "fist", intValue)){
				param = CONDITIONPARAM_SKILL_FIST;
			}
			else if(readXMLInteger(node, "club", intValue)){
				param = CONDITIONPARAM_SKILL_CLUB;
			}
			else if(readXMLInteger(node, "axe", intValue)){
				param = CONDITIONPARAM_SKILL_AXE;
			}
			else if(readXMLInteger(node, "sword", intValue)){
				param = CONDITIONPARAM_SKILL_SWORD;
			}
			else if(readXMLInteger(node, "distance", intValue) || readXMLInteger(node, "dist", intValue)){
				param = CONDITIONPARAM_SKILL_DISTANCE;
			}
			else if(readXMLInteger(node, "shielding", intValue) || readXMLInteger(node, "shield", intValue)){
				param = CONDITIONPARAM_SKILL_SHIELD;
			}
			else if(readXMLInteger(node, "fishing", intValue) || readXMLInteger(node, "fish", intValue)){
				param = CONDITIONPARAM_SKILL_FISHING;
			}
			else if(readXMLInteger(node, "meleePercent", intValue)){
				param = CONDITIONPARAM_SKILL_MELEEPERCENT;
			}
			else if(readXMLInteger(node, "fistPercent", intValue)){
				param = CONDITIONPARAM_SKILL_FISTPERCENT;
			}
			else if(readXMLInteger(node, "clubPercent", intValue)){
				param = CONDITIONPARAM_SKILL_CLUBPERCENT;
			}
			else if(readXMLInteger(node, "axePercent", intValue)){
				param = CONDITIONPARAM_SKILL_AXEPERCENT;
			}
			else if(readXMLInteger(node, "swordPercent", intValue)){
				param = CONDITIONPARAM_SKILL_SWORDPERCENT;
			}
			else if(readXMLInteger(node, "distancePercent", intValue) || readXMLInteger(node, "distPercent", intValue)){
				param = CONDITIONPARAM_SKILL_DISTANCEPERCENT;
			}
			else if(readXMLInteger(node, "shieldingPercent", intValue) || readXMLInteger(node, "shieldPercent", intValue)){
				param = CONDITIONPARAM_SKILL_SHIELDPERCENT;
			}
			else if(readXMLInteger(node, "fishingPercent", intValue) || readXMLInteger(node, "fishPercent", intValue)){
				param = CONDITIONPARAM_SKILL_FISHINGPERCENT;
			}
			else if(readXMLInteger(node, "maxhealth", intValue)){
				param = CONDITIONPARAM_STAT_MAXHITPOINTS;
			}
			else if(readXMLInteger(node, "maxmana", intValue)){
				param = CONDITIONPARAM_STAT_MAXMANAPOINTS;
			}
			else if(readXMLInteger(node, "soul", intValue)){
				param = CONDITIONPARAM_STAT_SOULPOINTS;
			}
			else if(readXMLInteger(node, "magiclevel", intValue) || readXMLInteger(node, "maglevel", intValue)){
				param = CONDITIONPARAM_STAT_MAGICPOINTS;
			}
			else if(readXMLInteger(node, "maxhealthPercent", intValue)){
				param = CONDITIONPARAM_STAT_MAXHITPOINTSPERCENT;
			}
			else if(readXMLInteger(node, "maxmanaPercent", intValue)){
				param = CONDITIONPARAM_STAT_MAXMANAPOINTSPERCENT;
			}
			else if(readXMLInteger(node, "soulPercent", intValue)){
				param = CONDITIONPARAM_STAT_SOULPOINTSPERCENT;
			}
			else if(readXMLInteger(node, "magiclevelPercent", intValue) || readXMLInteger(node, "maglevelPercent", intValue)){
				param = CONDITIONPARAM_STAT_MAGICPOINTSPERCENT;
			}
			if(param != CONDITIONPARAM_BUFF_SPELL){
				if(ConditionAttributes* condition = dynamic_cast<ConditionAttributes*>(Condition::createCondition(
					CONDITIONID_COMBAT, CONDITION_ATTRIBUTES, duration, subId))){
					condition->setParam(param, intValue);
					combat->setCondition(condition);
				}
			}
		}
		else if(asLowerCaseString(name) == "firefield"){
			combat->setParam(COMBATPARAM_CREATEITEM, ITEM_FIREFIELD);
		}
		else if(asLowerCaseString(name) == "poisonfield"){
			combat->setParam(COMBATPARAM_CREATEITEM, ITEM_POISONFIELD);
		}
		else if(asLowerCaseString(name) == "energyfield"){
			combat->setParam(COMBATPARAM_CREATEITEM, ITEM_ENERGYFIELD);
		}
		else if(asLowerCaseString(name) == "firecondition" ||
				asLowerCaseString(name) == "poisoncondition" ||
				asLowerCaseString(name) == "energycondition" ||
				asLowerCaseString(name) == "earthcondition"){
			ConditionType_t conditionType = CONDITION_NONE;
			uint32_t tickInterval = 2000;

			if(name == "firecondition"){
				conditionType = CONDITION_FIRE;
				tickInterval = 10000;
			}
			else if(name == "poisoncondition"){
				conditionType = CONDITION_POISON;
				tickInterval = 5000;
			}
			else if(name == "energycondition"){
				conditionType = CONDITION_ENERGY;
				tickInterval = 10000;
			}
			else if(name == "earthcondition")
			{
				conditionType = CONDITION_POISON;
				tickInterval = 5000;
			}
			
			if(readXMLInteger(node, "tick", intValue) && intValue > 0){
				tickInterval = intValue;
			}

			int32_t minDamage = std::abs(sb.minCombatValue);
			int32_t maxDamage = std::abs(sb.maxCombatValue);
			int32_t startDamage = 0;

			if(readXMLInteger(node, "start", intValue)){
				intValue = std::abs(intValue);

				if(intValue <= minDamage){
					startDamage = intValue;
				}
			}

			Condition* condition = getDamageCondition(conditionType, maxDamage, minDamage, startDamage, tickInterval);
			combat->setCondition(condition);
		}
		else if(asLowerCaseString(name) == "strength" || asLowerCaseString(name) == "effect"){
			//
		}
		else{
			std::cout << "Error: [Monsters::deserializeSpell] - " << description <<  " - Unknown spell name: " << name << std::endl;
			delete combat;
			return false;
		}

		combat->setPlayerCombatValues(FORMULA_VALUE, sb.minCombatValue, 0, sb.maxCombatValue, 0);
		combatSpell = new CombatSpell(combat, needTarget, needDirection);

		xmlNodePtr attributeNode = node->children;

		while(attributeNode){
			if(xmlStrcmp(attributeNode->name, (const xmlChar*)"attribute") == 0){
				if(readXMLString(attributeNode, "key", strValue)){
					if(asLowerCaseString(strValue) == "shooteffect"){
						if(readXMLString(attributeNode, "value", strValue)){
							ShootType_t shoot = getShootType(strValue);
							if(shoot != NM_SHOOT_UNK){
								combat->setParam(COMBATPARAM_DISTANCEEFFECT, shoot);
							}
							else{
								std::cout << "Warning: [Monsters::deserializeSpell] - "  << description << " - Unknown shootEffect: " << strValue << std::endl;
							}
						}
					}
					else if(asLowerCaseString(strValue) == "areaeffect"){
						if(readXMLString(attributeNode, "value", strValue)){
							MagicEffectClasses effect = getMagicEffect(strValue);
							if(effect != NM_ME_UNK){
								combat->setParam(COMBATPARAM_EFFECT, effect);
							}
							else{
								std::cout << "Warning: [Monsters::deserializeSpell] - "  << description << " - Unknown areaEffect: " << strValue << std::endl;
							}
						}
					}
				}
			}

			attributeNode = attributeNode->next;
		}
	}

	sb.spell = combatSpell;
	return true;
}

void Monsters::deserializeParameters(xmlNodePtr node, MonsterType* mType, bool fromSpell /*= false*/)
{
	if(mType){
		while(node){
			if(xmlStrcmp(node->name, (const xmlChar*)"parameter") == 0){
				std::string key;
				if(readXMLString(node, "key", key)){
					std::string value;
					if(readXMLString(node, "value", value)){
						if(fromSpell)
							mType->m_spellParameters[key] = value;
						else
							mType->m_parameters[key] = value;
					}
				}
			}

			node = node->next;
		}
	}
}

#define SHOW_XML_WARNING(desc) std::cout << "Warning: [Monsters::loadMonster]. " << desc << ". " << file << std::endl;
#define SHOW_XML_ERROR(desc) std::cout << "Error: [Monsters::loadMonster]. " << desc << ". " << file << std::endl;

bool Monsters::loadMonster(const std::string& file, const std::string& monster_name, bool reloading /*= false*/)
{
	bool monsterLoad;
	MonsterType* mType = NULL;
	bool new_mType = true;

	if(reloading){
		uint32_t id = getIdByName(monster_name);
		if(id != 0){
			mType = getMonsterType(id);
			if(mType != NULL){
				new_mType = false;
				mType->reset();
			}
		}
	}
	if(new_mType){
		mType = new MonsterType();
	}

	monsterLoad = true;
	xmlDocPtr doc = xmlParseFile(file.c_str());

	if(doc){
		xmlNodePtr root, p;
		root = xmlDocGetRootElement(doc);

		if(xmlStrcmp(root->name,(const xmlChar*)"monster") != 0){
			std::cerr << "Malformed XML: " << file << std::endl;
		}

		int intValue;
		std::string strValue;

		p = root->children;

		if(readXMLString(root, "name", strValue)){
			mType->name = strValue;
		}
		else
			monsterLoad = false;

		if(readXMLString(root, "nameDescription", strValue)){
			mType->nameDescription = strValue;
		}
		else{
			mType->nameDescription = "a " + mType->name;
			toLowerCaseString(mType->nameDescription);
		}

		if(readXMLString(root, "race", strValue)){
			if((asLowerCaseString(strValue) == "venom") || (atoi(strValue.c_str()) == 1)){
				mType->race = RACE_VENOM;
			}
			else if((asLowerCaseString(strValue) == "blood") || (atoi(strValue.c_str()) == 2)){
				mType->race = RACE_BLOOD;
			}
			else if((asLowerCaseString(strValue) == "undead") || (atoi(strValue.c_str()) == 3)){
				mType->race = RACE_UNDEAD;
			}
			else if((asLowerCaseString(strValue) == "fire") || (atoi(strValue.c_str()) == 4)){
				mType->race = RACE_FIRE;
			}
			else if((asLowerCaseString(strValue) == "energy") || (atoi(strValue.c_str()) == 5)){
				mType->race = RACE_ENERGY;
			}
			else{
				SHOW_XML_WARNING("Unknown race type " << strValue);
			}
		}

		if(readXMLInteger(root, "experience", intValue)){
			mType->experience = intValue;
		}

		if(readXMLInteger(root, "speed", intValue)){
			mType->base_speed = intValue;
		}

		if(readXMLInteger(root, "manacost", intValue)){
			mType->manaCost = intValue;
		}

		while(p){
			if(p->type != XML_ELEMENT_NODE){
				p = p->next;
				continue;
			}

			if(xmlStrcmp(p->name, (const xmlChar*)"health") == 0){

				if(readXMLInteger(p, "now", intValue)){
					mType->health = intValue;
				}
				else{
					SHOW_XML_ERROR("Missing health.now");
					monsterLoad = false;
				}

				if(readXMLInteger(p, "max", intValue)){
					mType->health_max = intValue;
				}
				else{
					SHOW_XML_ERROR("Missing health.max");
					monsterLoad = false;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"parameters") == 0){
				deserializeParameters(p->children, mType);
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"flags") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"flag") == 0){

						if(readXMLInteger(tmpNode, "summonable", intValue)){
							mType->isSummonable = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "attackable", intValue)){
							mType->isAttackable = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "hostile", intValue)){
							mType->isHostile = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "illusionable", intValue)){
							mType->isIllusionable = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "convinceable", intValue)){
							mType->isConvinceable = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "pushable", intValue)){
							mType->pushable = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "canpushitems", intValue)){
							mType->canPushItems = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "canpushcreatures", intValue)){
							mType->canPushCreatures = (intValue != 0);
						}

						if(readXMLInteger(tmpNode, "staticattack", intValue)){
							if(intValue < 0){
								SHOW_XML_WARNING("staticattack lower than 0");
								intValue = 0;
							}

							if(intValue > 100){
								SHOW_XML_WARNING("staticattack greater than 100");
								intValue = 100;
							}

							mType->staticAttackChance = intValue;
						}

						if(readXMLInteger(tmpNode, "lightlevel", intValue)){
							mType->lightLevel = intValue;
						}

						if(readXMLInteger(tmpNode, "lightcolor", intValue)){
							mType->lightColor = intValue;
						}

						if(readXMLInteger(tmpNode, "targetdistance", intValue)){
							/*if(intValue > 6){
								SHOW_XML_WARNING("targetdistance greater than 6");
							}*/
							mType->targetDistance = std::max(1, intValue);
						}

						if(readXMLInteger(tmpNode, "runonhealth", intValue)){
							mType->runAwayHealth = intValue;
						}

						if(readXMLInteger(tmpNode, "lureable", intValue)){
							mType->isLureable = (intValue != 0);
						}
					}

					tmpNode = tmpNode->next;
				}
				//if a monster can push creatures,
				// it should not be pushable
				if(mType->canPushCreatures && mType->pushable){
					mType->pushable = false;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"targetchange") == 0){

				if(readXMLInteger(p, "speed", intValue) || readXMLInteger(p, "interval", intValue)){
					mType->changeTargetSpeed = std::max(1, intValue);
				}
				else{
					SHOW_XML_WARNING("Missing targetchange.speed");
				}

				if(readXMLInteger(p, "chance", intValue)){
					mType->changeTargetChance = intValue;
				}
				else{
					SHOW_XML_WARNING("Missing targetchange.chance");
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"targetstrategies") == 0){

				if(readXMLInteger(p, "nearest", intValue)){
					mType->targetStrategiesNearestPercent = std::max(1, intValue);
				}
				else{
					SHOW_XML_WARNING("Missing targetStrategiesNearestPercent");
				}

				if(readXMLInteger(p, "health", intValue)){
					mType->targetStrategiesLowerHPPercent = std::max(1, intValue);
				}
				else{
					SHOW_XML_WARNING("Missing targetStrategiesLowerHPPercent");
				}
				
				if(readXMLInteger(p, "damage", intValue)){
					mType->targetStrategiesMostDamagePercent = std::max(1, intValue);
				}
				else{
					SHOW_XML_WARNING("Missing targetStrategiesMostDamagePercent");
				}
				
				if(readXMLInteger(p, "random", intValue)){
					mType->targetStrategiesRandom = std::max(1, intValue);
				}
				else{
					SHOW_XML_WARNING("Missing targetStrategiesRandom");
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"strategy") == 0){

				if(readXMLInteger(p, "attack", intValue)){
					//mType->attackStrength = intValue;
				}

				if(readXMLInteger(p, "defense", intValue)){
					//mType->defenseStrength = intValue;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"look") == 0){

				if(readXMLInteger(p, "type", intValue)){
					mType->outfit.lookType = intValue;

					if(readXMLInteger(p, "head", intValue)){
						mType->outfit.lookHead = intValue;
					}

					if(readXMLInteger(p, "body", intValue)){
						mType->outfit.lookBody = intValue;
					}

					if(readXMLInteger(p, "legs", intValue)){
						mType->outfit.lookLegs = intValue;
					}

					if(readXMLInteger(p, "feet", intValue)){
						mType->outfit.lookFeet = intValue;
					}
				}
				else if(readXMLInteger(p, "typeex", intValue)){
					mType->outfit.lookTypeEx = intValue;
				}
				else{
					SHOW_XML_WARNING("Missing look type/typeex");
				}

				if(readXMLInteger(p, "corpse", intValue)){
					mType->lookCorpse = (uint16_t)intValue;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"attacks") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"attack") == 0){
						spellBlock_t sb;
						if(deserializeSpell(tmpNode, sb, mType, monster_name)){
							mType->spellAttackList.push_back(sb);
						}
						else{
							SHOW_XML_WARNING("Cant load spell");
						}
					}

					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"defenses") == 0){
				if(readXMLInteger(p, "defense", intValue)){
					mType->defense = intValue;
				}

				if(readXMLInteger(p, "armor", intValue)){
					mType->armor = intValue;
				}

				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"defense") == 0){

						spellBlock_t sb;
						if(deserializeSpell(tmpNode, sb, mType, monster_name)){
							mType->spellDefenseList.push_back(sb);
						}
						else{
							SHOW_XML_WARNING("Cant load spell");
						}
					}

					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"immunities") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"immunity") == 0){

						if(readXMLString(tmpNode, "name", strValue)){

							if(asLowerCaseString(strValue) == "physical"){
								mType->damageImmunities |= COMBAT_PHYSICALDAMAGE;
								//mType->conditionImmunities |= CONDITION_PHYSICAL;
							}
							else if(asLowerCaseString(strValue) == "energy"){
								mType->damageImmunities |= COMBAT_ENERGYDAMAGE;
								mType->conditionImmunities |= CONDITION_ENERGY;
							}
							else if(asLowerCaseString(strValue) == "fire"){
								mType->damageImmunities |= COMBAT_FIREDAMAGE;
								mType->conditionImmunities |= CONDITION_FIRE;
							}
							else if(asLowerCaseString(strValue) == "poison" ||
									asLowerCaseString(strValue) == "earth"){
								mType->damageImmunities |= COMBAT_EARTHDAMAGE;
								mType->conditionImmunities |= CONDITION_POISON;
							}
							else if(asLowerCaseString(strValue) == "lifedrain"){
								mType->damageImmunities |= COMBAT_LIFEDRAIN;
								mType->conditionImmunities |= CONDITION_LIFEDRAIN;
							}
							else if(asLowerCaseString(strValue) == "manadrain"){
								mType->damageImmunities |= COMBAT_MANADRAIN;
								mType->conditionImmunities |= CONDITION_LIFEDRAIN;
							}
							else if(asLowerCaseString(strValue) == "paralyze"){
								mType->conditionImmunities |= CONDITION_PARALYZE;
							}
							else if(asLowerCaseString(strValue) == "outfit"){
								mType->conditionImmunities |= CONDITION_OUTFIT;
							}
							else if(asLowerCaseString(strValue) == "drunk"){
								mType->conditionImmunities |= CONDITION_DRUNK;
							}
							else if(asLowerCaseString(strValue) == "invisible" ||
									asLowerCaseString(strValue) == "invisibility"){
								mType->conditionImmunities |= CONDITION_INVISIBLE;
							}
							else{
								SHOW_XML_WARNING("Unknown immunity name " << strValue);
							}
						}
						//old immunities code
						else if(readXMLInteger(tmpNode, "physical", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_PHYSICALDAMAGE;
								//mType->conditionImmunities |= CONDITION_PHYSICAL;
							}
						}
						else if(readXMLInteger(tmpNode, "energy", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_ENERGYDAMAGE;
								mType->conditionImmunities |= CONDITION_ENERGY;
							}
						}
						else if(readXMLInteger(tmpNode, "fire", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_FIREDAMAGE;
								mType->conditionImmunities |= CONDITION_FIRE;
							}
						}
						else if(readXMLInteger(tmpNode, "poison", intValue) ||
								readXMLInteger(tmpNode, "earth", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_EARTHDAMAGE;
								mType->conditionImmunities |= CONDITION_POISON;
							}
						}
						else if(readXMLInteger(tmpNode, "lifedrain", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_LIFEDRAIN;
								mType->conditionImmunities |= CONDITION_LIFEDRAIN;
							}
						}
						else if(readXMLInteger(tmpNode, "manadrain", intValue)){
							if(intValue != 0){
								mType->damageImmunities |= COMBAT_MANADRAIN;
								mType->conditionImmunities |= CONDITION_LIFEDRAIN;
							}
						}						
						else if(readXMLInteger(tmpNode, "paralyze", intValue)){
							if(intValue != 0){
								mType->conditionImmunities |= CONDITION_PARALYZE;
							}
						}
						else if(readXMLInteger(tmpNode, "outfit", intValue)){
							if(intValue != 0){
								mType->conditionImmunities |= CONDITION_OUTFIT;
							}
						}
						else if(readXMLInteger(tmpNode, "drunk", intValue)){
							if(intValue != 0){
								mType->conditionImmunities |= CONDITION_DRUNK;
							}
						}
						else if(readXMLInteger(tmpNode, "invisible", intValue) ||
								readXMLInteger(tmpNode, "invisibility", intValue)){
							if(intValue != 0){
								mType->conditionImmunities |= CONDITION_INVISIBLE;
							}
						}
						else{
							SHOW_XML_WARNING("Unknown immunity being used.");
						}
					}

					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"voices") == 0){
				xmlNodePtr tmpNode = p->children;

				if(readXMLInteger(p, "speed", intValue) || readXMLInteger(p, "interval", intValue)){
					mType->yellSpeedTicks = intValue;
				}
				else{
					SHOW_XML_WARNING("Missing voices.speed");
				}

				if(readXMLInteger(p, "chance", intValue)){
					mType->yellChance = intValue;
				}
				else{
					SHOW_XML_WARNING("Missing voices.chance");
				}

				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"voice") == 0){

						voiceBlock_t vb;
						vb.text = "";
						vb.yellText = false;

						if(readXMLString(tmpNode, "sentence", strValue)){
							vb.text = strValue;
						}
						else{
							SHOW_XML_WARNING("Missing voice.sentence");
						}

						if(readXMLInteger(tmpNode, "yell", intValue)){
							vb.yellText = (intValue != 0);
						}

						mType->voiceVector.push_back(vb);
					}

					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"loot") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(tmpNode->type != XML_ELEMENT_NODE){
						tmpNode = tmpNode->next;
						continue;
					}

					LootBlock lootBlock;
					if(loadLootItem(tmpNode, lootBlock)){
						mType->lootItems.push_front(lootBlock);
					}
					else{
						SHOW_XML_WARNING("Cant load loot");
					}

					tmpNode = tmpNode->next;
				}
			}
		else if(xmlStrcmp(p->name, (const xmlChar*)"elements") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"element") == 0){
						if(readXMLInteger(tmpNode, "physicalPercent", intValue)){
							mType->elementMap[COMBAT_PHYSICALDAMAGE] = intValue;
						}
						if(readXMLInteger(tmpNode, "earthPercent", intValue)){
							mType->elementMap[COMBAT_EARTHDAMAGE] = intValue;
						}
						if(readXMLInteger(tmpNode, "firePercent", intValue)){
							mType->elementMap[COMBAT_FIREDAMAGE] = intValue;
						}
						if(readXMLInteger(tmpNode, "energyPercent", intValue)){
							mType->elementMap[COMBAT_ENERGYDAMAGE] = intValue;
						}
                        if(readXMLInteger(tmpNode, "lifeDrainPercent", intValue)){
							mType->elementMap[COMBAT_LIFEDRAIN] = intValue;
						}
                        if(readXMLInteger(tmpNode, "manaDrainPercent", intValue)){
							mType->elementMap[COMBAT_MANADRAIN] = intValue;
						}
                        if(readXMLInteger(tmpNode, "healingPercent", intValue)){
							mType->elementMap[COMBAT_HEALING] = intValue;
						}
					}
					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"summons") == 0){

				if(readXMLInteger(p, "maxSummons", intValue)){
					mType->maxSummons = std::min(intValue, 100);
				}
				else{
					SHOW_XML_WARNING("Missing summons.maxSummons");
				}

				xmlNodePtr tmpNode = p->children;
				while(tmpNode){

					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"summon") == 0){
						int32_t chance = 100;
						int32_t speed = 1000;
						int32_t max = 1;

						if(readXMLInteger(tmpNode, "max", intValue)){
							max = intValue;
						}

						if(readXMLInteger(tmpNode, "speed", intValue) || readXMLInteger(tmpNode, "interval", intValue)){
							speed = intValue;
						}

						if(readXMLInteger(tmpNode, "chance", intValue)){
							chance = intValue;
						}

						if(readXMLString(tmpNode, "name", strValue)){
							summonBlock_t sb;
							sb.name = strValue;
							sb.speed = speed;
							sb.max = max;
							sb.chance = chance;

							mType->summonList.push_back(sb);
						}
						else{
							SHOW_XML_WARNING("Missing summon.name");
						}
					}

					tmpNode = tmpNode->next;
				}
			}
			else if(xmlStrcmp(p->name, (const xmlChar*)"script") == 0){
				xmlNodePtr tmpNode = p->children;
				while(tmpNode){
					if(xmlStrcmp(tmpNode->name, (const xmlChar*)"event") == 0){

						if(readXMLString(tmpNode, "name", strValue)){
							mType->scriptList.push_back(strValue);
						}
						else{
							SHOW_XML_WARNING("Missing name for script event");
						}
					}
					tmpNode = tmpNode->next;
				}
			}
			else{
				SHOW_XML_WARNING("Unknown attribute type - " << p->name);
			}

			p = p->next;
		}

		xmlFreeDoc(doc);
	}
	else{
		monsterLoad = false;
	}

	if(monsterLoad){

		static uint32_t id = 0;
		if(new_mType){
			std::string lowername = monster_name;
			toLowerCaseString(lowername);

			id++;
			monsterNames[lowername] = id;
			monsters[id] = mType;
		}

		return true;
	}
	else{
		if(new_mType){
			delete mType;
		}
		return false;
	}
}

bool Monsters::loadLootItem(xmlNodePtr node, LootBlock& lootBlock)
{
	int intValue;
	std::string strValue;

	if(readXMLInteger(node, "id", intValue)){
		lootBlock.id = intValue;
	}

	if(lootBlock.id == 0){
		return false;
	}

	if(readXMLInteger(node, "countmax", intValue) && intValue > 0){
		lootBlock.countmax = intValue;
	}
	else{
		lootBlock.countmax = 1;
	}

	if(readXMLInteger(node, "chance", intValue) || readXMLInteger(node, "chance1", intValue)){
		lootBlock.chance = intValue;

		if(lootBlock.chance > MAX_LOOTCHANCE){
			lootBlock.chance = MAX_LOOTCHANCE;
		}
	}
	else{
		//std::cout << "missing chance for loot id = "<< lootBlock.id << std::endl;
		lootBlock.chance = MAX_LOOTCHANCE;
	}

	if(Item::items[lootBlock.id].isContainer()){
		loadLootContainer(node, lootBlock);
	}

	//optional
	if(readXMLInteger(node, "subtype", intValue)){
		lootBlock.subType = intValue;
	}

	if(readXMLInteger(node, "actionid", intValue) || readXMLInteger(node, "actionId", intValue)){
		lootBlock.actionId = intValue;
	}

	if(readXMLString(node, "text", strValue)){
		lootBlock.text = strValue;
	}

	if(readXMLString(node, "dropEmpty", strValue) || readXMLString(node, "dropempty", strValue)){
		lootBlock.dropEmpty = (asLowerCaseString(strValue) == "true");
	}

	return true;
}

bool Monsters::loadLootContainer(xmlNodePtr node, LootBlock& lBlock)
{
	if(node == NULL){
		return false;
	}

	xmlNodePtr tmpNode = node->children;
	xmlNodePtr p;

	if(tmpNode == NULL){
		return false;
	}

	while(tmpNode){
		if(xmlStrcmp(tmpNode->name, (const xmlChar*)"inside") == 0){
			p = tmpNode->children;
			while(p){
				LootBlock lootBlock;
				if(loadLootItem(p, lootBlock)){
					lBlock.childLoot.push_front(lootBlock);
				}
				p = p->next;
			}
			return true;
		}//inside

		tmpNode = tmpNode->next;
	}

	return false;
}

MonsterType* Monsters::getMonsterType(const std::string& name)
{
	uint32_t mId = getIdByName(name);
	if(mId == 0){
		return NULL;
	}

	return getMonsterType(mId);
}

MonsterType* Monsters::getMonsterType(uint32_t mid)
{
	MonsterMap::iterator it = monsters.find(mid);
	if(it != monsters.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

uint32_t Monsters::getIdByName(const std::string& name)
{
	std::string lower_name = name;
	std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), tolower);
	MonsterNameMap::iterator it = monsterNames.find(lower_name);
	if(it != monsterNames.end()){
		return it->second;
	}
	else{
		return 0;
	}
}

Monsters::~Monsters()
{
	for(MonsterMap::iterator it = monsters.begin(); it != monsters.end(); ++it)
		delete it->second;
}
