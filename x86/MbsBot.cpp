/**	Copyright (C) 2010-2013 - Mauricio Bieze Stefani
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

MbsBot *MbsBot::instancia = 0;

MbsBot::MbsBot()
{
	memset(serialPortDevice, 0, sizeof(serialPortDevice));
	sensores = std::vector<int>(16);
}

int MbsBot::init(const char *port, int baud)
{
	carregaConfig();

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
		salvaConfig();
	}
	else
		TRACE_ERROR("Error %d while trying to open %s\n", rc, serialPortDevice);

	return rc;
}

int MbsBot::salvaConfig(const char *filename)
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

int MbsBot::carregaConfig(const char *filename)
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
                int s = 0;
                while( (tok = strtok(NULL, " ")) )
                {
                    sensores[s] = atoi(tok);
                    s++;
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
                                        rodaEsquerdaTraseira = atoi(tok);
                                        if ( (tok = strtok(NULL, " ")) )
                                        {
                                            rodaDireitaTraseira = atoi(tok);
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
                }
            }
            else if(strcmp(tok, VAR_T_RF) == 0)
            {
                if ( (tok = strtok(NULL, " ")) )
                    tempoRF = atoi(tok);
            }
            else if(strcmp(tok, VAR_PID) == 0)
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

int MbsBot::enviaJoystick(unsigned short bt, unsigned short x, unsigned short  y, unsigned short z, unsigned short r)
{
	int rc=0;

	// otimizacao pra naum mandar repetido

	static unsigned short btAntigo = 0;
	static unsigned short xAntigo = 0;
	static unsigned short yAntigo = 0;
	static unsigned short zAntigo = 0;
	static unsigned short rAntigo = 0;

	if ((bt != btAntigo) || (x != xAntigo) || (y != yAntigo) || (z != zAntigo) || (r != rAntigo))
	{
        btAntigo = bt;
        xAntigo = x;
        yAntigo = y;
        zAntigo = z;
        rAntigo = r;

		rc = envia("%s %d %d %d %d %d%c", CMD_JOYPAD, bt, x, y, z, r, CMD_EOL);
	}
	return rc;
}
