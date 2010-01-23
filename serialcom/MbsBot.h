#ifndef MBSBOT_H
#define MBSBOT_H

#include <MBSUtil.h>

#define SERIAL_BUFFER_SIZE 100
#define COMMAND_END '\n'

class MbsBot
{
	public:
		MbsBot();
		virtual ~MbsBot();

		int send(const char * command, int len=-1);
		char * receive();

		int setLeftWheel(int val);
		int setRightWheel(int val);

		int setLeftWheelCenter(int val);
		int setRightWheelCenter(int val);

		SerialPort serialPort;
	protected:
	private:
		char response[SERIAL_BUFFER_SIZE];
};

#endif // MBSBOT_H
