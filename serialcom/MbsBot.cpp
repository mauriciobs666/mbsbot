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

MbsBot::MbsBot()
{
	memset(serialPortDevice, 0, sizeof(serialPortDevice));
}

int MbsBot::init(const char *port, int baud)
{
	loadLocalConfig();

	printf("serialPortDevice from file = %s\n", serialPortDevice);

	if(port == NULL)
	{
		if(strcmp(serialPortDevice, "") == 0)
		{
			// use predefined value
			#ifdef __WXMSW__
				strcpy(serialPortDevice,"COM1");
			#else
				strcpy(serialPortDevice,"/dev/ttyUSB0");
			#endif
		}
	}
	else
		strcpy(serialPortDevice, port);

	if (baud == -1)
	{
		if ( baudRate == -1)
			baudRate = 115200;
	}
	else
		baudRate = baud;

	int rc = serialPort.init( serialPortDevice, baudRate );

	if( rc == 0) // connected, no error
		saveLocalConfig();

	return rc;
}

int MbsBot::saveLocalConfig(const char *filename)
{
	FILE *hnd = fopen(filename,"w");
	if(hnd)
	{
		fprintf(hnd,"DEVICE %s\n", serialPortDevice);
		fprintf(hnd,"BAUD %d\n", baudRate);
		fclose(hnd);
		return 0;
	}
	return -1;
}

int MbsBot::loadLocalConfig(const char *filename)
{
	FILE *hnd = fopen(filename,"r");
	if(hnd)
	{
		char currentLine[100];
		while(fgets(currentLine, sizeof(currentLine),hnd))
		{
			printf("fgets() = %s\n", currentLine);

			char *tok = strtok(currentLine, " =");
			if(tok && strcmp(tok, "DEVICE") == 0 )
				strcpy(serialPortDevice, strtok(NULL," \n"));
			else if(tok && strcmp(tok, "BAUD") == 0)
				baudRate = atoi(strtok(NULL," "));
		}
		fclose(hnd);
		return 0;
	}
	return -1;
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
	int rc=0;

	// optimization: remember last values and only send command if changed

	static int lastL = 0;
	static int lastR = 0;

	if ((lw != lastL) || (rw != lastR))
	{
		lastL = lw;
		lastR = rw;

		snprintf(cmd, 20, "drv %d %d\n", lw, rw);
		rc = send(cmd);
	}
	return rc;
}

int MbsBot::setProgram(enum ProgramID prg)
{
	char cmd[20];
//	int val=
	snprintf(cmd, 20, "set p %d\n", prg);
	return send(cmd);
}
