/**	Copyright (C) 2010 - Mauricio Bieze Stefani
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

#define PROTOCOL_VERSION 1

/* General packet format
<command> [arguments] COMMAND_END
*/
#define COMMAND_END '\n'
#define MAX_COMMAND_SIZE 50

#define CMD_READ	"get"
/* CMD_READ [variable]
	l	-	left wheel servo
	lc	-	left wheel servo center pulse duration (in ms)
	r	-	right wheel servo
	rc	-	right wheel servo center pulse duration (in ms)
	p	-	current program
	di	-	drive.inch() delay configuration
	drf	-	range finder between readings delay (servo movement)
	as	-	all analog sensors
	sx	-	servo "X"
*/

#define CMD_WRITE	"set"
/* CMD_WRITE [variable] [=] [value]
	variable	-	same used for CMD_WRITE
	value		-	int
*/

#define CMD_SAVE	"save"
/* CMD_SAVE
save to eeprom
*/

#define CMD_LOAD	"load"
/* CMD_LOAD
discard changes and reload from eeprom
*/

#define CMD_DEFAULT	"default"
/* CMD_DEFAULT
load default hard-coded values into RAM
*/

#define CMD_LF_CAL	"cal"
/* CMD_LF_CAL
line-follower auto calibration
*/

#define CMD_MV_INCH	"inch"
/* CMD_MV_INCH
move forward one inch
*/

#define CMD_MV_STOP	"stop"
/* CMD_MV_STOP
stop wheels
*/

#define CMD_MV_WHEELS	"drv"
/* CMD_MV_WHEELS leftwheel rightwheel [duration]
	leftwheel	+/- 0-100 %
	rightwheel 	+/- 0-100 %
	duration    time in ms
*/

#define CMD_MV_VECT "vect"
/* CMD_MV_VECT X_axis Y_axis [duration]
	X_axis	    +/- 0-100 %
	Y_axis 	    +/- 0-100 %
	duration    time in ms
*/

#define CMD_STATUS	"status"
/* CMD_STATUS
*/

#define CMD_UNAME	"uname"
/* CMD_UNAME
*/

#define CMD_BEEP	"beep"
/* CMD_BEEP [frequency]
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
