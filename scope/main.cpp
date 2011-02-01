#include <stdio.h>
#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480
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

    int y = HEIGHT/2;
    int x = 0;

    while(go)
    {
        if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

        // clear current column
        for (int yy=0; yy<HEIGHT; yy++)
            setpixel(screen, x, yy, 0, 0, 0);

        //
        setpixel(screen, x, y);

        (rand() % 2) ? y++ : y--;
        if(y>=HEIGHT) y=HEIGHT-1;
        if(y<0) y=0;

        x++;
        if(x>=WIDTH) x=0;

        for (int yy=0; yy<HEIGHT; yy++)
            setpixel(screen, x, yy, 255, 255, 255);

        if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

        SDL_Flip(screen);

        SDL_Delay(1);

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
