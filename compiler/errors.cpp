/*
	Aseba - an event-based framework for distributed robot control
	Copyright (C) 2007--2009:
		Stephane Magnenat <stephane at magnenat dot net>
		(http://stephane.magnenat.net)
		and other contributors, see authors.txt for details
		Mobots group, Laboratory of Robotics Systems, EPFL, Lausanne
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any other version as decided by the two original authors
	Stephane Magnenat and Valentin Longchamp.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "compiler.h"
#include <sstream>

namespace Aseba
{
	/** \addtogroup compiler */
	/*@{*/
	
	//! Return the string version of this error
	std::string Error::toString() const
	{
		std::ostringstream oss;
		if (pos.valid)
			oss << "Error at Line: " << pos.row + 1 << " Col: " << pos.column + 1 << " : " << message;
		else
			oss << "Error : " << message;
		return oss.str();
	}
	
	/*@}*/
	
}; // Aseba
