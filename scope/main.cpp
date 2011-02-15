/**	Copyright (C) 2011 - Mauricio Bieze Stefani
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

#include <SDL.h>
#include <MBSUtil.h>

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
using namespace boost;

#ifdef _WIN32
std::string serialPortDevice("COM1");
#else
std::string serialPortDevice("/dev/ttyUSB0");
#endif

int baudRate = 115200;

#define RANDOM_TST

// Boost
asio::io_service io;
asio::serial_port mcuPort(io);

// SDL
SDL_Surface *screen;
#define WIDTH 1000
#define HEIGHT 256

inline void setpixel(Uint32 x, Uint32 y, Uint32 cor)
{
    // 4 B/pixel
    * ((Uint32*) screen->pixels + y * screen->pitch / 4 + x) = cor;
}

inline void setpixel(Uint32 x, Uint32 y, Uint8 r=0, Uint8 g=255, Uint8 b=0)
{
    setpixel( x, y, SDL_MapRGB( screen->format, r, g, b ) );
}

int main(int argc, char* argv[])
{
    SDL_Event event;

    bool sair = false;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE))) //SDL_FULLSCREEN|
    {
        SDL_Quit();
        return 1;
    }

    const Uint32 VERDE = SDL_MapRGB( screen->format, 0, 255, 0 );
    const Uint32 BRANCO = SDL_MapRGB( screen->format, 255, 255, 255 );
    const Uint32 PRETO = SDL_MapRGB( screen->format, 0, 0, 0 );

    #ifndef RANDOM_TST
    try // abre serial port
    {
        mcuPort.open(serialPortDevice);
        mcuPort.set_option(asio::serial_port_base::baud_rate(baudRate));
        TRACE_INFO("Aberto: %s @ %d bps", serialPortDevice.c_str(), baudRate);
    }
    catch(boost::system::system_error &e)
    {
        TRACE_ERROR("%s: %s", e.what(), serialPortDevice.c_str());
        exit(-2);
    }
    #endif

    int y = HEIGHT/2;
    int x = 0;

    while( !sair )
    {
        if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

        // limpa coluna
        for (int y=0; y < HEIGHT; y++) setpixel(x, y, PRETO);

        #ifndef RANDOM_TST

        // le da serial
        unsigned char data;
        mcuPort.read_some(asio::buffer(&data,1));
        y = HEIGHT-1 - data;

        #else

        // random
        (rand() % 2) ? ( y < HEIGHT-1 ? ++y : y ) : ( y > 0 ? --y : y );
        //SDL_Delay(1);

        #endif

        setpixel( x, y, VERDE );

        x++;
        if( x >= WIDTH ) x=0;

        // linha branca
        for (int y=0; y<HEIGHT; y++) setpixel(x, y, BRANCO);

        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

        //if(x==0)
        //    SDL_Flip(screen);

        SDL_UpdateRect(screen, ( x>0 ? x-1 : 0 ), 0, ( x<WIDTH-1 ? 2 : 1 ), HEIGHT-1);

        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            case SDL_QUIT:
                sair = true;
                break;
            }
        }
    }

    SDL_Quit();
    return 0;
}
