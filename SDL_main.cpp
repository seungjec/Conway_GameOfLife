#include "SDL_main.h"
#include <stdio.h>
#include <algorithm>

void RunSDL()
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event{};

    int window_w = 800;
    int window_h = 600;

    auto err = InitializedSDL(&window, &renderer, window_w, window_h);
    if (err != 0)
        return;

    err = ExecuteSDL(&renderer, event, window_w, window_h);
    if (err != 0)
        return;

    FinalizedSDL(&window, &renderer);
}

int InitializedSDL(SDL_Window** window, SDL_Renderer** renderer, int width, int height)
{
    int window_w = width;
    int window_h = height;

    auto err = SDL_Init(SDL_INIT_EVERYTHING);
    if (err != 0)
    {
        printf("SDL_Init fail, Error Code = %d, Desc = %s", err, SDL_GetError());
        return err;
    }

    *window = SDL_CreateWindow(
        "Conway's Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_w, window_h, false);
    if (*window == NULL)
    {
        printf("SDL_CreateWindow fail, Desc = %s", SDL_GetError());
        return -1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, 0);
    if (*renderer == NULL)
    {
        printf("SDL_CreateRenderer fail, Desc = %s", SDL_GetError());
        return -1;
    }

    return 0;
}

void FinalizedSDL(SDL_Window** window, SDL_Renderer** renderer)
{
    if (*window != NULL)
    {
        SDL_DestroyWindow(*window);
        *window = NULL;
    }
    if (*renderer != NULL)
    {
        SDL_DestroyRenderer(*renderer);
        *renderer = NULL;
    }
    
    SDL_Quit();
}

int ExecuteSDL(SDL_Renderer** renderer, SDL_Event& event, int width, int height)
{
    int window_w = width;
    int window_h = height;

    int FPS = 60;
    Uint64 millisecond_per_frame = 1000 / FPS;
    Uint64 frameStart, aFrameTime;

    bool isRunning = true;
    bool isUpdate = true;
    int GridColor = 230;

    SDL_Surface* tmpSurface = NULL;

    tmpSurface = SDL_CreateRGBSurface(0, 800, 400, 8, 0, 0, 0, 0);
    if (tmpSurface == NULL)
    {
        printf("IMG_Load fail, Desc = %s", SDL_GetError());
        return -1;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(*renderer, tmpSurface);
    if (texture == NULL)
    {
        printf("SDL_CreateTextureFromSurface fail, Desc = %s", SDL_GetError());
        return -1;
    }

    SDL_FreeSurface(tmpSurface);

    int grid_size = 10;
    int numXPoints = (window_h / grid_size + 1) * 2;
    int numYPoints = (window_w / grid_size + 1) * 2;

    SDL_Point* XLinePoints = (SDL_Point *)malloc(numXPoints * sizeof(SDL_Point));
    SDL_Point* YLinePoints = (SDL_Point*)malloc(numYPoints * sizeof(SDL_Point));
    if (XLinePoints == NULL || YLinePoints == NULL)
        return -1;

    SetGridLine(renderer, XLinePoints, YLinePoints, window_w, window_h, grid_size);

    int numXCells = window_w / grid_size;
    int numYCells = window_h / grid_size;
    bool* Cells = (bool*)malloc(numXCells * numYCells * sizeof(bool));
    SDL_Rect* CellRects = (SDL_Rect*)malloc(numXCells * numYCells * sizeof(SDL_Rect));
    if (Cells == NULL || CellRects == NULL)
        return -1;

    SetCells(Cells, numXCells, numYCells, grid_size);
    SetCellRects(CellRects, numXCells, numYCells, grid_size);

    // Main Loop
    while (isRunning)
    {
        // FPS
        frameStart = SDL_GetTicks64();

        // Event
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                isRunning = false;
                break;
            case SDLK_SPACE:
                if (isUpdate)
                {
                    isUpdate = false;
                    GridColor = 150;
                }
                else
                {
                    isUpdate = true;
                    GridColor = 230;
                }
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                int xidx = event.button.x / grid_size;
                int yidx = event.button.y / grid_size;
                Cells[xidx + numXCells * yidx] = true;
            }
                break;
            case SDL_BUTTON_RIGHT:
            {
                int xidx = event.button.x / grid_size;
                int yidx = event.button.y / grid_size;
                Cells[xidx + numXCells * yidx] = false;
            }
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                int xidx = event.motion.x / grid_size;
                int yidx = event.motion.y / grid_size;
                Cells[xidx + numXCells * yidx] = true;
            }
                break;
            case SDL_BUTTON_RIGHT:
            {
                printf("SDL_BUTTON_RIGHT\n");
                int xidx = event.motion.x / grid_size;
                int yidx = event.motion.y / grid_size;
                Cells[xidx + numXCells * yidx] = false;
            }
                break;
            case SDL_BUTTON_X1:
            {
                printf("SDL_BUTTON_X1\n");
                int xidx = event.motion.x / grid_size;
                int yidx = event.motion.y / grid_size;
                Cells[xidx + numXCells * yidx] = false;
            }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        // Update
        if (isUpdate)
        {
            UpdateCell(Cells, numXCells, numYCells);
        }

        
        // Render
        SDL_RenderClear(*renderer);
        SDL_RenderCopy(*renderer, texture, NULL, NULL);

        SDL_SetRenderDrawColor(*renderer, GridColor, GridColor, GridColor, 255);
        SDL_RenderDrawLines(*renderer, XLinePoints, numXPoints);
        SDL_RenderDrawLines(*renderer, YLinePoints, numYPoints);

        SDL_SetRenderDrawColor(*renderer, 100, 100, 100, 255);
        for (int yidx = 0; yidx < numYCells; yidx++)
        {
            for (int xidx = 0; xidx < numXCells; xidx++)
            {
                if (Cells[xidx + numXCells * yidx])
                    SDL_RenderFillRect(*renderer, &CellRects[xidx + numXCells * yidx]);
            }
        }

        SDL_RenderPresent(*renderer);


        // FPS
        aFrameTime = SDL_GetTicks64() - frameStart;
        if (aFrameTime < millisecond_per_frame)
            SDL_Delay((Uint32)(millisecond_per_frame - aFrameTime));
        else
            printf("Frame Drop : %lldms\n", aFrameTime);

    }

    if (texture != NULL)
       SDL_DestroyTexture(texture);
    if (XLinePoints != NULL)
        free(XLinePoints);
    if (YLinePoints != NULL)
        free(YLinePoints);
    if (Cells != NULL)
        free(Cells);
    if (CellRects != NULL)
        free(CellRects);

    return 0;
}

