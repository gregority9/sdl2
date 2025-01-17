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
#define CS 20

#define SPEED_UP_TIME 150
#define MAGIC_CHANCE 10
#define MAGIC_LIFE 40
#define SHORTEN_SIZE 2

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
    struct snake *before;
};

typedef struct snake Snake;

Snake *head;
Snake *tail;

//GRAPHIC DESIGN START

void setCentersHead(int *CX, int *CY, int dirr){
    const int cs = 0.5 * (GRID_DIM/GRID_SIZE);
    switch(dirr){
        case SNAKE_DOWN:
            *CX += cs; 
            break;
        case SNAKE_LEFT:
            *CX += 2 * cs;  
            *CY += cs;
            break;
        case SNAKE_RIGHT:
            *CY += cs;
            break;
        case SNAKE_UP:
            *CX += cs;
            *CY += 2 * cs;
            break;
    }

    return;
}

void setCentersLeftEye(int *CX, int *CY, int dirr){
    switch(dirr){
        case SNAKE_DOWN:
            *CX += 0.5 * CS;
            *CY += 0.25 * CS; 
            break;
        case SNAKE_LEFT:
            *CX -= 0.25 * CS;  
            *CY += 0.5 * CS;
            break;
        case SNAKE_RIGHT:
            *CX += 0.25 * CS;
            *CY -= 0.5 * CS;
            break;
        case SNAKE_UP:
            *CX -= 0.5 * CS;
            *CY -= 0.25 * CS;
            break;
    }
    return;
}

void setCentersRightEye(int *CX, int *CY, int dirr){
    switch(dirr){
        case SNAKE_DOWN:
            *CX -= CS; 
            break;
        case SNAKE_LEFT:  
            *CY -= CS;
            break;
        case SNAKE_RIGHT:
            *CY += CS;
            break;
        case SNAKE_UP:
            *CX += CS;
            break;
    }
    return;
}

void DrawSemiCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int dirr){
    const int precision = 200;
    for(int i=0; i<=precision; i++){
        double angle = (M_PI * i) / precision; // Kąt w radianach 0-pi
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        SDL_RenderDrawLine(renderer, centerX, centerY, x, y);
    }
    return;
}

void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int dirr){
    const int precision = 400;
    for(int i=0; i<=precision; i++){
        double angle = (2 * M_PI * i) / precision; // Kąt w radianach 0-pi
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        SDL_RenderDrawLine(renderer, centerX, centerY, x, y);
    }
    return;
}

void renderSnakeHead(SDL_Renderer *renderer, int centerX, int centerY, int radius, int dirr){
    setCentersHead(&centerX, &centerY, dirr);
    radius /= 2;
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    DrawSemiCircle(renderer, centerX, centerY, radius, dirr);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    radius /= 5;
    setCentersLeftEye(&centerX, &centerY, dirr);
    DrawCircle(renderer, centerX, centerY, radius, dirr);

    setCentersRightEye(&centerX, &centerY, dirr);
    DrawCircle(renderer, centerX, centerY, radius, dirr);

}

//GRAPHIC DESIGN END

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
    newSnake->before = NULL;

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
    printf("5: %u %u\n", *lastX, *lastY);
    switch(snakeDir){
        case SNAKE_UP:
            (*lastY)--;
            break;
        case SNAKE_DOWN:
            (*lastY)++;
            break;
        case SNAKE_LEFT:
            (*lastX)--;
            break;
        case SNAKE_RIGHT:
            (*lastX)++;
            break;
        default:
            (*lastY)--;
            break;
    }
    printf("6: %u %u\n", *lastX, *lastY);
    return;
}

