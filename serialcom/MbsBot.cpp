#include "MbsBot.h"

MbsBot::MbsBot()
{
	//ctor
}

MbsBot::~MbsBot()
{
	//dtor
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
