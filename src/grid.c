#include <stdbool.h>
#include "../drawapp-4.0/graphics.h"
#include "grid.h"
#include "common.h"

typedef enum {EMPTY, MARKER, OBSTACLE, WALL, HOME} TileType;

struct Grid {
    int width;
    int height;
    TileType* array;
    // Width and height of a tile (grid square) in pixels
    int tileSize;
};

bool tileIsMarker(struct Grid* grid, int x, int y){
    return grid->array[coordsToIndex(grid->width, x, y)] == MARKER;
}

bool tileIsEmpty(struct Grid* grid, int x, int y){
    return grid->array[coordsToIndex(grid->width, x, y)] == EMPTY;
}

bool tileIsHome(struct Grid* grid, int x, int y){
    return grid->array[coordsToIndex(grid->width, x, y)] == HOME;
}

bool tileIsWall(struct Grid* grid, int x, int y){
    return grid->array[coordsToIndex(grid->width, x, y)] == WALL;
}

bool tileIsObstacle(struct Grid* grid, int x, int y){
    return grid->array[coordsToIndex(grid->width, x, y)] == OBSTACLE;
}

bool randomFreeTile(struct Grid* grid, int* x, int* y){
    // Choose a random location on the grid that is empty. Returns false if no empty tiles are found
    int xEmpty[grid->width * grid->height];
    int yEmpty[grid->width * grid->height];
    int index = 0;
    for (int x = 0; x < grid->width; x++){
        for (int y = 0; y < grid->height; y++){
            if (tileIsEmpty(grid, x, y)){
                xEmpty[index] = x;
                yEmpty[index] = y;
                index++;
            }
        }
    }
    if (index == 0){
        return false;
    }
    int randomIndex = rand() % index;
    *x = xEmpty[randomIndex];
    *y = yEmpty[randomIndex];
    return true;
}

int countAccessibleTiles(struct Grid* grid, bool* visited, int x, int y){
    // Check if all markers and home are accessible to the robot
    visited[coordsToIndex(grid->width, x, y)] = true;
    int tileCount = 1;
    int xNew, yNew;
    
    const int moves[2] = {1, -1};
    for (int i = 0; i < 2; i++){
        xNew = x + moves[i];
        yNew = y;
        if (!tileIsWall(grid, xNew, yNew) && !tileIsObstacle(grid, xNew, yNew) && !visited[coordsToIndex(grid->width, xNew, yNew)]){
            tileCount += countAccessibleTiles(grid, visited, xNew, yNew);
        }
        xNew = x;
        yNew = y + moves[i];
        if (!tileIsWall(grid, xNew, yNew) && !tileIsObstacle(grid, xNew, yNew) && !visited[coordsToIndex(grid->width, xNew, yNew)]){
            tileCount += countAccessibleTiles(grid, visited, xNew, yNew);
        }
    }
    
    return tileCount;
}

int addWalls(struct Grid* grid, int wallLimit, int wallChance){
    int wallCount = 0;

    // Outer walls
    for (int i = 0; i < grid->width; i++){
        // Top wall
        grid->array[coordsToIndex(grid->width, i, 0)] = WALL;
        // Bottom wall
        grid->array[coordsToIndex(grid->width, i, grid->height - 1)] = WALL;
        wallCount += 2;
    }
    for (int i = 0; i < grid->height; i++){
        // Left wall
        grid->array[coordsToIndex(grid->width, 0, i)] = WALL;
        // Right wall
        grid->array[coordsToIndex(grid->width, grid->width - 1, i)] = WALL;
        wallCount += 2;
    }

    // Correct for the corners being double counted
    wallCount -= 4;

    // Inner walls
    for (int y = 1; y < grid->height - 1; y++){
        for (int x = 1; x < grid->width - 1; x++){
            if (wallCount == wallLimit){
                return wallCount;
            }
            // Make sure all wall tiles are connected
            bool wallAdjacent = tileIsWall(grid, x-1, y) | tileIsWall(grid, x+1, y) | tileIsWall(grid, x, y-1) | tileIsWall(grid, x, y+1);
            if (wallAdjacent && (rand() % wallChance == 0)){
                grid->array[coordsToIndex(grid->width, x, y)] = WALL;
                wallCount++;
            }
        }
    }

    return wallCount;
}

