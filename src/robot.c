#include <stdlib.h>
#include <ctype.h>
#include "../drawapp-4.0/graphics.h"
#include "robot.h"
#include "common.h"

const int VISIT_MEM_HEIGHT = 100;
const int VISIT_MEM_WIDTH = 100;
extern int SLEEP_DURATION;

struct Robot {
    Direction direction;
    int x;
    int y;
    int numMarkers;
    // To store visited tiles as the robot traverses the grid finding markers
    bool* visitMemory;
    // Size of a grid tile in pixels - used when drawing the robot
    int tileSize;
};

void setRobotPoints(int xArray[], int yArray[], int x0, int y0, int x1, int y1, int x2, int y2){
    xArray[0] = x0; yArray[0] = y0;
    xArray[1] = x1; yArray[1] = y1;
    xArray[2] = x2; yArray[2] = y2;
}

struct Robot* createRobot(int x, int y, Direction direction, int tileSize){
    struct Robot* robot = (struct Robot*)malloc(sizeof(struct Robot));
    robot->x = x;
    robot->y = y;
    robot->direction = direction;
    robot->visitMemory = (bool*)calloc(VISIT_MEM_HEIGHT * VISIT_MEM_WIDTH, sizeof(bool));
    robot->tileSize = tileSize;
    robot->numMarkers = 0;
    return robot;
}

void drawRobot(struct Robot* robot){
    foreground();
    clear();
    setColour(green);

    int xTopLeft = getRobotX(robot) * robot->tileSize;
    int yTopLeft = getRobotY(robot) * robot->tileSize;

    int xArray[3];
    int yArray[3];

    switch (getRobotDirection(robot)){ 
        case NORTH:
            setRobotPoints(xArray, yArray, xTopLeft, yTopLeft + robot->tileSize, xTopLeft + robot->tileSize / 2, yTopLeft,
            xTopLeft + robot->tileSize, yTopLeft + robot->tileSize);
            break;
        case EAST:
            setRobotPoints(xArray, yArray, xTopLeft, yTopLeft, xTopLeft + robot->tileSize, yTopLeft + robot->tileSize / 2, 
            xTopLeft, yTopLeft + robot->tileSize);
            break;
        case SOUTH:
            setRobotPoints(xArray, yArray, xTopLeft, yTopLeft, xTopLeft + robot->tileSize / 2, yTopLeft + robot->tileSize, 
            xTopLeft + robot->tileSize, yTopLeft);
            break;
        case WEST:
            setRobotPoints(xArray, yArray, xTopLeft + robot->tileSize, yTopLeft, xTopLeft, yTopLeft + robot->tileSize / 2, 
            xTopLeft + robot->tileSize, yTopLeft + robot->tileSize);
            break;
    }

    fillPolygon(3, xArray, yArray);
}

void getTileInFront(struct Robot* robot, int* x, int* y){
    *x = robot->x;
    *y = robot->y;

    switch (robot->direction){
        case NORTH : (*y)--; break;
        case EAST : (*x)++; break;
        case SOUTH : (*y)++; break;
        case WEST : (*x)--; break;
    }
}

void getTileLeft(struct Robot* robot, int* x, int* y){
    *x = robot->x;
    *y = robot->y;

    switch (robot->direction){
        case NORTH : (*x)--; break;
        case EAST : (*y)--; break;
        case SOUTH : (*x)++; break;
        case WEST : (*y)++; break;
    }
}

void getTileRight(struct Robot* robot, int* x, int* y){
    *x = robot->x;
    *y = robot->y;

    switch (robot->direction){
        case NORTH : (*x)++; break;
        case EAST : (*y)++; break;
        case SOUTH : (*x)--; break;
        case WEST : (*y)--; break;
    }
}

void forward(struct Robot* robot){
    getTileInFront(robot, &robot->x, &robot->y);
    drawRobot(robot);
    sleep(SLEEP_DURATION);
}

void right(struct Robot* robot){
    switch (robot->direction){
        case NORTH : robot->direction = EAST; break;
        case EAST : robot->direction = SOUTH; break;
        case SOUTH : robot->direction = WEST; break;
        case WEST : robot->direction = NORTH; break;
    }
    drawRobot(robot);
    sleep(SLEEP_DURATION);
}

void left(struct Robot* robot){
    switch (robot->direction){
        case NORTH : robot->direction = WEST; break;
        case EAST : robot->direction = NORTH; break;
        case SOUTH : robot->direction = EAST; break;
        case WEST : robot->direction = SOUTH; break;
    }
    drawRobot(robot);
    sleep(SLEEP_DURATION);
}

int getRobotX(struct Robot* robot){
    return robot->x;
}

int getRobotY(struct Robot* robot){
    return robot->y;
}

Direction getRobotDirection(struct Robot* robot){
    return robot->direction;
}

int markerCount(struct Robot* robot){
    return robot->numMarkers;
}

void incrementMarkerCount(struct Robot* robot){
    robot->numMarkers++;
}

void decrementMarkerCount(struct Robot* robot){
    robot->numMarkers--;
}

void visitCurrentTile(struct Robot* robot){
    robot->visitMemory[coordsToIndex(VISIT_MEM_WIDTH, robot->x, robot->y)] = true;
}

bool tileVisited(struct Robot* robot, int x, int y){
    return robot->visitMemory[coordsToIndex(VISIT_MEM_WIDTH, x, y)];
}

void resetVisitMemory(struct Robot* robot){
    for (int i = 0; i < VISIT_MEM_HEIGHT * VISIT_MEM_WIDTH; i++){
        robot->visitMemory[i] = false;
    }
}

void destroyRobot(struct Robot* robot){
    free(robot->visitMemory);
    free(robot);
}
