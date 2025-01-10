#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>

extern "C"{
    #include "SDL2-2.0.10/include/SDL.h"
    #include "SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv){
    
    int rc;
    int timeStart, timeNow;

    SDL_Surface *screen, *charset, *eti;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *scrtex;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL_Init erro: %s\n", SDL_GetError());
        return 1;
    }

    rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_SetWindowTitle(window, "Snake");

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_ShowCursor(SDL_DISABLE);

    charset = SDL_LoadBMP("./cs8x8.bmp");
    if(charset == NULL){
        //obsługa błedu nie włączania się obrazka, idk po co w sumie mi to
    }
    SDL_SetColorKey(charset, true, 0x000000);

    eti = SDL_LoadBMP("eti.bmp");

    //tu też obsługa błedu, bez sensu

    char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

    t


}