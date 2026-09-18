#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "robot.h"
#include "grid.h"
#include "../drawapp-4.0/graphics.h"

extern int SLEEP_DURATION;
const int MIN_GRID_WIDTH = 10;
const int MIN_GRID_HEIGHT = 10;
const int MAX_GRID_WIDTH = 60;
const int MAX_GRID_HEIGHT = 30;
const int TILE_SIZE = 20;
// Lower the number, the more walls are added (on average)
const int WALL_CHANCE = 2;

bool canMoveForward(struct Robot* robot, struct Grid* grid){
    int x = getRobotX(robot), y = getRobotY(robot);
    getTileInFront(robot, &x, &y);
    return tileIsEmpty(grid, x, y) | tileIsMarker(grid, x, y) | tileIsHome(grid, x, y);
}

void undoForward(struct Robot* robot, struct Grid* grid){
    for (int i=0; i<2; i++){
        left(robot);
    }
    forward(robot);
    for (int i=0; i<2; i++){
        right(robot);
    }
}

bool isAtMarker(struct Robot* robot, struct Grid* grid){
    return tileIsMarker(grid, getRobotX(robot), getRobotY(robot));
}

bool isAtHome(struct Robot* robot, struct Grid* grid){
    return tileIsHome(grid, getRobotX(robot), getRobotY(robot));
}

void pickUpMarker(struct Robot* robot, struct Grid* grid){
    incrementMarkerCount(robot);
    clearTile(grid, getRobotX(robot), getRobotY(robot));
    drawGrid(grid);
    drawRobot(robot);
    sleep(SLEEP_DURATION);
}

void dropMarker(struct Robot* robot, struct Grid* grid){
    decrementMarkerCount(robot);
    placeMarker(grid, getRobotX(robot), getRobotY(robot));
    drawGrid(grid);
    drawRobot(robot);
    sleep(SLEEP_DURATION);
}

void findMarkers(struct Robot* robot, struct Grid* grid, int depth){
    bool traversed = false;

    visitCurrentTile(robot);

    if (isAtMarker(robot, grid)){
        pickUpMarker(robot, grid);
    }

    int x, y;
    getTileInFront(robot, &x, &y);
    if (!tileVisited(robot, x, y) && canMoveForward(robot, grid)){
        forward(robot);
        findMarkers(robot, grid, depth + 1);
        traversed = true;
        undoForward(robot, grid);
    }

    // Traverse the grid to the left
    getTileLeft(robot, &x, &y);
    if (!tileVisited(robot, x, y)){
        left(robot);
        if (canMoveForward(robot, grid)){
            findMarkers(robot, grid, depth + 1);
            traversed = true;
        }
        right(robot);
    }

    // Traverse the grid to the right
    getTileRight(robot, &x, &y);
    if (!tileVisited(robot, x, y)){
        right(robot);
        if (canMoveForward(robot, grid)){
            findMarkers(robot, grid, depth + 1);
            traversed = true;
        }
        left(robot);
    }

    // Special case - robot can't move forwards, left or right from start position. Must move backwards.
    if (depth == 0 && !traversed){
        for (int i=0; i<2; i++){
            left(robot);
        }
        forward(robot);
        findMarkers(robot, grid, depth + 1);
    }
}

void dropAllMarkers(struct Robot* robot, struct Grid* grid){
    while (markerCount(robot) > 0){
        dropMarker(robot, grid);
    }
}

bool dropMarkersAtHome(struct Robot* robot, struct Grid* grid){
    visitCurrentTile(robot);

    if (isAtHome(robot, grid)){
        dropAllMarkers(robot, grid);
        return true;
    }

    int x, y;

    getTileInFront(robot, &x, &y);
    if (!tileVisited(robot, x, y) & canMoveForward(robot, grid)){
        forward(robot);
        if (dropMarkersAtHome(robot, grid)){
            return true;
        }
        undoForward(robot, grid);
    }

    // Traverse the grid to the left
    getTileLeft(robot, &x, &y);
    if (!tileVisited(robot, x, y)){
        left(robot);
        if (canMoveForward(robot, grid)){
            if (dropMarkersAtHome(robot, grid)){
                return true;
            }
        }
        right(robot);
    }

    // Traverse the grid to the right
    getTileRight(robot, &x, &y);
    if (!tileVisited(robot, x, y)){
        right(robot);
        if (canMoveForward(robot, grid)){
            if (dropMarkersAtHome(robot, grid)){
                return true;
            }
        }
        left(robot);
    }

    return false;
}

void setupRobotGrid(struct Robot** robot, struct Grid** grid){
    int gridWidth = rand() % (MAX_GRID_WIDTH - MIN_GRID_WIDTH) + MIN_GRID_WIDTH;
    int gridHeight = rand() % (MAX_GRID_HEIGHT - MIN_GRID_HEIGHT) + MIN_GRID_HEIGHT;
    int nObstacles = rand() % (gridWidth + gridHeight)/2 + 1;
    int nMarkers = rand() % (gridWidth + gridHeight)/2 + 1;
    int wallLimit = (gridWidth * gridHeight) / 2;
    
    setWindowSize(gridWidth * TILE_SIZE, gridHeight * TILE_SIZE);
    *grid = createGrid(gridWidth, gridHeight, TILE_SIZE, nObstacles, nMarkers, wallLimit, WALL_CHANCE);

    // Restrict robot to within the walls, and not next to a wall
    int robotX, robotY;
    randomFreeTile(*grid, &robotX, &robotY);
    char robotDir = (Direction[]){NORTH, EAST, SOUTH, WEST}[rand() % 4];

    *robot = createRobot(robotX, robotY, robotDir, TILE_SIZE);
}

int main(int argc, char **argv){

    srand(time(NULL));

    struct Robot* robot = NULL;
    struct Grid* grid = NULL;
    setupRobotGrid(&robot, &grid);

    drawGrid(grid);
    drawRobot(robot);
    sleep(SLEEP_DURATION);
    
    findMarkers(robot, grid, 0);
    resetVisitMemory(robot);
    dropMarkersAtHome(robot, grid);

    destroyGrid(grid);
    destroyRobot(robot);
}