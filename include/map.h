#ifndef MAP_H
#define MAP_H

#include <raylib.h>
#include "tmx.h"

#define TILE_SIZE 32
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

#define WALKABLE_TILE 0
#define SOLID_TILE 1

typedef struct {
  Texture2D texture;
} Tile;

typedef struct {
  tmx_map* data;
} Map;

Texture2D* load_map_texture(const char* filename);
void render_tmxmap_to_frame_buf(const char *map_fname, RenderTexture2D *buf);
void draw_map(RenderTexture2D *buf);
void unload_map_texture(Texture2D *tex);
void unload_collision_map(int **collision_map);
int **generate_collision_map(const char* map_fname);

#endif