int possibleRight(int snakeDir, int lastX, int lastY){
    Snake *snake = head;
    snakeDir = (snakeDir+1)%4;
    printf("1: %u %u\n", lastX, lastY);
    setLasts(snakeDir, &lastX, &lastY);
    printf("2: %u %u\n", lastX, lastY);
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

    printf("0: %u %u\n", *lastX, *lastY);
    if(possibleRight(*snakeDir, *lastX, *lastY)){
        printf("3: %u %u\n", *lastX, *lastY);
        *snakeDir = (*snakeDir+1)%4;
        setLasts(*snakeDir, lastX, lastY);
        printf("4: %u %u\n", *lastX, *lastY);
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

pos renderSnake(SDL_Renderer *renderer, int x, int y, int *snakeDir, int *quit){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    int cell_size = GRID_DIM / GRID_SIZE;

    Snake *drawSnake = head;

    SDL_Surface *snakeHead;

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

        //renderSnakeHead(renderer, snakeHead, cell.x, cell.y, (GRID_SIZE/GRID_DIM)/2, *snakeDir);

        SDL_RenderFillRect(renderer, &cell);
        if(drawSnake->next == NULL){
            break;
        }
        else{
            drawSnake = drawSnake->next;
        }
    }

    if(checkCrash(head->x, head->y)){
        *quit = 1;
        return {-1, -1};
    }

    pos lastPos = {lastX, lastY};


    return lastPos;
}

void growSnake(pos lastPos){
    tail->next = (Snake*)malloc(sizeof(Snake));
    tail->next->x = lastPos.x;
    tail->next->y = lastPos.y;
    tail->next->next = NULL;
    tail->next->before = tail;
    tail = tail->next;

    return;
}

void renderApple(SDL_Renderer *renderer, int x, int y, pos *apple, int *points, pos lastPos, int *size){

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    
    int cell_size = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;


    if(head->x == apple->x && head->y == apple->y){
        (*points)++;
        (*size)++;
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

void renderProgressBar(SDL_Renderer *renderer, int x, int y, int w, int h, float fraction){
    fraction = fraction > 1.f ? 1.f : fraction < 0.f ? 0.f : fraction;

    SDL_Rect bg = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &bg);

    int fH = (int)((float)h * fraction);
    int fY = y + (h - fH);

    SDL_Rect fg = {x, fY, w, fH};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
    return;
}

void createMagic(pos *magicApple){
    int over = 0;
    Snake *snake = head;
    while(!over){
        over = 1;
        magicApple->x = rand()%GRID_SIZE;
        magicApple->y = rand()%GRID_SIZE;
        while(1){
            if(snake->x == magicApple->x && snake->y == magicApple->y){
                over = 0;
            }
            if(snake->next == NULL) break;
            else snake = snake->next;
        }
    }
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

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};

void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    // rysowanie linii o d�ugo�ci l w pionie (gdy dx = 0, dy = 1) 
    // b�d� poziomie (gdy dx = 1, dy = 0)
    // draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void renderText(SDL_Surface *screen, SDL_Surface *charset, SDL_Texture *scrtex, SDL_Renderer *renderer, double worldTime, int magicAppleExistence, int gameSpeed, int points){
    //ustawiamy kolorki
    char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
    
    if(magicAppleExistence > 0) sprintf(text, "Czas trwania = %.1lf s   Punkty = %u  MagicApple: pozostało %u s", worldTime, points, magicAppleExistence/(1000/gameSpeed));
    else sprintf(text, "Czas trwania = %.1lf s    Punkty = %u", worldTime, points);

    DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    //SDL_RenderPresent(renderer);

    return;
}

int min(int a, int b){
    if(a<b) return a;
    else return b;
}

void magicBonus(int *gameSpeed, int *size){
    int random = rand()%100;
    if(*size == 1 || random<50){
        *gameSpeed *= 1.2;
    }
    else{
        for(int i=0; i<min(SHORTEN_SIZE, (*size)-1); i++){
            Snake *snake = tail;
            tail = snake->before;
            if(tail != NULL){
                tail->next = NULL;
            }
            free(snake);
        }
        (*size) -= (min(SHORTEN_SIZE, (*size)-1));
    }
}

void renderMagicApple(pos *magicApple, SDL_Renderer *renderer, int x, int y, int *size, pos lastPos, int *magicAppleExistence, int* gameSpeed){

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    int cell_size = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    cell.x = x + magicApple->x * cell_size;
    cell.y = y + magicApple->y * cell_size;

    if(head->x == magicApple->x && head->y == magicApple->y){
        magicBonus(gameSpeed, size);
        *magicAppleExistence = 0;
        return;
    }

    SDL_RenderDrawRect(renderer, &cell);
    SDL_RenderFillRect(renderer, &cell);

    return;
}

void loadGame(int *magicAppleExistence, pos *magicApple, pos *apple, int *timeNow, int *startTime, int *gameSpeed, int *points, int *size, int *snakeDir){
    FILE *load = fopen("save.txt", "r");
    int worldTime;

    
    for(int i=0; i<*size; i++){
        Snake *snake = tail;
        tail = tail->before;
        free(snake->before);
        free(snake->next);
    }

    fscanf(load, "%d %d %d %d %d %d %d %d %d %d", magicAppleExistence, &magicApple->x, &magicApple->y, &apple->x, &apple->y, &worldTime, gameSpeed, points, size, snakeDir);
    pos lastPos;
    fscanf(load, "%d %d", &head->x, &head->y);

    tail = head;
    for(int i=1; i<*size; i++){
        fscanf(load, "%d %d", &lastPos.x, &lastPos.y);
        tail->next = (Snake*)malloc(sizeof(Snake));
        tail->next->x = lastPos.x;
        tail->next->y = lastPos.y;
        tail->next->next = NULL;
        tail->next->before = tail;
        tail = tail->next;
    }

    *startTime = *timeNow - worldTime;
    return;
};

void saveGame(int magicAppleExistence, pos magicApple, pos apple, int worldTime, int gameSpeed, int points, int size, int snakeDir){
    //int magicAppleExistence, pos *magicApple, pos *Apple, Snake *snake (cały), int worldTime, int gameSpeed, int points, int size, int snakeDir
    FILE *save = fopen("save.txt", "w");
    fprintf(save, "%d %d %d %d %d %d %d %d %d %d ", magicAppleExistence, magicApple.x, magicApple.y, apple.x, apple.y, worldTime, gameSpeed, points, size, snakeDir);
    Snake *snake = head;
    for(int i=0; i<size; i++){
        fprintf(save, "%d %d ", snake->x, snake->y);
        snake = snake->next;
    }
    fclose(save);
    return;
};

void saveRecord(SDL_Renderer *renderer, int *points, SDL_Surface *charset, SDL_Surface *screen, SDL_Texture *scrtex){

    char text1[128];
    sprintf(text1, "Zakwalifikowales sie do rankingu, podaj swoja nazwe (do 20 znakow) i zatwierdz klawiszem 'Enter'");
	DrawString(screen, screen->w / 2 - strlen(text1) * 8 / 2, 200, text1, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    FILE *read = fopen("ranking.txt", "r");
    char names[3][20];
    int bestResults[3];

    for(int i=0; i<3; i++){
        fscanf(read, "%s %d", &names[i], &bestResults[i]);
    }
    fclose(read);
    char inputText[21];
    int size = 0;
    bool running = true;

    SDL_StartTextInput();
    SDL_Event e;

    while(running){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_TEXTINPUT){
                if(size<20){
                    inputText[size] = e.text.text[0];
                    size++;
                }
            }
            else if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_BACKSPACE && size>0){
                    size--;
                    inputText[size] = '\0';
                }else if(e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN){
                    running = false;
                    break;
                }
            }
        }
    }
    SDL_StopTextInput();
    
    SDL_RenderClear(renderer);
    DrawString(screen, screen->w / 2 - strlen(inputText) * 8 / 2, 300, inputText, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    FILE* save = fopen("ranking.txt", "w");
    if(*points > bestResults[0]){
        for(int i=0; i<size; i++){
            fprintf(save, "%c", inputText[i]);
        }
        fprintf(save, " %d\n", *points);
    }
    fprintf(save, "%s %d\n", names[0], bestResults[0]);
    if(*points > bestResults[1] && *points < bestResults[0]){
        for(int i=0; i<size; i++){
            fprintf(save, "%c", inputText[i]);
        }
        fprintf(save, " %d\n", *points);
    }
    fprintf(save, "%s %d\n", names[1], bestResults[1]);
    if(*points < bestResults[1]){
        for(int i=0; i<size; i++){
            fprintf(save, "%c", inputText[i]);
        }
        fprintf(save, " %d\n", *points);
    }
    fclose(save);
    return;
};


