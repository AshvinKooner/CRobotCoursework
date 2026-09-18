#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Grid;

struct Grid* createGrid(int, int, int, int, int, int, int);

void drawGrid(struct Grid*);

bool randomFreeTile(struct Grid*, int*, int*);

bool tileIsMarker(struct Grid*, int, int);

bool tileIsEmpty(struct Grid*, int, int);

bool tileIsHome(struct Grid*, int, int);

void placeMarker(struct Grid*, int, int);

void clearTile(struct Grid*, int, int);

int getTileSize(struct Grid*);

void destroyGrid(struct Grid*);