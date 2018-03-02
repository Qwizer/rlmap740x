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

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
#include "outputmessage.h"
#include "connection.h"
#include "admin.h"
#include "status.h"
#include "protocollogin.h"
#endif

#include "creature.h"
#include "player.h"
#include "tools.h"
#include "game.h"
#include "house.h"
#include "ioplayer.h"
#include "configmanager.h"
#include "town.h"
#include "raids.h"
#include "tools.h"
#include "talkaction.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

extern Game g_game;
extern ConfigManager g_config;

TalkActions::TalkActions() :
m_scriptInterface("TalkAction Interface")
{
	m_scriptInterface.initState();
}

TalkActions::~TalkActions()
{
	clear();
}

void TalkActions::clear()
{
	TalkActionList::iterator it = wordsMap.begin();
	while(it != wordsMap.end()){
		delete it->second;
		wordsMap.erase(it);
		it = wordsMap.begin();
	}

	m_scriptInterface.reInitState();
}

LuaScriptInterface& TalkActions::getScriptInterface()
{
	return m_scriptInterface;
}

std::string TalkActions::getScriptBaseName()
{
	return "talkactions";
}

Event* TalkActions::getEvent(const std::string& nodeName)
{
	if(nodeName == "talkaction"){
		return new TalkAction(&m_scriptInterface);
	}
	else{
		return NULL;
	}
}

bool TalkActions::registerEvent(Event* event, xmlNodePtr p)
{
	TalkAction* talkAction = dynamic_cast<TalkAction*>(event);
	if(!talkAction)
		return false;

	wordsMap.push_back(std::make_pair(talkAction->getWords(), talkAction));
	return true;
}

TalkActionResult_t TalkActions::onPlayerSpeak(Player* player, SpeakClasses type, const std::string& words)
{
	if(type != SPEAK_SAY){
		return TALKACTION_CONTINUE;
	}

	std::string str_words_quote;
	std::string str_param_quote;
	std::string str_words_first_word;
	std::string str_param_first_word;

	// With quotation filtering
	size_t loc = words.find( '"', 0 );
	if(loc != std::string::npos && loc >= 0){
		str_words_quote = std::string(words, 0, loc);
		str_param_quote = std::string(words, (loc+1), words.size()-loc-1);
	}
	else {
		str_words_quote = words;
		str_param_quote = std::string("");
	}

	trim_left(str_words_quote, " ");
	trim_right(str_param_quote, " ");

	// With whitespace filtering
	loc = words.find( ' ', 0 );
	if(loc != std::string::npos && loc >= 0){
		str_words_first_word = std::string(words, 0, loc);
		str_param_first_word = std::string(words, (loc+1), words.size()-loc-1);
	}
	else {
		str_words_first_word = words;
		str_param_first_word = std::string("");
	}

	TalkActionList::iterator it;
	for(it = wordsMap.begin(); it != wordsMap.end(); ++it){
		std::string cmdstring;
		std::string paramstring;
		if(it->second->getFilterType() == TALKACTION_MATCH_QUOTATION) {
			cmdstring = str_words_quote;
			paramstring = str_param_quote;
		}
		else if(it->second->getFilterType() == TALKACTION_MATCH_FIRST_WORD) {
			cmdstring = str_words_first_word;
			paramstring = str_param_first_word;
		}
		else {
			continue;
		}
		if(cmdstring == it->first || (!it->second->isCaseSensitive() && boost::algorithm::iequals(it->first, cmdstring))){
			bool ret = true;
			if(player->getAccessLevel() < it->second->getAccessLevel()){
				if(player->getAccessLevel() > 0){
					player->sendTextMessage(MSG_STATUS_SMALL, "You can not execute this command.");
					ret = false;
				}
			}
			else{
				TalkAction* talkAction = it->second;

				if(talkAction->isScripted()){
					ret = talkAction->executeSay(player, cmdstring, paramstring);
				}
				else{
					TalkActionFunction* func = talkAction->getFunction();
					if(func){
						func(player, cmdstring, paramstring);
						ret = false;
					}
				}
			}

			if(ret){
				return TALKACTION_CONTINUE;
			}
			else{
				return TALKACTION_BREAK;
			}
		}
	}

	return TALKACTION_CONTINUE;
}


