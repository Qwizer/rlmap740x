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

#include "actions.h"
#include "const.h"
#include "player.h"
#include "monster.h"
#include "npc.h"
#include "game.h"
#include "item.h"
#include "container.h"
#include "combat.h"
#include "house.h"
#include "tasks.h"
#include "tools.h"
#include "spells.h"
#include "configmanager.h"
#include "beds.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <sstream>

extern Game g_game;
extern Spells* g_spells;
extern Actions* g_actions;
extern ConfigManager g_config;

Actions::Actions() :
m_scriptInterface("Action Interface")
{
	defaultAction = NULL;
	m_scriptInterface.initState();
}

Actions::~Actions()
{
	clear();
}

inline void Actions::clearMap(ActionUseMap& map)
{
	ActionUseMap::iterator it = map.begin();
	while(it != map.end()){
		delete it->second;
		map.erase(it);
		it = map.begin();
	}
}

void Actions::clear()
{
	clearMap(useItemMap);
	clearMap(uniqueItemMap);
	clearMap(actionItemMap);
	if(defaultAction){
		delete defaultAction;
		defaultAction = NULL;
	}

	m_scriptInterface.reInitState();
	m_loaded = false;
}

LuaScriptInterface& Actions::getScriptInterface()
{
	return m_scriptInterface;
}

std::string Actions::getScriptBaseName()
{
	return "actions";
}

Event* Actions::getEvent(const std::string& nodeName)
{
	if(asLowerCaseString(nodeName) == "action"){
		return new Action(&m_scriptInterface);
	}
	else{
		return NULL;
	}
}

bool Actions::registerEvent(Event* event, xmlNodePtr p)
{
	Action* action = dynamic_cast<Action*>(event);
	if(!action)
		return false;

	int32_t id, toId;
	if(readXMLInteger(p,"itemid",id) || readXMLInteger(p,"fromitemid",id) || readXMLInteger(p,"fromid",id)){
		if(!readXMLInteger(p,"toitemid",toId) && !readXMLInteger(p,"toid",toId))
			toId = id;
		for(; toId >= id; --toId){
			useItemMap[toId] = action->clone();
		}
	}
	else if(readXMLInteger(p,"uniqueid",id) || readXMLInteger(p,"fromuniqueid",id) || readXMLInteger(p,"fromuid",id)){
		if(!readXMLInteger(p,"touniqueid",toId) && !readXMLInteger(p,"touid",toId))
			toId = id;
		for(; toId >= id; --toId){
			uniqueItemMap[toId] = action->clone();
		}
	}
	else if(readXMLInteger(p,"actionid",id) || readXMLInteger(p,"fromactionid",id) || readXMLInteger(p,"fromaid",id)){
		if(!readXMLInteger(p,"toactionid",toId) && !readXMLInteger(p,"toaid",toId))
			toId = id;
		for(; toId >= id; --toId){
			actionItemMap[toId] = action->clone();
		}
	}
	else if(readXMLInteger(p,"default",id) && id != 0) {
		if(!defaultAction)
			defaultAction = action->clone();
		else
			std::cout << "Warning: you can't define more than one default action." << std::endl;
	}
	else{
		return false;
	}

	delete event;
	return true;
}

ReturnValue Actions::canUse(const Player* player, const Position& pos)
{
	const Position& playerPos = player->getPosition();

	if(pos.x != 0xFFFF){
		if(playerPos.z > pos.z){
			return RET_FIRSTGOUPSTAIRS;
		}
		else if(playerPos.z < pos.z){
			return RET_FIRSTGODOWNSTAIRS;
		}
		else if(!Position::areInRange<1,1,0>(playerPos, pos)){
			return RET_TOOFARAWAY;
		}
	}

	return RET_NOERROR;
}

bool Actions::hasAction(const Item* item) const
{
	return	(getAction(item, ACTION_UNIQUEID) != NULL) ||
			(getAction(item, ACTION_ACTIONID) != NULL) ||
			(getAction(item, ACTION_ITEMID) != NULL) ||
			(getAction(item, ACTION_RUNEID) != NULL);
}

