////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// Global events.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#ifndef __OTSERV_GLOBALEVENT_H__
#define __OTSERV_GLOBALEVENT_H__
#include "baseevents.h"

#include "const.h"

#define TIMER_INTERVAL 1000

enum GlobalEvent_t
{
	GLOBALEVENT_NONE,
	GLOBALEVENT_TIMER,
	GLOBALEVENT_STARTUP,
	GLOBALEVENT_SHUTDOWN
};

class GlobalEvent;
typedef std::map<std::string, GlobalEvent*> GlobalEventMap;

class GlobalEvents : public BaseEvents
{
	public:
		GlobalEvents();
		virtual ~GlobalEvents();
		void startup();

		void timer();
		void think();
		void execute(GlobalEvent_t type);

		GlobalEventMap getEventMap(GlobalEvent_t type);
		void clearMap(GlobalEventMap& map);

	protected:
		virtual std::string getScriptBaseName() {return "globalevents";}
		virtual void clear();

		virtual Event* getEvent(const std::string& nodeName);
		virtual bool registerEvent(Event* event, xmlNodePtr p);

		virtual LuaScriptInterface& getScriptInterface() {return m_scriptInterface;}
		LuaScriptInterface m_scriptInterface;

		GlobalEventMap thinkMap, serverMap, timerMap;
		int32_t thinkEventId, timerEventId;
};

class GlobalEvent : public Event
{
	public:
		GlobalEvent(LuaScriptInterface* _interface);
		virtual ~GlobalEvent() {}

		virtual bool configureEvent(xmlNodePtr p);

		uint32_t executeRecord(uint32_t current, uint32_t old);
		uint32_t executeEvent();

		GlobalEvent_t getEventType() const {return m_eventType;}
		std::string getName() const {return m_name;}

		uint32_t getInterval() const {return m_interval;}

		int64_t getLastExecution() const {return m_lastExecution;}
		void setLastExecution(int64_t time) {m_lastExecution = time;}

		time_t getNextExecution() const {return m_nextExecution;}
		void setNextExecution(time_t time) {m_nextExecution = time;}

	protected:
		GlobalEvent_t m_eventType;

		virtual std::string getScriptEventName();

		std::string m_name;
		time_t m_nextExecution;
		int64_t m_lastExecution;
		uint32_t m_interval;
};

#endif
