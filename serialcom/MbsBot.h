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

#ifndef MBSBOT_H
#define MBSBOT_H

#include <MBSUtil.h>

#define SERIAL_BUFFER_SIZE 100
#define COMMAND_END '\n'

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

		MbsBot()
			{}
		virtual ~MbsBot()
			{}

		int init(const char *port=NULL, int baud=-1);

		int send(const char * command, int len=-1);
		char * receive();

		int setLeftWheel(int val);
		int setRightWheel(int val);

		int setLeftWheelCenter(int val);
		int setRightWheelCenter(int val);

	private:
		SerialPort serialPort;
		char response[SERIAL_BUFFER_SIZE];
};

#endif // MBSBOT_H
