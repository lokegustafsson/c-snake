#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/timeb.h>
#include "circular_buffer.h"

const uint32_t SCREEN_WIDTH      = 640;
const uint32_t SCREEN_HEIGHT     = 640;
const uint32_t CELL_MARGIN_RATIO = 5;
const uint32_t TICK_MILLISECONDS = 100;
const uint32_t COLOR_MARGIN      = 0xFFFFFF;
const uint32_t COLOR_EMPTY       = 0xDDDDDD;
const uint32_t COLOR_SNAKE       = 0x111111;
const uint32_t COLOR_APPLE       = 0xEE1111;
#define GRID_SIZE 17
#define GRID_COUNT (GRID_SIZE * GRID_SIZE)

// Types
typedef enum {
    EMPTY, APPLE, SNAKE
} CellState;

typedef enum {
    RIGHT, LEFT, UP, DOWN
} Direction;

typedef struct {
    CellState     grid[GRID_COUNT];
    cbuf_handle_t snake;
    Direction     headDir;
    Direction     lastDir;
    uint16_t      apple;
    uint16_t      capacity;
} GameState;

// Forward function declarations to allow game state functions calling each other
void tick(GameState* state);
void addApple(GameState* state);
GameState initGame();

// Functions

uint16_t rand_range(uint16_t i, uint16_t j) {
    assert(i < j);
    return i + rand() % (j - i);
}

void draw(const GameState* state, SDL_Surface* screen) {
    SDL_FillRect(screen, NULL, COLOR_MARGIN);
    for (uint16_t r = 0; r < GRID_SIZE; r++) {
        for (uint16_t c = 0; c < GRID_SIZE; c++) {
            const uint32_t width_pixels = (screen->w < screen->h) ? screen->w : screen->h;
            const uint32_t r_margin = width_pixels / (CELL_MARGIN_RATIO * GRID_SIZE + GRID_SIZE + 1);
            const uint32_t c_margin = width_pixels / (CELL_MARGIN_RATIO * GRID_SIZE + GRID_SIZE + 1);
            const SDL_Rect cell = {
                .x = c_margin * (1 + c * (CELL_MARGIN_RATIO + 1)),
                .y = r_margin * (1 + r * (CELL_MARGIN_RATIO + 1)),
                .w = c_margin * CELL_MARGIN_RATIO,
                .h = r_margin * CELL_MARGIN_RATIO,
            };
            uint32_t color;
            switch(state->grid[r * GRID_SIZE + c]) {
                case EMPTY: color = COLOR_EMPTY; break;
                case SNAKE: color = COLOR_SNAKE; break;
                case APPLE: color = COLOR_APPLE; break;
            }
            SDL_FillRect(screen, &cell, color);
        }
    }
}

void tick(GameState* state) {
    const uint16_t oldHead = cbuf_head(state->snake);
    uint16_t newHead = oldHead;
    switch(state->headDir) {
        case  LEFT: newHead -=         1; break;
        case    UP: newHead -= GRID_SIZE; break;
        case RIGHT: newHead +=         1; break;
        case  DOWN: newHead += GRID_SIZE; break;
    }
    const bool changedRow = newHead / GRID_SIZE != oldHead / GRID_SIZE;
    const bool changedColumn = newHead % GRID_SIZE != oldHead % GRID_SIZE;
    if (newHead >= GRID_COUNT || (changedRow && changedColumn) || state->grid[newHead] == SNAKE) {
        cbuf_free(state->snake);
        *state = initGame();
        return;
    }
    cbuf_push(state->snake, newHead);
    if (state->grid[newHead] == APPLE) {
        state->capacity++;
        addApple(state);
    }
    while (cbuf_count(state->snake) > state->capacity) {
        state->grid[cbuf_pop_tail(state->snake)] = EMPTY;
    }
    state->grid[newHead] = SNAKE;
    state->lastDir = state->headDir;
}

void addApple(GameState* state) {
    uint16_t apple;
    do {
        apple = rand() % GRID_COUNT;
    } while (state->grid[apple] == SNAKE);
    state->grid[apple] = APPLE;
}

GameState initGame() {
    GameState state = {
        .snake = cbuf_init(GRID_COUNT),
        .headDir = rand() % 4,
        .capacity = 3,
    };
    for (uint16_t i = 0; i < GRID_COUNT; i++) {
        state.grid[i] = EMPTY;
    }
    const uint16_t pos = rand_range(4, GRID_SIZE - 5) + GRID_SIZE * rand_range(4, GRID_SIZE - 5);
    state.grid[pos] = SNAKE;
    cbuf_push(state.snake, pos);

    tick(&state); tick(&state);
    addApple(&state);
    return state;
}

uint32_t pushTickEvent(uint32_t interval, void* param) {
    SDL_Event event = { .type = SDL_USEREVENT };
    SDL_PushEvent(&event);
    return interval;
}

// Main
int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("Initialization failure. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if(window == NULL) {
        printf("Window creation failure. SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_AddTimer(TICK_MILLISECONDS, pushTickEvent, NULL);
    GameState state = initGame();

    if (strcmp(SDL_GetError(), "")) {
        printf("Error during initialization. SDL_Error: %s\n", SDL_GetError());
        SDL_ClearError();
    }

    while (1) {
        if (strcmp(SDL_GetError(), "")) {
            printf("SDL_Error: %s\n", SDL_GetError());
            SDL_ClearError();
        }
        SDL_Event ev;
        while (SDL_PollEvent(&ev) != 0) {
            switch (ev.type) {
                case SDL_QUIT: {
                    cbuf_free(state.snake);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }
                case SDL_USEREVENT: {
                    tick(&state);
                    break;
                }
                case SDL_KEYDOWN: {
                    switch (ev.key.keysym.sym) {
                        case SDLK_LEFT: {
                            if (state.lastDir != RIGHT) state.headDir = LEFT;
                            break;
                        }
                        case SDLK_UP: {
                            if (state.lastDir != DOWN) state.headDir = UP;
                            break;
                        }
                        case SDLK_RIGHT: {
                            if (state.lastDir != LEFT) state.headDir = RIGHT;
                            break;
                        }
                        case SDLK_DOWN: {
                            if (state.lastDir != UP) state.headDir = DOWN;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        draw(&state, SDL_GetWindowSurface(window));
        SDL_UpdateWindowSurface(window);
    }
}
