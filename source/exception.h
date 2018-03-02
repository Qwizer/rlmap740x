//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Exception Handler class
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

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "definitions.h"

#ifdef __WINDOWS__
	#include <Windows.h>
	#include <dbghelp.h>
#endif

class ExceptionHandler
{
public:
	ExceptionHandler();
	~ExceptionHandler();
	bool InstallHandler();
	bool RemoveHandler();

private:
#ifdef __WINDOWS__
	static long WINAPI MiniDumpExceptionHandler(EXCEPTION_POINTERS* exceptionPointers = NULL);
	static int ref_counter;
#endif
	bool isInstalled;
};

#endif