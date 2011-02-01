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

using namespace std;
using namespace boost;

#ifdef _WIN32
    #define DEFAULT_SERIAL_DEVICE "COM1"
#else
	#define DEFAULT_SERIAL_DEVICE "/dev/ttyUSB0"
#endif
std::string serialPortDevice(DEFAULT_SERIAL_DEVICE);

#define DEFAULT_SERIAL_BAUD 115200
int baudRate = DEFAULT_SERIAL_BAUD;

asio::io_service io;
asio::serial_port mcuPort(io);

#define WIDTH 1000
#define HEIGHT 256
#define BPP 4
#define DEPTH 32

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r=0, Uint8 g=255, Uint8 b=0)
{
    Uint32 cor = SDL_MapRGB( screen->format, r, g, b );
    Uint32 *pixmem32 = (Uint32*) screen->pixels  + y * screen->pitch / BPP + x;
    *pixmem32 = cor;
}

int main(int argc, char* argv[])
{
    SDL_Surface *screen;
    SDL_Event event;

    bool go = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE))) //SDL_FULLSCREEN|
    {
        SDL_Quit();
        return 1;
    }

    try // open serial port
    {
        mcuPort.open(serialPortDevice);
        mcuPort.set_option(asio::serial_port_base::baud_rate(baudRate));
        TRACE_INFO("Opened %s @ %d bps", serialPortDevice.c_str(), baudRate);
    }
    catch(boost::system::system_error &e)
    {
        TRACE_ERROR("Error while trying to open %s: \"%s\"", serialPortDevice.c_str(), e.what());
        exit(-2);
    }

    int y = HEIGHT/2;
    int x = 0;
    unsigned char data;

    while(go)
    {
        if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

        // clear current column
        for (int yy=0; yy<HEIGHT; yy++)
            setpixel(screen, x, yy, 0, 0, 0);

/*
        (rand() % 2) ? y++ : y--;
        if(y>=HEIGHT) y=HEIGHT-1;
        if(y<0) y=0;
        setpixel(screen, x, y);
*/

        mcuPort.read_some(asio::buffer(&data,1));
        setpixel(screen, x, data);

        x++;
        if(x>=WIDTH) x=0;

        for (int yy=0; yy<HEIGHT; yy++)
            setpixel(screen, x, yy, 255, 255, 255);

        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

        SDL_Flip(screen);

        //SDL_Delay(1);

        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                go = false;
                break;
            case SDL_KEYDOWN:
                go = false;
                break;
            }
        }
    }

    SDL_Quit();
    return 0;
}