TalkAction::TalkAction(LuaScriptInterface* _interface) :
Event(_interface),
filterType(TALKACTION_MATCH_QUOTATION),
caseSensitive(false),
accessLevel(0),
function(NULL)
{
	//
}

TalkAction::~TalkAction()
{
	//
}

bool TalkAction::configureEvent(xmlNodePtr p)
{
	std::string str;
	int intValue;
	if(readXMLString(p, "words", str)){
		commandString = str;
	}
	else{
		std::cout << "Error: [TalkAction::configureEvent] No words for TalkAction or Spell." << std::endl;
		return false;
	}

	if(readXMLString(p, "filter", str)){
		if(str == "quotation") {
			filterType = TALKACTION_MATCH_QUOTATION;
		} else if(str == "first word") {
			filterType = TALKACTION_MATCH_FIRST_WORD;
		}
	}

	if(readXMLInteger(p, "case-sensitive", intValue) || readXMLInteger(p, "sensitive", intValue)){
		caseSensitive = (intValue != 0);
	}

	if(readXMLInteger(p, "access", intValue)){
		accessLevel = intValue;
	}

	return true;
}

bool TalkAction::loadFunction(const std::string& functionName)
{
	if(asLowerCaseString(functionName) == "placenpc"){
		function = placeNpc;
	}
	else if(asLowerCaseString(functionName) == "sellhouse"){
		function = sellHouse;
	}
	else if(asLowerCaseString(functionName) == "forceraid"){
		function = forceRaid;
	}
	#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	else if(asLowerCaseString(functionName) == "serverdiagnostic"){
		function = serverDiag;
	}
	#endif
	else{
		return false;
	}

	m_scripted = false;
	return true;
}

std::string TalkAction::getScriptEventName()
{
	return "onSay";
}

bool TalkAction::executeSay(Player* player, const std::string& words, const std::string& param)
{
	//onSay(cid, words, param)
	if(m_scriptInterface->reserveScriptEnv()){
		ScriptEnviroment* env = m_scriptInterface->getScriptEnv();

		#ifdef __DEBUG_LUASCRIPTS__
		std::stringstream desc;
		desc << player->getName() << " - " << words << " " << param;
		env->setEventDesc(desc.str());
		#endif

		env->setScriptId(m_scriptId, m_scriptInterface);
		env->setRealPos(player->getPosition());

		uint32_t cid = env->addThing(player);

		lua_State* L = m_scriptInterface->getLuaState();

		m_scriptInterface->pushFunction(m_scriptId);
		lua_pushnumber(L, cid);
		lua_pushstring(L, words.c_str());
		lua_pushstring(L, param.c_str());

		bool result = m_scriptInterface->callFunction(3);
		m_scriptInterface->releaseScriptEnv();

		return result;
	}
	else{
		std::cout << "[Error] Call stack overflow. TalkAction::executeSay" << std::endl;
		return false;
	}
}

bool TalkAction::placeNpc(Player* player, const std::string& words, const std::string& param)
{
	if(!player){
		return false;
	}

	Npc* npc = Npc::createNpc(param);
	if(!npc){
		player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "This NPC does not exist.");
		return false;
	}

	// Place the npc
	if(g_game.placeCreature(npc, player->getPosition())){
		g_game.addMagicEffect(player->getPosition(), NM_ME_MAGIC_BLOOD);
		npc->setMasterPos(npc->getPosition());
		return true;
	}
	else{
		delete npc;
		player->sendCancelMessage(RET_NOTENOUGHROOM);
		g_game.addMagicEffect(player->getPosition(), NM_ME_PUFF);
	}

	return false;
}