void SetGridLine(SDL_Renderer** renderer, SDL_Point* XLinePoints, SDL_Point* YLinePoints, int window_w, int window_h, int grid_size)
{
    SDL_SetRenderDrawColor(*renderer, 230, 230, 230, 255);

    int numXLines = window_h / grid_size + 1;
    int numYLines = window_w / grid_size + 1;
    int numXPoints = numXLines * 2;
    int numYPoints = numYLines * 2;

    for (int i = 0; i < numXLines; i++)
    {
        if (i % 2 == 0)
        {
            XLinePoints[2 * i] = { 0, grid_size * i };
            XLinePoints[2 * i + 1] = { window_w - 1, grid_size * i };
        }
        else
        {
            XLinePoints[2 * i] = { window_w - 1, grid_size * i };
            XLinePoints[2 * i + 1] = { 0, grid_size * i };
        }
    }

    for (int i = 0; i < numYLines; i++)
    {
        if (i % 2 == 0)
        {
            YLinePoints[2 * i] = { grid_size * i, 0 };
            YLinePoints[2 * i + 1] = { grid_size * i, window_h - 1 };
        }
        else
        {
            YLinePoints[2 * i] = { grid_size * i, window_h - 1 };
            YLinePoints[2 * i + 1] = { grid_size * i, 0 };
        }
    }
}

void UpdateCell(bool* Cells, int numXCells, int numYCells)
{
    numXCells = std::max(std::min(numXCells, MAX_SIZE), 0);
    numYCells = std::max(std::min(numYCells, MAX_SIZE), 0);

    bool* tmpCells = (bool*)malloc(numXCells * numYCells * sizeof(bool));
    if (tmpCells == NULL)
        return;

    for (int yidx = 0; yidx < numYCells; yidx++)
    {
        for (int xidx = 0; xidx < numXCells; xidx++)
        {
            tmpCells[xidx + numXCells * yidx] = CheckRule(Cells, xidx, yidx, numXCells, numYCells);
        }
    }

    memcpy(Cells, tmpCells, numXCells * numYCells * sizeof(bool));

    if (tmpCells != NULL)
    {
        free(tmpCells);
    }
}

/*
Any live cell with fewer than two live neighbours dies, as if by underpopulation.
Any live cell with two or three live neighbours lives on to the next generation.
Any live cell with more than three live neighbours dies, as if by overpopulation.
Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
*/
bool CheckRule(bool* Cells, int xidx, int yidx, int width, int height)
{
    int xm = std::max(xidx - 1, 0);
    int x0 = xidx;
    int xp = std::min(xidx + 1, width - 1);
    int ym = std::max(yidx - 1, 0);
    int y0 = yidx;
    int yp = std::min(yidx + 1, height - 1);
    bool islive = Cells[x0 + width * y0];

    int numNeighbours = Cells[xm + width * ym] +
        Cells[x0 + width * ym] +
        Cells[xp + width * ym] +
        Cells[xm + width * y0] +
        Cells[xp + width * y0] +
        Cells[xm + width * yp] +
        Cells[x0 + width * yp] +
        Cells[xp + width * yp];

    if (islive == true)
    {
        if (numNeighbours < 2 || numNeighbours > 3)
            return false;
        else
            return true;
    }
    else
    {
        if (numNeighbours == 3)
            return true;
        else
            return false;
    }
}

void SetCells(bool* Cells, int numXCells, int numYCells, int grid_size)
{
    memset(Cells, 0, numXCells * numYCells * sizeof(bool));

    //Cells[xidx + numXCells * yidx];
    Cells[41 + numXCells * 30] = true;
    Cells[40 + numXCells * 31] = true;
    Cells[41 + numXCells * 31] = true;
    Cells[41 + numXCells * 32] = true;
    Cells[42 + numXCells * 32] = true;

    //for (int i = 0; i < 3; i++)
    //{
    //    Cells[1400 + i] = true;
    //}
}

void SetCellRects(SDL_Rect* CellRects, int numXCells, int numYCells, int grid_size)
{
    for (int yidx = 0; yidx < numYCells; yidx++)
    {
        for (int xidx = 0; xidx < numXCells; xidx++)
        {
            CellRects[xidx + numXCells * yidx] = { grid_size * xidx, grid_size * yidx, grid_size, grid_size };
        }
    }
}