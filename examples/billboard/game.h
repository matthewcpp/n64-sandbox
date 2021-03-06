#ifndef GAME_H
#define GAME_H

#include "framework64/util/fps_camera.h"
#include "framework64/util/quad.h"

#include "framework64/entity.h"
#include "framework64/system.h"

typedef struct {
    System* system;
    FpsCamera fps;
    Entity blue_cube;
    Entity nintendo_seal;
    Entity n64_logo;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif