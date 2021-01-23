#include "sprites_example.h"

void sprites_example_init(SpritesExample* example, Input* input, Renderer* renderer) {
    example->input = input;
    example->renderer = renderer;

    camera_init(&example->camera);

    n64_logo_sprite_init(&example->n64logo);
    example->n64logo.position.x = 10;
    example->n64logo.position.y = 10;

    ken_sprite_init(&example->ken_sprite);

    IVec2 screen_size;
    renderer_get_screen_size(example->renderer, &screen_size);
    example->ken_sprite.position.x = 10;
    example->ken_sprite.position.y = screen_size.y - 10 - example->ken_sprite.sprite.height;
}

void sprites_example_update(SpritesExample* example, float time_delta){
    ken_sprite_update(&example->ken_sprite, time_delta);
}

void sprites_example_draw(SpritesExample* example){
    renderer_begin(example->renderer, &example->camera);
    renderer_begin_2d(example->renderer);
    renderer_set_sprite_mode(example->renderer);
    n64_logo_sprite_draw(&example->n64logo, example->renderer);

    ken_sprite_draw(&example->ken_sprite, example->renderer);

    renderer_end(example->renderer);
}