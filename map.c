#include "map.h"
#include <stdio.h>
#include <stdlib.h>


Texture2D* load_map_texture(const char* filename) {
  Texture2D *tex = (Texture2D *)malloc(sizeof(Texture2D));
  if (tex != NULL) {
    *tex = LoadTexture(filename);
    TraceLog(LOG_INFO, "TMX texture loaded from %s", filename);
    return tex;
  }
  return NULL;
}

void unload_map_texture(Texture2D *tex) {
  if (tex != NULL) {
    UnloadTexture(*tex);
    free(tex);
  }
}
void unload_collision_map(int **collision_map) {
  // Free collision map memory
    for (int i = 0; i < 18; ++i) {
      free(collision_map[i]);
  }
  free(collision_map);
}

int **generate_collision_map(const char* map_fname) {
  tmx_map* map_tmx = tmx_load(map_fname);
  printf("%d\n", map_tmx->height);
    if (!map_tmx) {
        tmx_perror("tmx_load");
        return NULL;
    }

    int** collision_map = (int**)malloc(map_tmx->height * sizeof(int*));
    for (int i = 0; i < map_tmx->height; ++i) {
        collision_map[i] = (int*)malloc(map_tmx->width * sizeof(int));
        for (int j = 0; j < map_tmx->width; ++j) {
            tmx_layer* layer = map_tmx->ly_head;
            while (layer) {
                if (layer->type == L_LAYER && layer->visible) {
                    unsigned long idx = i * map_tmx->width + j;
                    unsigned int gid = layer->content.gids[idx];
                    tmx_tile* tile = map_tmx->tiles[gid];
                    if (tile) {
                        // Check tile properties or conditions to determine collision status
                        collision_map[i][j] = (tile->properties && tmx_get_property(tile->properties, "collision")) ? SOLID_TILE : WALKABLE_TILE;
                    } else {
                        collision_map[i][j] = WALKABLE_TILE; // No tile here, assume walkable
                    }
                }
                layer = layer->next;
            }
        }
    }

    tmx_map_free(map_tmx);

    return collision_map;
}

static void DrawTmxLayer(tmx_map *map, tmx_layer *layer)
{
    unsigned long row, col;
    unsigned int gid;
    unsigned int flip;
    tmx_tile *tile;
    unsigned int tile_width;
    unsigned int tile_height;
    Rectangle sourceRect;
    Rectangle destRect;
    Texture2D *tsTexture; // tileset texture
    float rotation = 0.0;
    Vector2 origin = {0.0, 0.0};

    for (row = 0; row < map->height; row++)
    {
        for (col = 0; col < map->width; col++)
        {
            gid = layer->content.gids[(row * map->width) + col];
            flip = TMX_FLIPPED_VERTICALLY;    // get flip operations from GID
            tile = map->tiles[gid];
            if (tile != NULL)
            {
                // Get tile's texture out of the tileset texture
                if (tile->image != NULL)
                {
                    tsTexture = (Texture2D *)tile->image->resource_image;
                    tile_width = tile->image->width;
                    tile_height = tile->image->height;
                }
                else
                {
                    tsTexture = (Texture2D *)tile->tileset->image->resource_image;
                    tile_width = tile->tileset->tile_width;
                    tile_height = tile->tileset->tile_height;
                }

                sourceRect.x = tile->ul_x;
                sourceRect.y = tile->ul_y;
                sourceRect.width = destRect.width = tile_width;
                sourceRect.height = destRect.height = tile_height;
                destRect.x = col * tile_width;
                destRect.y = row * tile_height;

                // Deal with flips
                origin.x = 0.0;
                origin.y = 0.0;
                rotation = 1.0;
                switch(flip)
                {
                    case TMX_FLIPPED_DIAGONALLY:
                    {
                        sourceRect.height = -1 * sourceRect.height;
                        rotation = 90.0;
                    } break;
                    case TMX_FLIPPED_VERTICALLY:
                    {
                        sourceRect.height = -1 * sourceRect.height;
                    } break;
                    case TMX_FLIPPED_DIAGONALLY + TMX_FLIPPED_VERTICALLY:
                    {
                        rotation = -90.0;
                    } break;
                    case TMX_FLIPPED_HORIZONTALLY:
                    {
                        sourceRect.width = -1 * sourceRect.width;
                    } break;
                    case  TMX_FLIPPED_DIAGONALLY + TMX_FLIPPED_HORIZONTALLY:
                    {
                        rotation = 90.0;
                    } break;
                    case TMX_FLIPPED_HORIZONTALLY + TMX_FLIPPED_VERTICALLY:
                    {
                        rotation = 180.0;
                    } break;
                    case TMX_FLIPPED_DIAGONALLY + TMX_FLIPPED_HORIZONTALLY + TMX_FLIPPED_VERTICALLY:
                    {
                        sourceRect.width = -1 * sourceRect.width;
                        rotation = 90.0;
                    } break;
                    default:
                    {
                        origin.x = 0.0;
                        origin.y = 0.0;
                        rotation = 0.0;
                    } break;
                }

                // Adjust origin to rotate around the center of the tile, 
                // which means destination recangle's origin must be adjusted.
                if (rotation != 0.0)
                {
                    origin.x = tile_width / 2;
                    origin.y = tile_height / 2;
                    destRect.x += tile_width / 2;
                    destRect.y += tile_height / 2;
                }

                // TODO: Take layer opacity into account
                DrawTexturePro(*tsTexture, sourceRect, destRect, origin, rotation, WHITE);
            }
        }
    }
}
void render_tmxmap_to_frame_buf(const char *map_fname, RenderTexture2D *buf)
{
    tmx_layer *layer = NULL;

    // Setting these two function pointers allows TMX lib to load the tileset graphics and
    // set each tile's resource_image property properly.
    tmx_img_load_func = (void *(*)(const char *))load_map_texture;
    tmx_img_free_func = (void (*)(void *))unload_map_texture;
    tmx_map *mapTmx = tmx_load(map_fname);
    if (mapTmx == NULL) {
        tmx_perror("tmx_load");
        return;
    }

    // Create a frame buffer
    // TODO: I don't life loading the RenderTexture here and unloading it in main(), but map info is needed to 
    // allocate the buffer of the right size, so either load it here, or separate tmx_load part of the code into
    // a separate function to allow the application code to call LoadRenderTexture between tmx_load and actual
    // drawing of the map.
    *buf = LoadRenderTexture(mapTmx->width * mapTmx->tile_width, mapTmx->height * mapTmx->tile_height);

    BeginTextureMode(*buf); // start rendering into the buffer
        ClearBackground(SKYBLUE);
        // Iterate through TMX layers rendering them into buf
        layer = mapTmx->ly_head;
        while(layer)
        {
            if (layer->visible)
            {
                switch(layer->type)
                {
                    case L_LAYER:
                    {
                        TraceLog(LOG_INFO, "Render TMX layer \"%s\"", layer->name);
                        DrawTmxLayer(mapTmx, layer);
                    } break;

                    // Group, Object and Image layer types are not implemented in this example
                    case L_GROUP:   // deliberate fall-through
                    case L_OBJGR:
                    case L_IMAGE:
                    case L_NONE:
                    default:
                        break;
                }
            }
            layer = layer->next;
        }
    EndTextureMode();   // stop rendering into the buffer

    tmx_map_free(mapTmx);
}
void draw_map(RenderTexture2D *buf) {
  DrawTextureRec(
      buf->texture,
      (Rectangle){0, 0, buf->texture.width, buf->texture.height},
      (Vector2){0.0f, 0.0f},
      WHITE);
}