void showRecords(SDL_Renderer *renderer, int *points, SDL_Surface *charset, SDL_Surface *screen, SDL_Texture *scrtex){
    SDL_RenderClear(renderer);
    FILE *records = fopen("ranking.txt", "r");
    char names[3][20];
    int bestResults[3];

    for(int i=0; i<3; i++){
        fscanf(records, "%s %d", &names[i], &bestResults[i]);
    }

    char text1[128];
    char text2[128];
    char text3[128];

	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
    
    sprintf(text1, "%s:    %d", names[0], bestResults[0]);
    sprintf(text2, "%s:    %d", names[1], bestResults[1]);
    sprintf(text3, "%s:    %d", names[2], bestResults[2]);

    DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 400, czerwony, niebieski);
	DrawString(screen, screen->w / 2 - strlen(text1) * 8 / 2, 20, text1, charset);
	DrawString(screen, screen->w / 2 - strlen(text2) * 8 / 2, 80, text2, charset);
	DrawString(screen, screen->w / 2 - strlen(text3) * 8 / 2, 140, text3, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
    fclose(records);
    if(*points > bestResults[2]){
        saveRecord(renderer, points, charset, screen, scrtex);
    }

    char endMessage[128];
    sprintf(endMessage, "Aby zagrać kolejną grę nacisnij n, w przeciwnym wypadku wyjdz klikjac ESC");
    DrawString(screen, screen->w/ 2 - strlen(endMessage) * 8 / 2, 350, endMessage, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    return;
};



void game(int *running, SDL_Surface *charset, SDL_Surface *screen, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *scrtex){
    SDL_Surface *appleShape, *headShape, *bodyShape, *tailShape;
    int timeStart, timeNow;
    int size = 1;
    SDL_Event event;

    SDL_SetColorKey(charset, true, 0x000000); //ustawiamy, żeby tło było przezroczyste, tzn Snake się pokazywał ponad oknem


    timeStart = SDL_GetTicks(); //bierzemy czas początkowy z procesora w mikrosekundach

    int lastTime = timeStart;
    int quit = 0; //zmienna służąca do zakończenie programu "SNAKE"
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    
    SDL_FillRect(screen, NULL, czarny);  //ustawienie całego ekranu na czarny (NULL znaczy wszystko)
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch); //Updatujemy texture, żeby wyświetlić dane ze zmiennej screen na ekran
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    const int grid_x = (SCREEN_WIDTH / 2) - (GRID_DIM / 2);
    const int grid_y = (SCREEN_HEIGHT / 2) - (GRID_DIM / 2);
    int points = 0;
    int speedUp = 0;
    int GAME_SPEED = 200;
    double worldTime;
    srand(time(0));
    pos app = {0, 0};
    pos *apple = &app;
    pos magicApple = {rand()%GRID_SIZE, rand()%GRID_SIZE};
    apple->x = rand()%GRID_SIZE;
    apple->y = rand()%GRID_SIZE;
    int magicAppleExistence = 0;
    
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
                        case SDLK_s:
                            saveGame(magicAppleExistence, magicApple, *apple, worldTime, GAME_SPEED, points, size, snakeDir);
                            break;
                        case SDLK_l:
                            loadGame(&magicAppleExistence, &magicApple, apple, &timeNow, &timeStart, &GAME_SPEED, &points, &size, &snakeDir);
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            *running = 0;
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
                        case SDLK_n:
                            quit = true;
                            game(running, charset, screen, window, renderer, scrtex);
                            break;
                    }
                    break;
            }
        }

        timeNow = SDL_GetTicks(); //bierzemy aktualny czas z procesroa w mikrosekundach
        worldTime = (timeNow - timeStart) * 0.001;
        pos lastPos;

        //RENDER LOOP START

        if(timeNow-lastTime > GAME_SPEED){
            if(++speedUp == SPEED_UP_TIME){
                GAME_SPEED /= 2;
            }
            SDL_RenderClear(renderer); //czyścimy renderer
            renderText(screen, charset, scrtex, renderer, worldTime, magicAppleExistence, GAME_SPEED, points);
            renderGrid(renderer, grid_x, grid_y);
            lastPos = renderSnake(renderer, grid_x, grid_y, &snakeDir, &quit);
            renderApple(renderer, grid_x, grid_y, apple, &points, lastPos, &size);
            if(magicAppleExistence <= 0){
                if(rand()%MAGIC_CHANCE == 0){
                    magicAppleExistence = MAGIC_LIFE;
                    createMagic(&magicApple);
                }
            }
            else{
                renderProgressBar(renderer, grid_x + GRID_DIM + 2 * (GRID_DIM/GRID_SIZE), grid_y, GRID_DIM/GRID_SIZE, GRID_DIM, (float)((float)magicAppleExistence/(float)MAGIC_LIFE));
                renderMagicApple(&magicApple, renderer, grid_x, grid_y, &size, lastPos, &magicAppleExistence, &GAME_SPEED);
                magicAppleExistence--;
            }
            SDL_Surface *xd;
            renderSnakeHead(renderer, grid_x, grid_y, GRID_DIM/GRID_SIZE, 0);
            lastTime = timeNow;
            SDL_RenderPresent(renderer);
        }   

        //RENDER LOOP END
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Ustawiamy kolor renderowania
        SDL_RenderPresent(renderer);
    }

    showRecords(renderer, &points, charset, screen, scrtex);
}


#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv){
    
    int rc;

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

    int running = 1;
    while(running){
        while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_QUIT:
                        running = 0;
                        break;
                    case SDL_KEYUP:
                        break;
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym){
                            case SDLK_ESCAPE:
                                running = 0;
                                break;
                            case SDLK_n:
                                game(&running, charset, screen, window, renderer, scrtex);
                                break;
                        }
                    break;
                }
        }
    }    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    

    return 0;
}