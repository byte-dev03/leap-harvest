#include "player.h"
#include <stdio.h>
#include <math.h>

#define TILE_HEIGHT 32
#define TILE_WIDTH 32

void player_init(Player *p, float x, float y) {
  p->speed = 200.0f;
  p->velocity = (Vector2){0, 0};
  p->position = (Vector2){ x, y };
  p->direction = 1;

  p->textures[0] = LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Idle_4.png"),
  p->textures[1] = LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Run_6.png"),
  p->textures[2] = LoadTexture("assets/2 Owlet_Monster/Owlet_Monster_Jump_8.png"),

  p->current_animation = 0;
  p->current_texture = p->textures[p->current_animation];
  p->current_frame = 0;

  p->frame_width = (float)(p->current_texture.width/NUM_FRAMES);
  p->frame_height = (float)(p->current_texture.height);
  p->frame_rec = (Rectangle){
    0, 0, p->frame_width, p->frame_height };

  p->num_frames = 4;
  p->frame_rate = 0.1f;
  p->frame_timer = 0.0f;

  p->tile_x = (int)(p->position.x / TILE_WIDTH);
  p->tile_y = (int)(p->position.y / TILE_HEIGHT);

  p->jumped = false;
  p->gravity = 9.81f;
  p->grounded = true;
  p->jump_height = 1.0f;
  p->flip_h = false;

}

void player_draw(Player *p) {
  Rectangle source_rec = p->frame_rec;
  Rectangle dest_rec = { p->position.x, p->position.y, p->frame_width, p->frame_height };
  Vector2 origin = { 0.0f, 0.0f };

  if (p->flip_h) {
      source_rec.width *= -1;  // Flip horizontally
  }

  DrawTexturePro(p->current_texture, source_rec, dest_rec, origin, 0.0f, WHITE);
}

void player_animate(Player *p, float dt) {
  p->frame_timer += dt;

  if (p->frame_timer >= p->frame_rate) {
      p->frame_timer = 0.0f;
      p->current_frame++;

      switch (p->current_animation) { 
        case 0:
          p->num_frames = 4;
          break;
        case 1:
          p->num_frames = 6;
          break;
        case 2:
          p->num_frames = 8;
          break;
        default:
          p->num_frames = 1;
          break;
      }

      if (p->current_frame >= p->num_frames) {
          p->current_frame = 0;
      }

  }

  p->frame_rec.x = p->frame_width*p->current_frame;
  p->frame_rec.y = 0;
}

void player_jump(Player *p, float dt, int **collision_map) {
  float initial_velocity = sqrtf(2.0f * p->jump_height * fabsf(p->gravity));

  if (IsKeyDown(KEY_SPACE) && p->grounded) {
    p->velocity.y = -initial_velocity;
    p->jumped = true;
    p->grounded = false;
    p->current_animation = 2;
    p->flip_h = (p->direction == -1);
  }

  
  if (p->jumped) {
    p->velocity.y += p->gravity * dt;
    p->position.y += p->velocity.y;
  }

  if (p->position.y >= 289) {
    p->position.y = 289;
    p->velocity.y = 0;
    p->jumped = false;
    p->grounded = true;
    p->current_animation = 0;
    p->flip_h = (p->direction == -1);
  }

  // Check collision with tiles below the player
  int player_tile_x = p->tile_x;
  int player_tile_y = p->tile_y - 1;

  if (collision_map[player_tile_y][player_tile_x] == 0) {
    // No tile below, allow player to fall
    p->velocity.y += p->gravity * dt;
    p->position.y += p->velocity.y;
    p->grounded = false;
  } else {
    // There's a tile below, stop falling
    p->position.y = (player_tile_y - 1) * TILE_HEIGHT; // Adjust player position to be on top of the tile
    p->velocity.y = 0;
    p->jumped = false;
    p->grounded = true;
    p->current_animation = 0;

  }
}
void player_move(Player *p, float dt) {
  p->velocity.x = 0;
  if (IsKeyDown(KEY_D)) {
    p->current_animation = 1;
    p->flip_h = false;
    p->direction = 1;
    p->velocity.x = p->speed;
  }
  else if (IsKeyDown(KEY_A)) {
    p->current_animation = 1;
    p->flip_h = true;
    p->direction = -1;
    p->velocity.x = -p->speed;
  } else {
    p->current_animation = 0;
    p->flip_h = (p->direction == -1);
  }

  p->position.x += p->velocity.x * dt;
}

void player_update(Player *p, float dt, int **collision_map) {
  player_move(p, dt);
  p->current_texture = p->textures[p->current_animation];
  player_animate(p, dt);
  player_jump(p, dt, collision_map);
  // Update tile coordinates based on new position
  p->tile_x = (int)(p->position.x / TILE_WIDTH);
  p->tile_y = (int)(p->position.y / TILE_HEIGHT);
}

void player_cleanup(Player *p) {
  for (int i = 0; i <= 2; i++) {
    UnloadTexture(p->textures[i]);
  }
  UnloadTexture(p->current_texture);
}
