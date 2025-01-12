#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <ctime>

extern "C"{
    #include"SDL2-2.0.10/include/SDL.h"
    #include"SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define GRID_SIZE 20
#define GRID_DIM 800

typedef struct{
    int x, y;
}pos;

enum{
    SNAKE_UP,
    SNAKE_RIGHT,
    SNAKE_DOWN,
    SNAKE_LEFT,
};

struct snake {
    int x;
    int y;

    struct snake *next;
};

typedef struct snake Snake;

Snake *head;
Snake *tail;

void swap(int *a, int *b){
    int c = *a;
    *a = *b;
    *b = c;
    return;
}

void initSnake(){
    Snake *newSnake = (Snake*)malloc(sizeof(Snake));

    if(newSnake == NULL){
        printf("MEMORY Allocation failed ;////");
        return;
    }

    newSnake->x = GRID_SIZE / 2;
    newSnake->y = GRID_SIZE / 2;
    newSnake->next = NULL;

    head = newSnake;
    tail = newSnake;


    return;
}

void renderGrid(SDL_Renderer *renderer, int x, int y){

    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
    
    int cell_size = GRID_DIM / GRID_SIZE;

    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    for(int i=0; i<GRID_SIZE; i++){
        for(int j=0; j<GRID_SIZE; j++){
            cell.x = i*cell_size + x;
            cell.y = j*cell_size + y;
            SDL_RenderDrawRect(renderer, &cell);
        }
    }

    return;
}

void setLasts(int snakeDir, int *lastX, int *lastY){
    switch(snakeDir){
        case SNAKE_UP:
            *lastY--;
            break;
        case SNAKE_DOWN:
            *lastY++;
            break;
        case SNAKE_LEFT:
            *lastX--;
            break;
        case SNAKE_RIGHT:
            *lastX++;
            break;
        default:
            *lastY--;
            break;
    }
    return;
}

int possibleRight(int snakeDir, int lastX, int lastY){
    Snake *snake = head;
    snakeDir = (snakeDir+1)%4;
    setLasts(snakeDir, &lastX, &lastY);
    if(snake->next != NULL) snake = snake->next;
    else return 1;
    while(1){
        if(snake->x == lastX && snake->y == lastY){
            return 0;
        }
        if(snake->next == NULL){
            break;
        }
        else{
            snake = snake->next;
        }
    }
    return 1;
}

void checkOutside(int *snakeDir, int *lastX, int *lastY){
    printf("first: %u\n", *snakeDir);
    printf("w, %u %u\n", *lastX, *lastY);

    if(*lastX < 0) (*lastX)++;
    else if(*lastX >= GRID_SIZE) (*lastX)--;
    else if(*lastY < 0) (*lastY)++;
    else if(*lastY >= GRID_SIZE) (*lastY)--;
    else return;

    printf("ez, %u %u\n", *lastX, *lastY);

    if(possibleRight(*snakeDir, *lastX, *lastY)){
        *snakeDir = (*snakeDir+1)%4;
        setLasts(*snakeDir, lastX, lastY);
    }
    else{
        *snakeDir = (*snakeDir-1)%4;
        setLasts(*snakeDir, lastX, lastY);
    }

    printf("snakedir: %u\n", *snakeDir);
    return;
}

int checkCrash(int lastX, int lastY){

    Snake *snake = head;

    if(snake->next != NULL) snake = snake->next;
    else return 0;
    while(1){
        if(snake->x == lastX && snake->y == lastY){
            return 1;
        }
        if(snake->next == NULL){
            break;
        }
        else{
            snake = snake->next;
        }
    }

    return 0;
}


pos renderSnake(SDL_Renderer *renderer, int x, int y, int *snakeDir){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    int cell_size = GRID_DIM / GRID_SIZE;

    Snake *drawSnake = head;

    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    int lastX = drawSnake->x;
    int lastY = drawSnake->y;

    switch(*snakeDir){
        case SNAKE_UP:
            lastY--;
            break;
        case SNAKE_DOWN:
            lastY++;
            break;
        case SNAKE_LEFT:
            lastX--;
            break;
        case SNAKE_RIGHT:
            lastX++;
            break;
        default:
            lastY--;
            break;
    }
    checkOutside(snakeDir, &lastX, &lastY);

    while(1){
        swap(&drawSnake->x, &lastX);
        swap(&drawSnake->y, &lastY);

        cell.x = x + drawSnake->x*cell_size;
        cell.y = y + drawSnake->y*cell_size;

        SDL_RenderFillRect(renderer, &cell);
        if(drawSnake->next == NULL){
            break;
        }
        else{
            drawSnake = drawSnake->next;
        }
    }

    if(checkCrash(head->x, head->y)){
        printf("crash");
    }

    pos lastPos = {lastX, lastY};


    return lastPos;
}

void growSnake(pos lastPos){
    tail->next = (Snake*)malloc(sizeof(Snake));
    tail->next->x = lastPos.x;
    tail->next->y = lastPos.y;
    tail->next->next = NULL;
    tail = tail->next;

    return;
}

void renderApple(SDL_Renderer *renderer, int x, int y, pos *apple, int *points, pos lastPos){

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    int cell_size = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;


    if(head->x == apple->x && head->y == apple->y){
        *points++;
        growSnake(lastPos);
        int over = 0;
        Snake *snake = head;
        while(!over){
            over = 1;
            apple->x = rand()%GRID_SIZE;
            apple->y = rand()%GRID_SIZE;
            while(1){
                if(snake->x == apple->x && snake->y == apple->y){
                    over = 0;
                }
                if(snake->next == NULL) break;
                else snake = snake->next;
            }
        }
    }

    cell.x = x + apple->x * cell_size;
    cell.y = y + apple->y * cell_size;

    SDL_RenderDrawRect(renderer, &cell);
    SDL_RenderFillRect(renderer, &cell);

    return;
}

void Draw_Surface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y){
    SDL_Rect dest; //ustawiamy miejsce rysowania
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest); //[co rysujemy/Ile rysujemy z tego (null-wszystko)/gdzie rysujemy(screen)/koordynaty]
}

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv){
    
    int rc;
    int timeStart, timeNow;

    SDL_Surface *screen, *charset, *eti;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *scrtex;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer); //Tworzymy okno i renderer

    if(rc != 0) { //obsługa błedu tworzenia zmiennej rc
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); //Dajemy podpowiedź jak się ma renderować obraz
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT); //Ustawiamy wielkość rendera
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Ustawiamy kolor renderowania

    SDL_SetWindowTitle(window, "Snake"); //Ustawiamy tytuł okna

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000); //Ustawiamy kolorki

    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT); //Ustawiamy teksture, czyli jak ma się aktualizować okno (stałe/dynamiczne itp)

    SDL_ShowCursor(SDL_DISABLE); //Wyłączamy pokazywanie się kursora myszy

    charset = SDL_LoadBMP("./cs8x8.bmp"); //Ładujemy obraz planszy
    if(charset == NULL){ //obsługa błędu jak się fotka nie załaduje
        printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
    }
    SDL_SetColorKey(charset, true, 0x000000); //ustawiamy, żeby tło było przezroczyste, tzn Snake się pokazywał ponad oknem

    eti = SDL_LoadBMP("./eti.bmp"); //Ładujemy "eti"
	if(eti == NULL) { //obsługa błedów ładowania obrazu
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

    //ustawiamy kolorki
    char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

    timeStart = SDL_GetTicks(); //bierzemy czas początkowy z procesora w mikrosekundach

    int lastTime = timeStart;
    int quit = 0; //zmienna służąca do zakończenie programu "SNAKE"
    
    SDL_FillRect(screen, NULL, czarny);  //ustawienie całego ekranu na czarny (NULL znaczy wszystko)
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch); //Updatujemy texture, żeby wyświetlić dane ze zmiennej screen na ekran
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    int grid_x = (SCREEN_WIDTH / 2) - (GRID_DIM / 2);
    int grid_y = (SCREEN_HEIGHT / 2) - (GRID_DIM / 2);
    int *points = 0;
    srand(time(0));
    pos app = {0, 0};
    pos *apple = &app;
    apple->x = rand()%GRID_SIZE;
    apple->y = rand()%GRID_SIZE;
    
    initSnake();
    int snakeDir = SNAKE_UP;

    while(!quit){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        
                        case SDLK_UP:
                            if(snakeDir != SNAKE_DOWN) snakeDir = SNAKE_UP;
                            break;
                        
                        case SDLK_DOWN:
                            if(snakeDir != SNAKE_UP) snakeDir = SNAKE_DOWN;
                            break;
                        
                        case SDLK_RIGHT:
                            if(snakeDir != SNAKE_LEFT) snakeDir = SNAKE_RIGHT;
                            break;
                        
                        case SDLK_LEFT:
                            if(snakeDir != SNAKE_RIGHT) snakeDir = SNAKE_LEFT;
                            break;
                    }
                    break;
            }
        }


        timeNow = SDL_GetTicks(); //bierzemy aktualny czas z procesroa w mikrosekundach

        pos lastPos;

        //RENDER LOOP START

        if(timeNow-lastTime > 200){
            SDL_RenderClear(renderer); //czyścimy renderer
            renderGrid(renderer, grid_x, grid_y);
            lastPos = renderSnake(renderer, grid_x, grid_y, &snakeDir);
            renderApple(renderer, grid_x, grid_y, apple, points, lastPos);
            lastTime = timeNow;
        }   

        //RENDER LOOP END
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Ustawiamy kolor renderowania
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}