void placeMarker(struct Grid* grid, int x, int y){
    grid->array[coordsToIndex(grid->width, x, y)] = MARKER;
}

void addMarkers(struct Grid* grid, int nMarkers){
    int x, y;
    for (int i = 0; i < nMarkers; i++){
        randomFreeTile(grid, &x, &y);
        placeMarker(grid, x, y);
    }
}

int addObstacles(struct Grid* grid, int nObstacles){
    int x, y;
    for (int i = 0; i < nObstacles; i++){
        randomFreeTile(grid, &x, &y);
        grid->array[coordsToIndex(grid->width, x, y)] = OBSTACLE;
    }
    return nObstacles;
}

void addHome(struct Grid* grid){
    int x, y;
    randomFreeTile(grid, &x, &y);
    grid->array[coordsToIndex(grid->width, x, y)] = HOME;
}

void clearGrid(struct Grid* grid){
    for (int i = 0; i < grid->width * grid->height; i++){
        grid->array[i] = EMPTY;
    }
}

struct Grid* createGrid(int width, int height, int tileSize, int nObstacles, int nMarkers, int wallLimit, int wallChance){
    struct Grid* grid = (struct Grid*)malloc(sizeof(struct Grid));
    grid->width = width;
    grid->height = height;
    // Grid is represented as a 1D array, with tile (x, y) accessed by array[y * width + x]
    grid->array = (TileType*)malloc(width * height * sizeof(TileType));
    for (int i = 0; i < width * height; i++){
        grid->array[i] = EMPTY;
    }
    grid->tileSize = tileSize;

    bool foundTile, gridValid;
    bool* visited = (bool*)calloc(width * height, sizeof(bool));
    int x, y, nWallTiles, nFreeTiles;

    do {
        clearGrid(grid);
        nWallTiles = addWalls(grid, wallLimit, wallChance);
        addObstacles(grid, nObstacles);
        foundTile = randomFreeTile(grid, &x, &y);
        nFreeTiles = (grid->height * grid->width) - nWallTiles - nObstacles;
        gridValid = foundTile && countAccessibleTiles(grid, visited, x, y) == nFreeTiles;
        memset(visited, false, width * height * sizeof(bool));
    } while (!gridValid);

    free(visited);

    addMarkers(grid, nMarkers);
    addHome(grid);

    return grid;
}

void drawGridTiles(struct Grid* grid){
    for (int x = 0; x < grid->width; x++){
        for (int y = 0; y < grid->height; y++){
            switch (grid->array[coordsToIndex(grid->width, x, y)]){
                case MARKER:
                    setColour(lightgray);
                    fillRect(x * grid->tileSize, y * grid->tileSize, grid->tileSize, grid->tileSize);
                    break;
                case OBSTACLE:
                    setColour(black);
                    fillRect(x * grid->tileSize, y * grid->tileSize, grid->tileSize, grid->tileSize);
                    break;
                case WALL:
                    setColour(red);
                    fillRect(x * grid->tileSize, y * grid->tileSize, grid->tileSize, grid->tileSize);
                    break;
                case HOME:
                    setColour(blue);
                    fillRect(x * grid->tileSize, y * grid->tileSize, grid->tileSize, grid->tileSize);
                    break;
                default:
                    break;
            }
        }
    }
}

void drawGridLines(struct Grid* grid){
    setColour(black);
    for (int i = 0; i < grid->height; i++){
        drawLine(0, i * grid->tileSize, grid->width * grid->tileSize, i * grid->tileSize);
    }
    for (int i = 0; i < grid->width; i++){
        drawLine(i * grid->tileSize, 0, i * grid->tileSize, grid->height * grid->tileSize);
    }
}

void drawGrid(struct Grid* grid){
    background();
    clear();
    setColour(black);

    drawGridTiles(grid);
    drawGridLines(grid);
}

void clearTile(struct Grid* grid, int x, int y){
    grid->array[coordsToIndex(grid->width, x, y)] = EMPTY;
}

int getTileSize(struct Grid* grid){
    return grid->tileSize;
}

void destroyGrid(struct Grid* grid){
    free(grid->array);
    free(grid);
}