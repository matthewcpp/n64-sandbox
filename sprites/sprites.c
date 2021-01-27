#include "sprites.h"

#include "n64_logo_sprite.h"
#include "ken_sprite.h"

#include <malloc.h>

void n64_logo_sprite_init(N64LogoSprite* logo_sprite) {
    logo_sprite->position.x = 0;
    logo_sprite->position.y = 0;

    logo_sprite->sprite.width = n64_logo_SPRITE_WIDTH;
    logo_sprite->sprite.height = n64_logo_SPRITE_HEIGHT;
    logo_sprite->sprite.hslices = n64_logo_SPRITE_HSLICES;
    logo_sprite->sprite.vslices = n64_logo_SPRITE_VSLICES;
    logo_sprite->sprite.slice_count = n64_logo_SPRITE_SLICE_COUNT;
    logo_sprite->sprite.slices = n64_logo_sprite_slices;
}

void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, Renderer* renderer){
    renderer_draw_sprite(renderer, &logo_sprite->sprite, logo_sprite->position.x, logo_sprite->position.y);
}

#define FRAME_DUR 1.0f / 10.0f

void ken_sprite_init(KenSprite* ken) {
    ken->position.x = 0;
    ken->position.y = 0;
    ken->frame_time = 0.0f;
    ken->frame_speed = 1.0f;
    ken->frame_index = 0;

    ken->sprite.width = ken_SPRITE_WIDTH;
    ken->sprite.height = ken_SPRITE_HEIGHT;
    ken->sprite.hslices = ken_SPRITE_HSLICES;
    ken->sprite.vslices = ken_SPRITE_VSLICES;
    ken->sprite.slice_count = ken_SPRITE_SLICE_COUNT;
    ken->sprite.slices = ken_sprite_slices;
}

void ken_sprite_update(KenSprite* ken, float time_delta) {
    ken->frame_time += time_delta;

    if (ken->frame_time >= FRAME_DUR) {
        ken->frame_time -= FRAME_DUR;

        ken->frame_index += 1;
        if (ken->frame_index >= ken->sprite.hslices)
            ken->frame_index = 0;
    }

}

void ken_sprite_draw(KenSprite* ken, Renderer* renderer) {
    int slice_height = image_sprite_get_slice_height(&ken->sprite);

    renderer_draw_sprite_slice(renderer, &ken->sprite, ken->frame_index, ken->position.x, ken->position.y);
    renderer_draw_sprite_slice(renderer, &ken->sprite, ken->frame_index + ken->sprite.hslices, ken->position.x, ken->position.y + slice_height);
}