ReturnValue Actions::canUse(const Player* player, const Position& pos, const Item* item)
{
	Action* action = getAction(item, ACTION_UNIQUEID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, pos);
		if(ret != RET_NOERROR){
			return ret;
		}

		return RET_NOERROR;
	}

	action = getAction(item, ACTION_ACTIONID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, pos);
		if(ret != RET_NOERROR){
			return ret;
		}

		return RET_NOERROR;
	}

	action = getAction(item, ACTION_ITEMID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, pos);
		if(ret != RET_NOERROR){
			return ret;
		}

		return RET_NOERROR;
	}

	action = getAction(item, ACTION_RUNEID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, pos);
		if(ret != RET_NOERROR){
			return ret;
		}

		return RET_NOERROR;
	}

	if(defaultAction){
		ReturnValue ret = defaultAction->canExecuteAction(player, pos);
		if(ret != RET_NOERROR){
			return ret;
		}
		return RET_NOERROR;
	}

	return RET_NOERROR;
}

ReturnValue Actions::canUseFar(const Creature* creature, const Position& toPos, bool checkLineOfSight)
{
	if(toPos.x == 0xFFFF){
		return RET_NOERROR;
	}

	const Position& creaturePos = creature->getPosition();

	if(creaturePos.z > toPos.z){
		return RET_FIRSTGOUPSTAIRS;
	}
	else if(creaturePos.z < toPos.z){
		return RET_FIRSTGODOWNSTAIRS;
	}
	else if(!Position::areInRange<7,5,0>(toPos, creaturePos)){
		return RET_TOOFARAWAY;
	}

	if(checkLineOfSight && !g_game.canThrowObjectTo(creaturePos, toPos)){
		return RET_CANNOTTHROW;
	}

	return RET_NOERROR;
}

Action* Actions::getAction(const Item* item, ActionType_t type /* = ACTION_ANY*/) const
{
	if(item->getUniqueId() != 0 && (type == ACTION_ANY || type == ACTION_UNIQUEID) ){
		ActionUseMap::const_iterator it = uniqueItemMap.find(item->getUniqueId());
		if(it != uniqueItemMap.end()){
			return it->second;
		}
	}
	if(item->getActionId() != 0 && (type == ACTION_ANY || type == ACTION_ACTIONID)){
		ActionUseMap::const_iterator it = actionItemMap.find(item->getActionId());
		if (it != actionItemMap.end()){
			return it->second;
		}
	}

	if(type == ACTION_ANY || type == ACTION_ITEMID){
		ActionUseMap::const_iterator it = useItemMap.find(item->getID());
		if(it != useItemMap.end()){
	   		return it->second;
		}
	}

	if(type == ACTION_ANY || type == ACTION_RUNEID){
		//rune items
		Action* runeSpell = g_spells->getRuneSpell(item->getID());
		if(runeSpell){
			return runeSpell;
		}
	}

	return NULL;
}

bool Actions::executeUse(Action* action, Player* player, Item* item,
	const PositionEx& posEx, uint32_t creatureId)
{
	if(!action->executeUse(player, item, posEx, posEx, false, creatureId)){
		return false;
	}

	return true;
}

ReturnValue Actions::internalUseItem(Player* player, const Position& pos,
	uint8_t index, Item* item, uint32_t creatureId)
{
	//check if it is a house door
	if(Door* door = item->getDoor()){
		if(!door->canUse(player)){
			return RET_CANNOTUSETHISOBJECT;
		}
	}

	int32_t stack = item->getParent()->__getIndexOfThing(item);
	PositionEx posEx(pos, stack);
	bool foundAction = false;

	Action* action = getAction(item, ACTION_UNIQUEID);
	if(action){
		//only continue with next action in the list if the previous returns false
		if(executeUse(action, player, item, posEx, creatureId)){
			return RET_NOERROR;
		}
		foundAction = true;
	}

	action = getAction(item, ACTION_ACTIONID);
	if(action){
		//only continue with next action in the list if the previous returns false
		if(executeUse(action, player, item, posEx, creatureId)){
			return RET_NOERROR;
		}
		foundAction = true;
	}

	action = getAction(item, ACTION_ITEMID);
	if(action){
		//only continue with next action in the list if the previous returns false
		if(executeUse(action, player, item, posEx, creatureId)){
			return RET_NOERROR;
		}
		foundAction = true;
	}

	action = getAction(item, ACTION_RUNEID);
	if(action){
		//only continue with next action in the list if the previous returns false
		if(executeUse(action, player, item, posEx, creatureId)){
			return RET_NOERROR;
		}
		foundAction = true;
	}

	if(!foundAction && defaultAction){
		foundAction = executeUse(defaultAction, player, item, posEx, creatureId);
	}

	if(BedItem* bed = item->getBed()){
		if(!bed->canUse(player)){
			return RET_CANNOTUSETHISOBJECT;
		}

		bed->sleep(player);
		return RET_NOERROR;
	}

	//if it is a container try to open it
	if(Container* container = item->getContainer()){
		Container* openContainer = NULL;
		//depot container
		if(Depot* depot = container->getDepot()){
			Depot* myDepot = player->getDepot(depot->getDepotId(), true);
			myDepot->setParent(depot->getParent());
			openContainer = myDepot;
		}
		else{
			openContainer = container;
		}

		//open/close container
		int32_t oldcid = player->getContainerID(openContainer);
		if(oldcid != -1){
			player->onCloseContainer(openContainer);
			player->closeContainer(oldcid);
		}
		else{
			player->addContainer(index, openContainer);
			player->onSendContainer(openContainer);
		}
		return RET_NOERROR;
	}

	if(item->isReadable()){
		if(item->canWriteText()){
			player->setWriteItem(item, item->getMaxWriteLength());
			player->sendTextWindow(item, item->getMaxWriteLength(), true);
		}
		else{
			player->setWriteItem(NULL);
			player->sendTextWindow(item, 0, false);
		}
		return RET_NOERROR;
	}

	if(foundAction)
		return RET_NOERROR;

	return RET_CANNOTUSETHISOBJECT;
}

