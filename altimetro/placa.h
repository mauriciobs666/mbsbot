
// (c) 2017-2019 MBS - Mauricio Bieze Stefani


/*
https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
*/

#ifndef PLACA_H_INCLUDED
#define PLACA_H_INCLUDED

#define DEBUG 1

#define SERIALX Serial
#define SERIALX_SPD 115200

#define DFT_DELAY_TRACE 500

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
/**
    Arduino Mega 2560 - caixa preta

    Sensor BMP280 pinos  (SDA) e  (SCL)
    Piezo pinos 11 e 12
*/
    #define BMP180
    #define BMP280
    #undef SERIALX
    #define SERIALX Serial1
    #define PINO_MONITOR_BATERIA A1
    #define PINO_ENERGIA_BLUETOOTH 49
    #define PINO_BOTAO_POWER    13
    #define CARTAO_SD
    #define CARTAO_SD_PINO_SS 53

#elif defined( ARDUINO_ARCH_ESP32 )
    //#define BMP280
#else
/**
    Proto board Arduino Pro Micro 32u4

    https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
    https://dl.espressif.com/dl/package_esp32_index.json

    Sensor BMP180 pinos 2 (SDA) e 3 (SCL)
    Piezo pinos 9 e 10
*/
    #define BMP180

    #ifdef DEBUG
        #define PINO_MONITOR_BATERIA A0
        #define PINO_ENERGIA_BLUETOOTH 4
//        #define PINO_ESTADO_BLUETOOTH
        #define PINO_BOTAO_POWER    15
        #define PINO_BOTAO_PROX     14
        #define PINO_BOTAO_OK       16
        #define PINO_LED_R 5
        #define PINO_LED_G 6
        #define PINO_LED_B 7
    #endif
#endif

#ifndef DEBUG

    // PRODUCAO !

    #define THRESHOLD_DECOLAGEM    6.0
    #define THRESHOLD_QUEDA     -130.0
    #define THRESHOLD_ABERTURA   -80.0
    #define THRESHOLD_SUBTERMINAL -7.0
    #define THRESHOLD_POUSO       -1.0

    #define DEBOUNCE_POUSO       10000
    #define DEBOUNCE_SUBTERMINAL 15000
    #define DEBOUNCE_VEL_MAX_NAV 10000

    #define AVISO_SUBIDA_CINTO  1500
    #define AVISO_SUBIDA_CHECK 12000

    #define AVISO_ALTA_1    6500
    #define AVISO_ALTA_2    5500
    #define AVISO_ALTA_3    4500
    #define ALARME_ALTA     3500

    #define AVISO_NAVEGACAO_A      300
    #define AVISO_NAVEGACAO_B      600
    #define AVISO_NAVEGACAO_C      900
    #define AVISO_NAVEGACAO_D     1200

    #define VOLUME_QUEDA        10
    #define VOLUME_NAVEGACAO    10

    #define TRACE 0

#else

    // DEBUG

    #define THRESHOLD_DECOLAGEM    0.8
    #define THRESHOLD_QUEDA       -0.8
    #define THRESHOLD_ABERTURA     0.8
    #define THRESHOLD_SUBTERMINAL -3.0
    #define THRESHOLD_POUSO       -0.5

    #define DEBOUNCE_POUSO       10000
    #define DEBOUNCE_SUBTERMINAL 3000
    #define DEBOUNCE_VEL_MAX_NAV 10000

    #define AVISO_SUBIDA_CINTO  6
    #define AVISO_SUBIDA_CHECK 12

    #define AVISO_ALTA_1       9
    #define AVISO_ALTA_2       6
    #define AVISO_ALTA_3       3
    #define ALARME_ALTA        5

    #define AVISO_NAVEGACAO_A        6
    #define AVISO_NAVEGACAO_B       12
    #define AVISO_NAVEGACAO_C       24
    #define AVISO_NAVEGACAO_D       36

    #define VOLUME_QUEDA        3
    #define VOLUME_NAVEGACAO    3

    #define TRACE ( TRACE_MASTER_EN | TRACE_SENSOR | TRACE_ALTURA | TRACE_VELOCIDADE )

#endif

#endif // PLACA_H_INCLUDED
