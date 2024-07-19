#include "include/player.h"
#include <math.h>
#include <stdio.h>

#define TILE_HEIGHT 32
#define TILE_WIDTH 32

void player_init(Player *p, float x, float y) {
  p->speed = 200.0f;
  p->velocity = (Vector2){0, 0};
  p->position = (Vector2){x, y};
  p->direction = 1;

  p->textures[0] =
      LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Idle_4.png");
  p->textures[1] =
      LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Run_6.png");
  p->textures[2] =
      LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Jump_8.png");

  p->current_animation = 0;
  p->current_texture = p->textures[p->current_animation];
  p->current_frame = 0;

  p->frame_width = (float)(p->current_texture.width / NUM_FRAMES);
  p->frame_height = (float)(p->current_texture.height);
  p->frame_rec = (Rectangle){0, 0, p->frame_width, p->frame_height};

  p->num_frames = 4;
  p->frame_rate = 0.1f;
  p->frame_timer = 0.0f;

  p->jumped = false;
  p->gravity = 9.81f;
  p->grounded = true;
  p->jump_height = 1.5f;
  p->flip_h = false;
}

void player_draw(Player *p) {
  Rectangle source_rec = p->frame_rec;
  Rectangle dest_rec = {p->position.x, p->position.y, p->frame_width,
                        p->frame_height};
  Vector2 origin = {0.0f, 0.0f};

  if (p->flip_h) {
    source_rec.width *= -1; // Flip horizontally
  }

  int tile_x = (int)p->position.x / TILE_SIZE;
  int tile_y = (int)p->position.y / TILE_SIZE - 20;
  DrawRectangle(tile_x, tile_y, 32, 32, RED);
  DrawTexturePro(p->current_texture, source_rec, dest_rec, origin, 0.0f, WHITE);
}

void player_animate(Player *p, float dt) {
  p->frame_timer += dt;

  if (p->frame_timer >= p->frame_rate) {
    p->frame_timer = 0.0f;
    p->current_frame++;

    p->num_frames = p->current_animation == 0   ? 4
                    : p->current_animation == 1 ? 6
                    : p->current_animation == 2 ? 8
                                                : 1;

    if (p->current_frame >= p->num_frames) {
      p->current_frame = 0;
    }
  }

  p->frame_rec.x = p->frame_width * p->current_frame;
  p->frame_rec.y = 0;
}
bool check_collision(Rectangle rect1, Rectangle rect2) {
  return rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x &&
         rect1.y < rect2.y + rect2.height && rect1.y + rect1.height > rect2.y;
}

bool player_collides_with_map(Player *p, Map *map, float dt) {
  Rectangle player_rect = {p->position.x, p->position.y, p->frame_width,
                           p->frame_height};

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map->data[y][x] != 0) {
        Rectangle tile_rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE,
                               TILE_SIZE};
        if (check_collision(player_rect, tile_rect)) {
          // Check if the player is colliding with the tile from the top
          if (p->velocity.y > 0) {
            p->position.y = tile_rect.y - p->frame_height;
            p->velocity.y = 0;
            p->grounded = true;
          }
          // Check if the player is colliding with the tile from the bottom
          else if (p->velocity.y < 0) {
            p->position.y = tile_rect.y + TILE_SIZE;
            p->velocity.y = 0;
          }
          // Check if the player is colliding with the tile from the left
          if (p->velocity.x > 0) {
            p->position.x = tile_rect.x - p->frame_width;
          }
          // Check if the player is colliding with the tile from the right
          else if (p->velocity.x < 0) {
            p->position.x = tile_rect.x + TILE_SIZE;
          }
          return true;
        }
      }
    }
  }

  // Check the tile below the player
  int tile_x = (int)p->position.x / TILE_SIZE;
  int tile_y = (int)(p->position.y + p->frame_height) / TILE_SIZE;

  if (tile_y < MAP_HEIGHT) {
    if (map->data[tile_y][tile_x] == 0) {
      p->grounded = false;
      p->velocity.y += p->gravity * dt; // Apply gravity
      p->position.y += p->velocity.y * dt;
    } else {
      p->grounded = true;
    }
  }

  return false;
}

void player_jump(Player *p, float dt, Map *map) {
  float initial_velocity = sqrtf(2.0f * p->jump_height * fabsf(p->gravity));

  int tile_x = (int)p->position.x / TILE_SIZE;
  int tile_y = (int)(p->position.y + p->frame_height) / TILE_SIZE;

  bool has_tile_above = tile_y - 1 >= 0 && map->data[tile_y - 1][tile_x] != 0;

  if (IsKeyDown(KEY_SPACE) && p->grounded && !has_tile_above) {
    p->velocity.y = -initial_velocity;
    p->jumped = true;
    p->grounded = false;
    p->current_animation = 2;
    p->flip_h = (p->direction == -1);
  }

  if (p->jumped) {
    p->velocity.y += p->gravity * dt;
    p->position.y += p->velocity.y * dt;

    if (player_collides_with_map(p, map, dt)) {
      p->jumped = false;
      p->grounded = true;
      p->current_animation = 0;
      p->flip_h = (p->direction == -1);
    }
  }
}

int player_reset_position(Player *p) {
  p->position.x = 400;
  p->position.y = 300;
  return 0;
}

void player_move(Player *p, float dt, Map *map) {
  Vector2 old_position = p->position;
  p->velocity.x = 0;
  if (IsKeyDown(KEY_D)) {
    p->current_animation = 1;
    p->flip_h = false;
    p->direction = 1;
    p->velocity.x = p->speed;
  } else if (IsKeyDown(KEY_A)) {
    p->current_animation = 1;
    p->flip_h = true;
    p->direction = -1;
    p->velocity.x = -p->speed;
  } else {
    p->current_animation = 0;
    p->flip_h = (p->direction == -1);
  }

  p->position.x += p->velocity.x * dt;

  if (player_collides_with_map(p, map, dt)) {
    p->position.x = old_position.x;
  }
}

void player_update(Player *p, float dt, Map *map) {
  player_move(p, dt, map);
  p->current_texture = p->textures[p->current_animation];
  player_animate(p, dt);
  player_jump(p, dt, map);
}

void player_cleanup(Player *p) {
  for (int i = 0; i <= 2; i++) {
    UnloadTexture(p->textures[i]);
  }
  UnloadTexture(p->current_texture);
}