bool Actions::useItem(Player* player, const Position& pos, uint8_t index,
	Item* item)
{
	if(!player->canDoAction()){
		return false;
	}

	player->setNextActionTask(NULL);
	player->stopWalk();

	player->setNextAction(OTSYS_TIME() + g_config.getNumber(ConfigManager::MIN_ACTIONTIME) + SCHEDULER_MINTICKS);
	ReturnValue ret = internalUseItem(player, pos, index, item, 0);
	if(ret != RET_NOERROR){
		player->sendCancelMessage(ret);
		return false;
	}

	return true;
}

bool Actions::executeUseEx(Action* action, Player* player, Item* item, const PositionEx& fromPosEx,
	const PositionEx& toPosEx, uint32_t creatureId)
{
	if(!action->executeUse(player, item, fromPosEx, toPosEx, true, creatureId)){
		return false;
	}

	return true;
}

ReturnValue Actions::internalUseItemEx(Player* player, const PositionEx& fromPosEx, const PositionEx& toPosEx,
	Item* item, uint32_t creatureId, bool& isSuccess)
{
	isSuccess = false;
	bool foundAction = false;

	Action* action = getAction(item, ACTION_UNIQUEID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, toPosEx);
		if(ret != RET_NOERROR){
			return ret;
		}

		//only continue with next action in the list if the previous returns false
		isSuccess = executeUseEx(action, player, item, fromPosEx, toPosEx, creatureId);
		if(isSuccess || action->hasOwnErrorHandler()){
			return RET_NOERROR;
		foundAction = true;
		}
	}

	action = getAction(item, ACTION_ACTIONID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, toPosEx);
		if(ret != RET_NOERROR){
			isSuccess = false;
			return ret;
		}

		//only continue with next action in the list if the previous returns false
		isSuccess = executeUseEx(action, player, item, fromPosEx, toPosEx, creatureId);
		if(isSuccess || action->hasOwnErrorHandler()){
			return RET_NOERROR;
		foundAction = true;
		}
	}

	action = getAction(item, ACTION_ITEMID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, toPosEx);
		if(ret != RET_NOERROR){
			return ret;
		}

		//only continue with next action in the list if the previous returns false
		isSuccess = executeUseEx(action, player, item, fromPosEx, toPosEx, creatureId);
		if(isSuccess || action->hasOwnErrorHandler()){
			return RET_NOERROR;
		foundAction = true;
		}
	}

	action = getAction(item, ACTION_RUNEID);
	if(action){
		ReturnValue ret = action->canExecuteAction(player, toPosEx);
		if(ret != RET_NOERROR){
			return ret;
		}

		//only continue with next action in the list if the previous returns false
		isSuccess = executeUseEx(action, player, item, fromPosEx, toPosEx, creatureId);
		if(isSuccess || action->hasOwnErrorHandler()){
			return RET_NOERROR;
		foundAction = true;
		}
	}

	if(defaultAction){
		isSuccess = executeUseEx(defaultAction, player, item, fromPosEx, toPosEx, creatureId);
		if(isSuccess || defaultAction->hasOwnErrorHandler()){
			return RET_NOERROR;
		foundAction = true;
		}
	}

	if(foundAction)
		return RET_NOERROR;

	return RET_CANNOTUSETHISOBJECT;
}

