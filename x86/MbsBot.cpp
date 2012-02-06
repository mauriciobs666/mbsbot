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

#include "MbsBot.h"

MbsBot *MbsBot::instance = 0;

MbsBot::MbsBot()
{
	memset(serialPortDevice, 0, sizeof(serialPortDevice));
	sensores = std::vector<int>(6);
}

int MbsBot::init(const char *port, int baud)
{
	loadLocalConfig();

	if(port == NULL)
	{
		if(strcmp(serialPortDevice, "") == 0)
		{
			// use predefined value
			#ifdef _WIN32
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
	{
		TRACE_INFO("Opened %s @ %d bps\n", serialPortDevice, baudRate);
		saveLocalConfig();
	}
	else
		TRACE_ERROR("Error %d while trying to open %s\n", rc, serialPortDevice);

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
			//printf("fgets() = %s\n", currentLine);
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

int MbsBot::envia(const char *formato, ...)
{
    char cmd[MAX_CMD];
    va_list args;

    va_start(args, formato);
    vsprintf(cmd, formato, args);
    va_end(args);

    return send(cmd);
}

char * MbsBot::recebe()
{
    char *rx = receive();
    if(rx)
    {
        char temp[SERIAL_BUFFER_SIZE];
        memcpy(temp, rx, SERIAL_BUFFER_SIZE);

        char * tok = strtok(temp, " ");
        if (tok)
        {
            if(strcmp(tok, VAR_RODA_ESQ) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    rodaEsquerda = atoi(tok);
            }
            else if(strcmp(tok, VAR_RODA_DIR) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    rodaDireita = atoi(tok);
            }
            else if(strcmp(tok, VAR_SERVO_X) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    servoPan = atoi(tok);
            }
            else if(strcmp(tok, VAR_SERVO_Y) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    servoTilt = atoi(tok);
            }
            else if(strcmp(tok, VAR_SERVO_Z) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    servoRoll = atoi(tok);
            }
            else if(strcmp(tok, VAR_AS) == 0)	// todos sensores analogicos
            {
                for(int s = 0; s < 6; s++)
                {
                    if ( (tok = strtok(NULL, " ")) )
                        sensores[s] = atoi(tok);
                    else
                        break;
                }
            }
            else if(strcmp(tok, CMD_STATUS) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                {
                    programa = atoi(tok);
                    if ( (tok = strtok(NULL, " ")) )
                    {
                        erro = atoi(tok);
                        if ( (tok = strtok(NULL, " ")) )
                        {
                            freioMao = atoi(tok);
                            if ( (tok = strtok(NULL, " ")) )
                            {
                                rodaEsquerda = atoi(tok);
                                if ( (tok = strtok(NULL, " ")) )
                                {
                                    rodaDireita = atoi(tok);
                                    if ( (tok = strtok(NULL, " ")) )
                                    {
                                        servoPan = atoi(tok);
                                        if ( (tok = strtok(NULL, " ")) )
                                        {
                                            servoTilt = atoi(tok);
                                            if ( (tok = strtok(NULL, " ")) )
                                            {
                                                servoRoll = atoi(tok);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(strcmp(tok, VAR_T_POL) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    tempoPol = atoi(tok);
            }
            else if(strcmp(tok, VAR_T_90) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    tempo90 = atoi(tok);
            }
            else if(strcmp(tok, VAR_T_RF) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    tempoRF = atoi(tok);
            }
            else if(strcmp(tok, "PID") == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    pidKP = atoi(tok);
                if ( (tok = strtok(NULL, " ")) )
                    pidKI = atoi(tok);
                if ( (tok = strtok(NULL, " ")) )
                    pidKD = atoi(tok);
            }
        }
        return resposta;
    }
    return NULL;
}

int MbsBot::send(const char * command, int len)
{
	return serialPort.Write(command, (len > 0 ? len : strlen(command)) );
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
		else if(c == CMD_EOL)
		{
			resposta[pos]=0;
			pos=0;
			return resposta;
		}
		else
		{
			resposta[pos]=c;
			pos++;
		}
	}
	return NULL;
}

int MbsBot::wheels(int lw, int rw, int duration)
{
	char cmd[MAX_CMD];
	int rc=0;

	// otimizacao pra naum mandar repetido
	static int lastL = 0;
	static int lastR = 0;

	if ((lw != lastL) || (rw != lastR) || (duration>0))
	{
		lastL = lw;
		lastR = rw;

        if(duration > 0)
            snprintf(cmd, MAX_CMD, "%s %d %d %d\n", CMD_MV_WHEELS, lw, rw, duration);
		else
            snprintf(cmd, MAX_CMD, "%s %d %d\n", CMD_MV_WHEELS, lw, rw);

		rc = send(cmd);
	}
	return rc;
}

int MbsBot::vectorialDrive(int x, int y, int duration)
{
    // x e y sao % potencia, duracao em ms

	char cmd[MAX_CMD];
	int rc=0;

	// otimizacao pra naum mandar repetido
	static int lastX = 0;
	static int lastY = 0;

	if ((x != lastX) || (y != lastY) || (duration>0))
	{
		lastX = x;
		lastY = y;

        if(duration > 0)
            snprintf(cmd, MAX_CMD, "%s %d %d %d\n", CMD_MV_VECT, x, y, duration);
		else
            snprintf(cmd, MAX_CMD, "%s %d %d\n", CMD_MV_VECT, x, y);

		rc = send(cmd);
	}
	return rc;
}
