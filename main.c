#include <stdio.h>
#include <raylib.h>
#include "player.h"
#include "map.h"

void camera_init(Camera2D camera, Vector2 position) {
}

int main(int argc, char** argv) {
  // Constants
  const Color bgColor = {135, 206, 233};
  const int screenWidth = 800;
  const int screenHeight = 600;
  Player p;
  Camera2D camera;
  Map map;

  // Initialization
  InitWindow(screenWidth, screenHeight, "Game");

  init_map(&map);

  player_init(&p, (float)screenWidth/2, (float)screenHeight/2);

  camera.target = p.position;
  camera.offset = (Vector2){screenWidth/2, screenHeight/2};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f;

  SetTargetFPS(60);

  load_map(&map);

  // main loop
  while (!WindowShouldClose()) {
    // update stuff here
    float dt = GetFrameTime();
    player_update(&p, dt);

    camera.target = p.position;

    // render stuff here
    BeginDrawing();

    ClearBackground(bgColor);

    BeginMode2D(camera);
      draw_map(&map);
      player_draw(&p);
    EndMode2D();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
