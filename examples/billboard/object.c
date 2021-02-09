#include "object.h"

Lights1 blue_cube_lights_0 = gdSPDefLights1(6, 104, 204, 125, 125, 125, 40, 40, 40);

const Vtx blue_cube_mesh_0[] = {
{ 1, 1, 1, 0, 0, 0, 0, 127, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ -1, -1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ 1, -1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ 1, 1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ -1, 1, -1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, -1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, 1, 1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, 0, -128, 0, 255 },
{ -1, -1, -1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, 1, 1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ 1, -1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ -1, -1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 0, -128, 255 },
};

const Gfx blue_cube_mesh_0_dl[] = {
gsSPSetLights1(blue_cube_lights_0),
gsSPVertex(blue_cube_mesh_0 + 0, 24, 0),
gsSP1Triangle(0, 1, 2, 0),
gsSP1Triangle(0, 2, 3, 0),
gsSP1Triangle(4, 5, 6, 0),
gsSP1Triangle(4, 6, 7, 0),
gsSP1Triangle(8, 9, 10, 0),
gsSP1Triangle(8, 10, 11, 0),
gsSP1Triangle(12, 13, 14, 0),
gsSP1Triangle(12, 14, 15, 0),
gsSP1Triangle(16, 17, 18, 0),
gsSP1Triangle(16, 18, 19, 0),
gsSP1Triangle(20, 21, 22, 0),
gsSP1Triangle(20, 22, 23, 0),
gsSPEndDisplayList()
};

void solid_object_draw(Entity* entity, Renderer* renderer) {
    renderer_entity_start(renderer, entity);
    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
    gSPDisplayList(renderer->display_list++, blue_cube_mesh_0_dl);

    renderer_entity_end(renderer);
}

#include "nintendo_seal_sprite.h"

void init_nintendo_seal_sprite(ImageSprite* sprite) {
    sprite->width = nintendo_seal_SPRITE_WIDTH;
    sprite->height = nintendo_seal_SPRITE_HEIGHT;
    sprite->hslices = nintendo_seal_SPRITE_HSLICES;
    sprite->vslices = nintendo_seal_SPRITE_VSLICES;
    sprite->slice_count = nintendo_seal_SPRITE_SLICE_COUNT;
    sprite->slices = &nintendo_seal_sprite_slices[0];
}

#include "n64_logo_sprite.h"

void init_n64_logo_sprite(ImageSprite* sprite) {
    sprite->width = n64_logo_SPRITE_WIDTH;
    sprite->height = n64_logo_SPRITE_HEIGHT;
    sprite->hslices = n64_logo_SPRITE_HSLICES;
    sprite->vslices = n64_logo_SPRITE_VSLICES;
    sprite->slice_count = n64_logo_SPRITE_SLICE_COUNT;
    sprite->slices = &n64_logo_sprite_slices[0];
}