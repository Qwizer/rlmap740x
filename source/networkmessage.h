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

#ifndef __OTSERV_NETWORK_MESSAGE_H__
#define __OTSERV_NETWORK_MESSAGE_H__

#include "definitions.h"
#include "otsystem.h"
#include "const.h"
#include <string>
#include <boost/shared_ptr.hpp>

class Item;
class Creature;
class Player;
class Position;
class RSA;

class NetworkMessage
{
public:
	enum { header_length = 2 };
	enum { max_body_length = NETWORKMESSAGE_MAXSIZE - header_length };

	// constructor/destructor
	NetworkMessage(){
		Reset();
	}
	virtual ~NetworkMessage(){};

	// resets the internal buffer to an empty message
protected:
	void Reset(){
		m_overrun = false;
		m_MsgSize = 0;
		m_ReadPos = 4;
	}
public:

	// simply read functions for incoming message
	uint8_t  GetByte(){
		if(!expectRead(1)){
			return 0;
		}
		
		return m_MsgBuf[m_ReadPos++];
	}

#ifndef __SWAP_ENDIAN__
	uint16_t GetU16(){
		if(!expectRead(2)){
			return 0;
		}
		
		uint16_t v = *(uint16_t*)(m_MsgBuf + m_ReadPos);
		m_ReadPos += 2;
		return v;
	}
	uint32_t GetU32(){
		if(!expectRead(4)){
			return 0;
		}
		
		uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
		m_ReadPos += 4;
		return v;
	}
	uint32_t PeekU32(){
		if(!expectRead(4)){
			return 0;
		}
		
		uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
		return v;
	}
#else
	uint16_t GetU16(){
		if(!expectRead(2)){
			return 0;
		}
		
		uint16_t v = *(uint16_t*)(m_MsgBuf + m_ReadPos);
		m_ReadPos += 2;
		return swap_uint16(v);
	}
	uint32_t GetU32(){
		if(!expectRead(4)){
			return 0;
		}
		
		uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
		m_ReadPos += 4;
		return swap_uint32(v);
	}
	uint32_t PeekU32(){
		if(!expectRead(4)){
			return 0;
		}
		
		uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
		return swap_uint32(v);
	}
#endif

	uint16_t GetSpriteId(){
		return GetU16();
	}

	std::string GetString();
	std::string GetRaw();
	Position GetPosition();

	// skips count unknown/unused bytes in an incoming message
	void SkipBytes(int count){m_ReadPos += count;}

	// simply write functions for outgoing message
	void AddByte(uint8_t value){
		if(canAdd(1)){
			m_MsgBuf[m_ReadPos++] = value;
			m_MsgSize++;
		}
	}

#ifndef __SWAP_ENDIAN__
	void AddU16(uint16_t value){
		if(canAdd(2)){
			*(uint16_t*)(m_MsgBuf + m_ReadPos) = value;
			m_ReadPos += 2; m_MsgSize += 2;
		}
	}
	void AddU32(uint32_t value){
		if(canAdd(4)){
			*(uint32_t*)(m_MsgBuf + m_ReadPos) = value;
			m_ReadPos += 4; m_MsgSize += 4;
		}
	}
#else
	void AddU16(uint16_t value){
		if(canAdd(2)){
			*(uint16_t*)(m_MsgBuf + m_ReadPos) = swap_uint16(value);
			m_ReadPos += 2; m_MsgSize += 2;
		}
	}
	void AddU32(uint32_t value){
		if(canAdd(4)){
			*(uint32_t*)(m_MsgBuf + m_ReadPos) = swap_uint32(value);
			m_ReadPos += 4; m_MsgSize += 4;
		}
	}
#endif

	void AddBytes(const char* bytes, uint32_t size);
	void AddPaddingBytes(uint32_t n);

	void AddString(const std::string& value){AddString(value.c_str());}
	void AddString(const char* value);


	// write functions for complex types
	void AddPosition(const Position &pos);
	void AddItem(uint16_t id, uint8_t count);
	void AddItem(const Item *item);
	void AddItemId(const Item *item);
	void AddItemId(uint16_t itemId);
	void AddCreature(const Creature *creature, bool known, unsigned int remove);

	int32_t getMessageLength() const { return m_MsgSize; }
	void setMessageLength(int32_t newSize) { m_MsgSize = newSize; }
	int32_t getReadPos() const { return m_ReadPos; }
	void setReadPos(int32_t pos) {m_ReadPos = pos; }

	int32_t decodeHeader();
	
	bool isOverrun(){ return m_overrun; };

	char* getBuffer() { return (char*)&m_MsgBuf[0]; }
	char* getBodyBuffer() { m_ReadPos = 2; return (char*)&m_MsgBuf[header_length]; }

#ifdef __TRACK_NETWORK__
	virtual void Track(std::string file, long line, std::string func) {};
	virtual void clearTrack() {};
#endif


protected:
	inline bool canAdd(uint32_t size){
		return (size + m_ReadPos < max_body_length);
	};
	
	inline bool expectRead(int32_t size){
		if(size >= (NETWORKMESSAGE_MAXSIZE - m_ReadPos)){
			m_overrun = true;
			return false;
		}
		
		return true;
	};

	int32_t m_MsgSize;
	int32_t m_ReadPos;
	
	bool m_overrun;

	uint8_t m_MsgBuf[NETWORKMESSAGE_MAXSIZE];
};

typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;

#endif // #ifndef __NETWORK_MESSAGE_H__
