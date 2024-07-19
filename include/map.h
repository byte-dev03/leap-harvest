#ifndef MAP_H
#define MAP_H

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TILE_SIZE 32
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

typedef struct {
  Texture2D texture;
} Tile;

typedef struct {
  int data[MAP_HEIGHT][MAP_WIDTH];
  Tile tiles[2];
} Map;

void init_map(Map *m);
void load_map(Map *m);
void draw_map(Map *m);
void unload_map(Map *m);

#endif
