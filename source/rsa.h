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


#ifndef __OTSERV_RSA_H__
#define __OTSERV_RSA_H__

#include "definitions.h"
#include "gmp.h"
#include <string.h>
#include <boost/thread.hpp>

class RSA{
public:
	RSA();
	~RSA();
	void setKey(const char* p, const char* q, const char* d);
	bool setKey(const std::string& file);
	bool decrypt(char* msg, int32_t size);
	bool encrypt(char* msg, int32_t size, const char* key);

	int32_t getKeySize();
	void getPublicKey(char* buffer);

protected:

	bool m_keySet;

	boost::recursive_mutex rsaLock;

	//use only GMP
	mpz_t m_p, m_q, m_u, m_d, m_dp, m_dq, m_mod;
};

#endif
