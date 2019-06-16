
// (c) 2017-2019 MBS - Mauricio Bieze Stefani

#ifndef PLACA_H_INCLUDED
#define PLACA_H_INCLUDED

#define BMP180

#define SERIALX_SPD 115200

#define DFT_DELAY_TRACE 500

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
/**
    Proto board Arduino Mega 2560


*/
    #define CARTAO_SD
    #define CARTAO_SD_PINO_SS 53
    #define SERIALX Serial2
//    #define SERIALX Serial
    #define DEBUG 1
    #define MONITOR_BATERIA
#else
    #define SERIALX Serial
#endif

#endif // PLACA_H_INCLUDED
