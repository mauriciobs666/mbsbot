/**	Copyright (C) 2010-2011 - Mauricio Bieze Stefani
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

#ifndef MBSBOT_H
#define MBSBOT_H

#include <stdarg.h>
#include <MBSUtil.h>
#include "../Protocol.h"

#define SERIAL_BUFFER_SIZE 1000

// baud rate da porta serial
const int spd[] = { 9600, 19200, 38400, 57600, 115200 };
const int n_spd = 5;

class MbsBot
{
	public:
		static MbsBot *instance;
		static MbsBot *getInstance()
		{
			if (!instance)
				instance = new MbsBot();
			return instance;
		}

		MbsBot();
		virtual ~MbsBot()
			{}

		int init(const char *port=NULL, int baud=-1);

		int saveLocalConfig(const char *filename="./mbsbot.cfg");
		int loadLocalConfig(const char *filename="./mbsbot.cfg");

		int getBaud() { return baudRate; }
		char* getPort() { return serialPortDevice; }

		int envia(const char *formato, ...);
		char* receive();

		int enviaVar(const char *var, int valor)
            { return envia("%s %s %d%c", CMD_WRITE, var, valor, CMD_EOL); }
		int pedeVar(const char *var)
            { return envia("%s %s%c", CMD_READ, var, CMD_EOL); }
        int pedeVars()
        {
            int rc = 0;
            // TODO (mbs#1#): pede todas vars
            return rc;
        }

        int save()
            { return envia("%s%c", CMD_SAVE, CMD_EOL); }
        int load()
            { return envia("%s%c", CMD_LOAD, CMD_EOL); }
        int loadDefault()
            { return envia("%s%c", CMD_DEFAULT, CMD_EOL); }
        int status()
            { return envia("%s%c", CMD_STATUS, CMD_EOL); }

        int setProgram(enum ProgramID val)
            { return enviaVar(VAR_PROGRAMA, val); }
		int setLeftWheel(int val)
            { return enviaVar(VAR_RODA_ESQ, val); }
		int setRightWheel(int val)
            { return enviaVar(VAR_RODA_DIR, val); }
		int setPan(int val)
            { return enviaVar(VAR_SERVO_X, val); }
        int setTilt(int val)
            { return enviaVar(VAR_SERVO_Y, val); }
		int setRoll(int val)
            { return enviaVar(VAR_SERVO_Z, val); }
		int setLeftWheelCenter(int val)
            { return enviaVar(VAR_ZERO_ESQ, val); }
		int setRightWheelCenter(int val)
            { return enviaVar(VAR_ZERO_DIR, val); }

        int sqrLeft()
            { return envia("%s\n", CMD_TURN_LEFT); }
        int sqrRight()
            { return envia("%s\n", CMD_TURN_RIGHT); }
        int stop()
            { return envia("%s\n", CMD_MV_STOP); }

		int wheels(int lw, int rw, int duration=0);

		int vectorialDrive(int x, int y, int duration=0);
		void setAccelStep(int step) { accelStep = step; }
		int getAccelStep() { return accelStep; }

	private:
        int send(const char *command, int len=-1);

		SerialPort serialPort;
		char serialPortDevice[100];
		int baudRate;
		char response[SERIAL_BUFFER_SIZE];

		int accelStep;
};

#endif // MBSBOT_H
