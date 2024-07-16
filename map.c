#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_map(Map *m){ 
  m->tiles[0] = (Tile){LoadTexture("assets/dirt_block.png")};
  m->tiles[1] = (Tile){LoadTexture("assets/grass_block.png")};
}

void load_map(Map *m) {
    FILE* file = fopen("assets/map.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        exit(-1);
    }
    
    char buf[1024];
    int row = 0;
    int col = 0;

    while (fgets(buf, sizeof(buf), file) && row < MAP_HEIGHT) {
        char* token = strtok(buf, ",");
        while (token != NULL && col < MAP_WIDTH) {
            m->data[row][col] = atoi(token);
            token = strtok(NULL, ",");
            col++;
        }
        col = 0; // Reset column counter for the next row
        row++;
    }

    fclose(file);
}

void draw_map(Map *m) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if(m->data[y][x] == 1){
        DrawTexture(m->tiles[0].texture, x*TILE_SIZE, y*TILE_SIZE-20, WHITE);
      } else if(m->data[y][x] == 2) {
        DrawTexture(m->tiles[1].texture, x*TILE_SIZE, y*TILE_SIZE-20, WHITE);
      }
    }
  }
}

void unload_map(Map *m) {
  for (int i = 0; i < sizeof(m->tiles); i++) {
    UnloadTexture(m->tiles[i].texture);
  }
}
