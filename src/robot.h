#include "grid.h"

typedef enum {NORTH, EAST, SOUTH, WEST} Direction;

struct Robot;

struct Robot* createRobot(int, int, Direction, int);

void drawRobot(struct Robot*);

void getTileInFront(struct Robot*, int*, int*);

void getTileLeft(struct Robot*, int*, int*);

void getTileRight(struct Robot*, int*, int*);

void forward(struct Robot*);

void left(struct Robot*);

void right(struct Robot*);

int getRobotX(struct Robot*);

int getRobotY(struct Robot*);

Direction getRobotDirection(struct Robot*);

int markerCount(struct Robot*);

void incrementMarkerCount(struct Robot*);

void decrementMarkerCount(struct Robot*);

void pickUpMarker(struct Robot*, struct Grid*);

void visitCurrentTile(struct Robot*);

bool tileVisited(struct Robot*, int, int);

void resetVisitMemory(struct Robot*);

void destroyRobot(struct Robot*);