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

#ifndef MBSBOT_H
#define MBSBOT_H

#include <stdarg.h>
#include <MBSUtil.h>
#include "../protocolo.h"
#include <vector>

#define SERIAL_BUFFER_SIZE 1000

// baud rate da porta serial
const int spd[] = { 9600, 19200, 38400, 57600, 115200 };
const int n_spd = 5;

class MbsBot
{
	public:
		static MbsBot *instancia;
		static MbsBot *getInstance()
		{
			if (!instancia)
				instancia = new MbsBot();
			return instancia;
		}

		MbsBot();
		virtual ~MbsBot() {}

		int init(const char *port=NULL, int baud=-1);
		void desconecta() { serialPort.closePort(); }
		int salvaConfig(const char *filename="./mbsbot.cfg");
		int carregaConfig(const char *filename="./mbsbot.cfg");

		int getBaud() { return baudRate; }
		char* getPorta() { return serialPortDevice; }

		int envia(const char *formato, ...);
		char* recebe();

		int enviaVar(const char *var, int valor)
            { return envia("%s %s %d%c", CMD_SET, var, valor, CMD_EOL); }
		int pedeVar(const char *var)
            { return envia("%s %s%c", CMD_GET, var, CMD_EOL); }
        void pedeVars()
        {
            // TODO (mbs#1#): pede todas vars

            pedeVar(VAR_PROGRAMA);
            pedeVar(VAR_T_RF);
            pedeVar(VAR_PID);

            pedeVar(VAR_RODA_ESQ);
            pedeVar(VAR_RODA_DIR);
            pedeVar(VAR_SERVO_X);
            pedeVar(VAR_SERVO_Y);
            pedeVar(VAR_SERVO_Z);

            pedeVar(VAR_AS);

            status();
        }

        int salva()
            { return envia("%s%c", CMD_GRAVA, CMD_EOL); }
        int carrega()
            { return envia("%s%c", CMD_CARREGA, CMD_EOL); }
        int carregaDefaults()
            { return envia("%s%c", CMD_DEFAULT, CMD_EOL); }
        int status()
            { return envia("%s%c", CMD_STATUS, CMD_EOL); }

        int setPrograma(enum ProgramID val)
            { return enviaVar(VAR_PROGRAMA, val); }
		int setRodaEsquerda(int val)
            { return enviaVar(VAR_RODA_ESQ, val); }
		int setRodaDireita(int val)
            { return enviaVar(VAR_RODA_DIR, val); }
		int setPan(int val)
            { return enviaVar(VAR_SERVO_X, val); }
        int setTilt(int val)
            { return enviaVar(VAR_SERVO_Y, val); }
		int setRoll(int val)
            { return enviaVar(VAR_SERVO_Z, val); }
		int setCentroRodaEsquerda(int val)
            { return enviaVar(VAR_ZERO_ESQ, val); }
		int setCentroRodaDireita(int val)
            { return enviaVar(VAR_ZERO_DIR, val); }
        int stop()
            { return envia("%s%c", CMD_MV_PARAR, CMD_EOL); }

		int enviaJoystick(unsigned short bt, unsigned short x, unsigned short  y, unsigned short z, unsigned short r);

        int getPrograma() { return programa; }
		int getErro() { return erro; }
		int getFreioMao() { return freioMao; }
		int getRodaEsquerda() { return rodaEsquerda; }
		int getRodaDireita() { return rodaDireita; }
		int getRodaEsquerdaT() { return rodaEsquerdaTraseira; }
		int getRodaDireitaT() { return rodaDireitaTraseira; }
		int getServoPan() { return servoPan; }
		int getServoTilt() { return servoTilt; }
		int getServoRoll() { return servoRoll; }
		int getSensor(int i) { return sensores[i]; }
		int getTempoRF() { return tempoRF; }
		int getPidKP() { return pidKP; }
		int getPidKI() { return pidKI; }
		int getPidKD() { return pidKD; }
	private:
        int send(const char *command, int len=-1);
        char* receive();

		SerialPort serialPort;
		char serialPortDevice[100];
		int baudRate;
		char resposta[SERIAL_BUFFER_SIZE];

		//variaveis remotas
		int programa;
		int erro;
		int freioMao;
		int rodaEsquerda;
		int rodaDireita;
		int rodaEsquerdaTraseira;
		int rodaDireitaTraseira;
		int servoPan;
		int servoTilt;
		int servoRoll;
		std::vector<int> sensores;
		int tempoRF;
		int pidKP;
		int pidKI;
		int pidKD;
};

#endif // MBSBOT_H
