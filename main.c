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
  RenderTexture2D map_fb;

  // Initialization
  InitWindow(screenWidth, screenHeight, "Game");

  render_tmxmap_to_frame_buf("assets/map.tmx", &map_fb);

  player_init(&p, (float)screenWidth/2, (float)screenHeight/2);

  camera.target = p.position;
  camera.offset = (Vector2){screenWidth/2, screenHeight/2};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f;

  SetTargetFPS(60);

  int **collision_map = generate_collision_map("assets/map.tmx");
  if (!collision_map) {
    CloseWindow();
    return -1;
  }

  // main loop
  while (!WindowShouldClose()) {
    // update stuff here
    float dt = GetFrameTime();
    player_update(&p, dt, collision_map);

    camera.target = p.position;

    // render stuff here
    BeginDrawing();

    ClearBackground(bgColor);

    BeginMode2D(camera);
      draw_map(&map_fb);
      player_draw(&p);
    EndMode2D();

    EndDrawing();
  }

  UnloadRenderTexture(map_fb);
  unload_collision_map(collision_map);
  CloseWindow();
  return 0;
}
