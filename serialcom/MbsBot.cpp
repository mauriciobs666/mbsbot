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

#include "MbsBot.h"

MbsBot *MbsBot::instance = 0;

int MbsBot::init(const char *port, int baud)
{
	if(port == NULL)
	{
		#ifdef __WXMSW__
			strcpy(serialPortDevice,"COM1");
		#else
			strcpy(serialPortDevice,"/dev/ttyUSB0");
		#endif
	}
	else
		strcpy(serialPortDevice,port);

	baudRate = (baud == -1) ? 115200 : baud;

	return serialPort.init( serialPortDevice, baudRate );
}

int MbsBot::send(const char * command, int len)
{
	if(len == -1)
		len = strlen(command);

	return serialPort.Write(command, len);
}

char * MbsBot::receive()
{
	char c;
	static int pos=0;

	while(serialPort.Read(&c, 1) > 0)
	{
		if (pos == SERIAL_BUFFER_SIZE-1)
		{
			pos=0;
		}
		else if(c == COMMAND_END)
		{
			response[pos]=0;
			pos=0;
			return response;
		}
		else
		{
			response[pos]=c;
			pos++;
		}
	}
	return NULL;
}

int MbsBot::setLeftWheel(int val)
{
	char cmd[20];
	snprintf(cmd, 20, "set l %d\n", val);
	return send(cmd);
}

int MbsBot::setRightWheel(int val)
{
	char cmd[20];
	snprintf(cmd, 20, "set r %d\n", val);
	return send(cmd);
}

int MbsBot::setHead(int val)
{
	char cmd[20];
	snprintf(cmd, 20, "set sx %d\n", val);
	return send(cmd);
}

int MbsBot::setLeftWheelCenter(int val)
{
	char cmd[20];
	snprintf(cmd, 20, "set lc %d\n", val);
	return send(cmd);
}

int MbsBot::setRightWheelCenter(int val)
{
	char cmd[20];
	snprintf(cmd, 20, "set rc %d\n", val);
	return send(cmd);
}

int MbsBot::drive(int lw, int rw)
{
	char cmd[20];
	snprintf(cmd, 20, "drv %d %d\n", lw, rw);
	return send(cmd);
}
