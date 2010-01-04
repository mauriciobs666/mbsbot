#include <iostream>
#include <MBSUtil.h>
#include <stdio.h>

using namespace std;

SerialPort s;

int main(int argc, char *argv[])
{
	bool getTheFuckOutOfHere = false;

	if(1 == argc)
	{
		printf("Usage: serialcom <port>\n");
		getTheFuckOutOfHere = true;
	}
	else
	{
		s.init(argv[1]);
	}

	char command[100];
	char response[100];

	while ( ! getTheFuckOutOfHere )
	{
		gets(command);

		if(strncmp(command,"exit",4)==0)
			getTheFuckOutOfHere = true;
		else
			s.Write(command, strlen(command));

		int available;
		do
		{
			available = s.Read(response, sizeof(response));
			if(available > 0)
				printf("%s", response);
		}
		while(available>0);
	}
    return 0;
}