bool Actions::useItemEx(Player* player, const Position& fromPos, const Position& toPos,
	uint8_t toStackPos, Item* item, uint32_t creatureId/* = 0*/)
{
	if(!player->canDoAction()){
		return false;
	}

	player->setNextActionTask(NULL);
	player->stopWalk();

	int32_t fromStackPos = item->getParent()->__getIndexOfThing(item);
	PositionEx fromPosEx(fromPos, fromStackPos);
	PositionEx toPosEx(toPos, toStackPos);
	ReturnValue ret = RET_NOERROR;
	bool isSuccess = false;

	player->setNextAction(OTSYS_TIME() + g_config.getNumber(ConfigManager::MIN_ACTIONEXTIME) + SCHEDULER_MINTICKS);
	ret = internalUseItemEx(player, fromPosEx, toPosEx, item, creatureId, isSuccess);

	if(ret != RET_NOERROR){
		player->sendCancelMessage(ret);
		return false;
	}

	return true;
}

Action::Action(LuaScriptInterface* _interface) :
Event(_interface)
{
	allowFarUse = false;
	checkLineOfSight = true;
}

Action::~Action()
{
	//
}

bool Action::configureEvent(xmlNodePtr p)
{
	int intValue;
	if(readXMLInteger(p, "allowfaruse", intValue)){
		if(intValue != 0){
			setAllowFarUse(true);
		}
	}
	if(readXMLInteger(p, "blockwalls", intValue)){
		if(intValue == 0){
			setCheckLineOfSight(false);
		}
	}

	return true;
}

std::string Action::getScriptEventName()
{
	return "onUse";
}

ReturnValue Action::canExecuteAction(const Player* player, const Position& toPos)
{
	ReturnValue ret = RET_NOERROR;

	if(!getAllowFarUse()){
		ret = g_actions->canUse(player, toPos);
		if(ret != RET_NOERROR){
			return ret;
		}
	}
	else{
		ret = g_actions->canUseFar(player, toPos, getCheckLineOfSight());
		if(ret != RET_NOERROR){
			return ret;
		}
	}

	return RET_NOERROR;
}

bool Action::executeUse(Player* player, Item* item, const PositionEx& fromPos,
	const PositionEx& toPos, bool extendedUse, uint32_t creatureId)
{
	//onUse(cid, item1, position1, item2, position2)
	if(m_scriptInterface->reserveScriptEnv()){
		ScriptEnviroment* env = m_scriptInterface->getScriptEnv();

		#ifdef __DEBUG_LUASCRIPTS__
		std::stringstream desc;
		desc << player->getName() << " - " << item->getID() << " " << fromPos << "|" << toPos;
		env->setEventDesc(desc.str());
		#endif

		env->setScriptId(m_scriptId, m_scriptInterface);
		env->setRealPos(player->getPosition());

		uint32_t cid = env->addThing(player);
		uint32_t itemid1 = env->addThing(item);

		lua_State* L = m_scriptInterface->getLuaState();

		m_scriptInterface->pushFunction(m_scriptId);
		lua_pushnumber(L, cid);
		LuaScriptInterface::pushThing(L, item, itemid1);
		LuaScriptInterface::pushPosition(L, fromPos, fromPos.stackpos);
		//std::cout << "posTo" <<  (Position)posTo << " stack" << (int)posTo.stackpos <<std::endl;
		Thing* thing = g_game.internalGetThing(player, toPos, toPos.stackpos);
		if(thing && (!extendedUse || thing != item)){
			uint32_t thingId2 = env->addThing(thing);
			LuaScriptInterface::pushThing(L, thing, thingId2);
			LuaScriptInterface::pushPosition(L, toPos, toPos.stackpos);
		}
		else{
			LuaScriptInterface::pushThing(L, NULL, 0);
			Position posEx;
			LuaScriptInterface::pushPosition(L, posEx, 0);
		}

		bool result = m_scriptInterface->callFunction(5);
		m_scriptInterface->releaseScriptEnv();

		return result;
	}
	else{
		std::cout << "[Error] Call stack overflow. Action::executeUse" << std::endl;
		return false;
	}
}
