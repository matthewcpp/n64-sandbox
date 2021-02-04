#include "framework64/renderer.h"

#include "framework64/matrix.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

void renderer_init(Renderer* renderer, int screen_width, int screen_height) {
    nuGfxInit(); // starts nusys graphics

    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;
    renderer->display_list = NULL;

    renderer->fill_color = GPACK_RGBA5551(255, 255, 255, 1);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    /*
      The viewport structure 
      The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
    */
    Vp view_port = {
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* The scale factor  */
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* Move  */
    };

    renderer->view_port = view_port;
}

void renderer_set_clear_color(Renderer* renderer, Color* clear_color) {
    renderer->clear_color = GPACK_RGBA5551(clear_color->r, clear_color->g, clear_color->b, 1);
}

void renderer_init_rdp(Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++,G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gDPSetTextureLOD(renderer->display_list++,G_TL_TILE);
    gDPSetTextureLUT(renderer->display_list++,G_TT_NONE);
    gDPSetTextureDetail(renderer->display_list++,G_TD_CLAMP);
    gDPSetTexturePersp(renderer->display_list++,G_TP_PERSP);
    gDPSetTextureFilter(renderer->display_list++,G_TF_BILERP);
    gDPSetTextureConvert(renderer->display_list++,G_TC_FILT);
    gDPSetCombineMode(renderer->display_list++,G_CC_SHADE, G_CC_SHADE);
    gDPSetCombineKey(renderer->display_list++,G_CK_NONE);
    gDPSetAlphaCompare(renderer->display_list++,G_AC_NONE);
    gDPSetRenderMode(renderer->display_list++,G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetColorDither(renderer->display_list++,G_CD_DISABLE);
    gDPSetDepthImage(renderer->display_list++,nuGfxZBuffer);
    gDPPipeSync(renderer->display_list++);
}

void renderer_init_rsp(Renderer* renderer) {
    gSPViewport(renderer->display_list++, &renderer->view_port);
    gSPClearGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
}

void renderer_clear_z_buffer(Renderer* renderer) {
    //gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    
    gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
}

void renderer_clear_frame_buffer(Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr));
    gDPSetFillColor(renderer->display_list++, (renderer->clear_color << 16 | renderer->clear_color));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE); 
}

void renderer_begin(Renderer* renderer, Camera* camera, RenderMode render_mode, RendererFlags flags) {
    renderer->render_mode = render_mode;

    if (flags & RENDERER_FLAG_CLEAR) {
        // set the display list pointer to the beginning
        renderer->display_list = &renderer->gfx_list[0];
    }
    renderer->display_list = &renderer->gfx_list[0];
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    renderer_init_rdp(renderer);
    renderer_init_rsp(renderer);

    if (flags & RENDERER_FLAG_CLEAR) {
        renderer_clear_z_buffer(renderer);
        renderer_clear_frame_buffer(renderer);
    }

    if (render_mode == RENDERER_MODE_TRIANGLES) {
        gDPSetRenderMode(renderer->display_list++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
    }
    else{
        gDPSetRenderMode(renderer->display_list++,  G_RM_AA_ZB_XLU_LINE,  G_RM_AA_ZB_XLU_LINE2);
    }

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->perspNorm);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    gDPPipeSync(renderer->display_list++);
    
}

void renderer_end(Renderer* renderer, RendererFlags flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->render_mode == RENDERER_MODE_TRIANGLES) ? NU_GFX_UCODE_F3DEX : NU_GFX_UCODE_L3DEX2, 
        (flags & RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
}

float entity_matrix[4][4];

void renderer_entity_start(Renderer* renderer, Entity* entity){
    matrix_from_trs((float*)entity_matrix, &entity->transform.position, &entity->transform.rotation, &entity->transform.scale);
    guMtxF2L(entity_matrix, &entity->dl_matrix);

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(entity->dl_matrix)), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
}

void renderer_entity_end(Renderer* renderer) {
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    gDPPipeSync(renderer->display_list++);
}

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

void renderer_begin_2d(Renderer* renderer) {
    gSPClearGeometryMode(renderer->display_list++, 0xFFFFFFFF);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gDPSetCombineKey(renderer->display_list++, G_CK_NONE);
    gDPSetAlphaCompare(renderer->display_list++, G_AC_NONE);
    gDPSetRenderMode(renderer->display_list++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetColorDither(renderer->display_list++, G_CD_DISABLE);
    gDPPipeSync(renderer->display_list++);
}

void renderer_set_fill_color(Renderer* renderer, Color* color) {
    renderer->fill_color = GPACK_RGBA5551(color->r, color->g, color->b, 255);

    if (renderer->display_list) {
        gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    }
}

void renderer_set_fill_mode(Renderer* renderer) {
    gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    //TODO: do i need to sync pipe here?
}

void renderer_draw_filled_rect(Renderer* renderer, IRect* rect) {
    gDPFillRectangle(renderer->display_list++, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    gDPPipeSync(renderer->display_list++);
}

void renderer_set_sprite_mode(Renderer* renderer){
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(renderer->display_list++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
    gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
    //TODO: do i need to sync pipe here?
}

void renderer_draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y) {
    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);

    int top_left_x = (frame % sprite->hslices) * slice_width;
    int top_left_y = (frame / sprite->hslices) * slice_height;

    gDPLoadTextureBlock(renderer->display_list++, sprite->slices[frame], G_IM_FMT_RGBA, G_IM_SIZ_16b, slice_width, slice_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPLoadSync(renderer->display_list++);

    gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + slice_width) << 2, (y + slice_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

    gDPPipeSync(renderer->display_list++);
}

void renderer_draw_sprite(Renderer* renderer, ImageSprite* sprite, int x, int y) {
    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);
    int slice = 0;

    for (uint8_t row = 0; row < sprite->hslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->vslices; col++) {
            int draw_x = x + slice_width * col;

            renderer_draw_sprite_slice(renderer, sprite, slice++, draw_x, draw_y);
        }
    }
}

void renderer_draw_text(Renderer* renderer, Font* font, int x, int y, char* text) {
    if (!text || text[0] == 0) return;

/*
    TODO: Color blending? need better alpha than 5551
    gDPSetPrimColor(renderer->display_list++, 255, 255, color->r, color->g, color->b, 255);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGBA_PRIM , G_CC_MODULATERGBA_PRIM );
*/
    
    char ch = text[0];
    uint16_t glyph_index = font_get_glyph_index(font, ch);
    FontGlyph* glyph = font->glyphs + glyph_index;
    uint16_t stride = font->spritefont_tile_width * font->spritefont_tile_height * 2;

    int caret = x + glyph->left;

    while (ch) {
        glyph_index = font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        gDPLoadTextureBlock(renderer->display_list++, font->spritefont + (stride * glyph_index), 
        G_IM_FMT_RGBA, G_IM_SIZ_16b, 
        font->spritefont_tile_width, font->spritefont_tile_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPLoadSync(renderer->display_list++);

        gSPTextureRectangle(renderer->display_list++, 
            draw_pos_x << 2, draw_pos_y << 2, 
            (draw_pos_x + font->spritefont_tile_width) << 2, (draw_pos_y + font->spritefont_tile_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

        gDPPipeSync(renderer->display_list++);

        caret += glyph->advance;
        text++;
        ch = text[0];
    }
}