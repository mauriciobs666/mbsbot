/*
 *	Copyright (C) 2010 - Mauricio Bieze Stefani
 *	This file is part of the MBSBOT project.
 *
 *	MBSBOT is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	MBSBOT is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

/* General packet format
<command> [arguments] COMMAND_END
*/
#define COMMAND_END '\n'

/* Command reference:

get [variable]
	l	-	left wheel servo
	lc	-	left wheel servo center pulse duration (in ms)
	r	-	right wheel servo
	rc	-	right wheel servo center pulse duration (in ms)
	p	-	current program
	di	-	drive.inch() delay configuration
	drf	-	range finder between readings delay (servo movement)
	as	-	all analog sensors
	sx	-	servo "X"

set	[variable] [=] [value]
	variable	-	same used for 'get'
	value		-	int

save
	save to eeprom

load
	discard changes and reload from eeprom

cal
	line-follower auto calibration

default
	load default hard-coded values into RAM

inch
	move forward one inch

stop
	stop wheels
*/

// programs available:

enum ProgramID
{
	PRG_RC = 0,			// Remote control
	PRG_SHOW_SENSORS,	// Remote control with sensor monitoring
	PRG_PHOTOVORE,		//
	PRG_LINEFOLLOWER,	//
	PRG_SHARP,			// Sharp IR ranger test
	PRG_SHARP_CHASE		//
};

#endif // PROTOCOL_H_INCLUDED
