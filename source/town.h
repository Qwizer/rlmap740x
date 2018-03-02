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


#ifndef __TOWN_H__
#define __TOWN_H__

#include "definitions.h"
#include "position.h"
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <list>
#include <map>

class Town
{
public:
	Town(uint32_t _townid)
	{
		townid = _townid;
	}
	
	~Town(){};
	
	const Position& getTemplePosition() const {return posTemple;};
	const std::string& getName() const {return townName;};

	void setTemplePos(const Position& pos) {posTemple = pos;};
	void setName(std::string _townName) {townName = _townName;};
	uint32_t getTownID() const {return townid;};

private:
	uint32_t townid;
	std::string townName;
	Position posTemple;
};

typedef std::map<uint32_t, Town*> TownMap;

class Towns
{
public:
	static Towns& getInstance()
	{
		static Towns instance;
		return instance;
	}

	bool addTown(uint32_t _townid, Town* town)
	{
		TownMap::iterator it = townMap.find(_townid);
		
		if(it != townMap.end()){
			return false;
		}

		townMap[_townid] = town;
		return true;
	}
	
	Town* getTown(const std::string& townname)
	{
		for(TownMap::iterator it = townMap.begin(); it != townMap.end(); ++it){
			if(boost::algorithm::iequals(it->second->getName(), townname)){
				return it->second;
			}
		}

		return NULL;
	}

	Town* getTown(uint32_t _townid)
	{
		TownMap::iterator it = townMap.find(_townid);
		
		if(it != townMap.end()){
			return it->second;
		}

		return NULL;
	}

	TownMap::const_iterator getTownBegin() const{return townMap.begin();}
	TownMap::const_iterator getTownEnd() const{return townMap.end();}

private:
	TownMap townMap;
};

#endif