bool TalkAction::sellHouse(Player* player, const std::string& words, const std::string& param)
{
	if(!player){
		return false;
	}

	House* house = Houses::getInstance().getHouseByPlayerId(player->getGUID());

	if(!house){
		player->sendCancel("You do not own any house.");
		return false;
	}

	if(!Houses::getInstance().payRent(player, house)){
		player->sendCancel("You have to pay the rent before selling your house and you do not have enough money.");
		return false;
	}

	Player* tradePartner = g_game.getPlayerByName(param);
	if(!(tradePartner && tradePartner != player)){
		player->sendCancel("Trade player not found.");
		return false;
	}

	if(!tradePartner->isPremium() && g_config.getNumber(ConfigManager::HOUSE_ONLY_PREMIUM)){
		player->sendCancel("Trade player doesn't have a premium account.");
		return false;
	}

	if(tradePartner->getPlayerInfo(PLAYERINFO_LEVEL) < g_config.getNumber(ConfigManager::HOUSE_LEVEL)){
		player->sendCancel("Trade player level is too low.");
		return false;
	}

	if(Houses::getInstance().getHouseByPlayerId(tradePartner->getGUID())){
		player->sendCancel("Trade player already owns a house.");
		return false;
	}

	if(!Position::areInRange<2,2,0>(tradePartner->getPosition(), player->getPosition())){
		player->sendCancel("Trade player is too far away.");
		return false;
	}

	Item* transferItem = house->getTransferItem();
	if(!transferItem){
		player->sendCancel("You can not trade this house.");
		return false;
	}

	transferItem->getParent()->setParent(player);
	if(g_game.internalStartTrade(player, tradePartner, transferItem)){
		return true;
	}
	else{
		house->resetTransferItem();
	}

	return false;
}

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
bool TalkAction::serverDiag(Player* player, const std::string& words, const std::string& param)
{
	if(!player){
		return false;
	}

	std::stringstream text;
	text << "Server diagonostic:\n";
	text << "World:" << "\n";
	text << "Player: " << g_game.getPlayersOnline() << " (" << Player::playerCount << ")\n";
	text << "Npc: " << g_game.getNpcsOnline() << " (" << Npc::npcCount << ")\n";
	text << "Monster: " << g_game.getMonstersOnline() << " (" << Monster::monsterCount << ")\n";

	text << "\nProtocols:" << "\n";
	text << "--------------------\n";
	text << "ProtocolGame: " << ProtocolGame::protocolGameCount << "\n";
	text << "ProtocolLogin: " << ProtocolLogin::protocolLoginCount << "\n";
	text << "ProtocolAdmin: " << ProtocolAdmin::protocolAdminCount << "\n";
	text << "ProtocolStatus: " << ProtocolStatus::protocolStatusCount << "\n\n";

	text << "\nConnections:\n";
	text << "--------------------\n";
	text << "Active connections: " << Connection::connectionCount << "\n";
	text << "Total message pool: " << OutputMessagePool::getInstance()->getTotalMessageCount() << "\n";
	text << "Auto message pool: " << OutputMessagePool::getInstance()->getAutoMessageCount() << "\n";
	text << "Free message pool: " << OutputMessagePool::getInstance()->getAvailableMessageCount() << "\n";

	text << "\nLibraries:\n";
	text << "--------------------\n";
	text << "asio: " << BOOST_ASIO_VERSION << "\n";
	text << "libxml: " << XML_DEFAULT_VERSION << "\n";
	text << "lua: " << LUA_RELEASE << "\n";

	//TODO: more information that could be useful

	player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, text.str().c_str());

	return true;
}
#endif

bool TalkAction::forceRaid(Player* player, const std::string& words, const std::string& param)
{
	if(!player){
		return false;
	}

	Raid* raid = Raids::getInstance()->getRaidByName(param);
	if(!raid || !raid->isLoaded()){
		player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "No such raid exists.");
		return false;
	}

	if(Raids::getInstance()->getRunning()){
		player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Another raid is already being executed.");
		return false;
	}

	Raids::getInstance()->setRunning(raid);
	RaidEvent* event = raid->getNextRaidEvent();

	if(!event){
		player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "The raid does not contain any data.");
		return false;
	}

	raid->setState(RAIDSTATE_EXECUTING);
	uint32_t ticks = event->getDelay();
	if(ticks > 0){
		g_scheduler.addEvent(createSchedulerTask(ticks,
			boost::bind(&Raid::executeRaidEvent, raid, event)));
	}
	else{
		g_dispatcher.addTask(createTask(
		boost::bind(&Raid::executeRaidEvent, raid, event)));
	}

	player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Raid started.");
	return true;
}
