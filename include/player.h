#ifndef PLAYER_H
#define PLAYER_H

#define NUM_FRAMES 4
#define GRAVITY 0.5f

#include "map.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

typedef struct Player Player;

struct Player {
  float speed;
  Vector2 position;
  Vector2 velocity;
  bool flip_h;
  int direction; // Example: 1 for right, and -1 for left

  // gravity and jumping variables
  float gravity;
  float in_air_timer;
  float air_limit;
  float jump_height;
  bool jumped;
  bool grounded;

  // Animation variables
  Texture2D textures[3];
  Texture2D current_texture;
  int current_animation;
  float frame_width;
  float frame_height;
  int num_frames;
  int current_frame;
  float frame_rate;
  float frame_timer;
  Rectangle frame_rec;
};

void player_init(Player *p, float x, float y);
void player_draw(Player *p);

bool check_collision(Rectangle rect1, Rectangle rect2);
bool player_collides_with_map(Player *p, Map *map, float dt);

void player_animate(Player *p, float dt);
void player_move(Player *p, float dt, Map *map);
void player_jump(Player *p, float dt, Map *map);

int player_reset_position(Player *p);

void player_update(Player *p, float dt, Map *map);

void player_cleanup(Player *p);

#endif
