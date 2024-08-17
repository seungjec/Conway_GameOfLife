#pragma once

#include <SDL.h>

#define WINDOW_W 1920*2
#define WINDOW_H 1080*2
#define FULL_SCREEN 0
#define FPS 100
#define GRID_SIZE 10
#define GRID_COLOR 230
#define CELL_COLOR 100
#define PAUSE_COLOR 150


void RunSDL();
int InitializedSDL(SDL_Window** window, SDL_Renderer** renderer, int width, int height);
void FinalizedSDL(SDL_Window** window, SDL_Renderer** renderer);
int ExecuteSDL(SDL_Renderer** renderer, SDL_Event& event, int width, int height);

void SetGridLine(SDL_Renderer** renderer, SDL_Point* XLinePoints, SDL_Point* YLinePoints, int window_w, int window_h, int grid_size);
bool UpdateCell(bool* Cells, int numXCells, int numYCells);
bool CheckRule(bool* Cells, int xidx, int yidx, int width, int height);
bool SetCells(bool* Cells, int numXCells, int numYCells, int grid_size);
void SetCellRects(SDL_Rect* CellRects, int numXCells, int numYCells, int grid_size);
