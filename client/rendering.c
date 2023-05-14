#include "rendering.h"

#include <stdlib.h>

#define min(a, b) ((a) < (b) ? a : b)

Renderer init_renderer(int w, int h) {
    Renderer res = { 0 };
    res.scale = 1.0f;
    res.font = LoadFontEx("resources/SourceCodePro-Medium.ttf", DEFAULT_FONT_SIZE, NULL, 500);
    res.initial_w = w;
    res.initial_h = h;

    return res;
}

void handle_resize_event(Renderer *renderer) {
    renderer->scale = min(GetScreenWidth()/renderer->initial_w, GetScreenHeight()/renderer->initial_h);
    renderer->zero_point = (Vector2){
        GetScreenWidth() / 2.0f - (renderer->scale * renderer->initial_w / 2),
        GetScreenHeight() / 2.0f - (renderer->scale * renderer->initial_h / 2),
    };
}

void render_rect(Renderer *renderer, int x, int y, int width, int height, Color color) {
    Rectangle rect = scale_rect_for_renderer((Rectangle){x,y,width,height}, renderer);
    DrawRectangleRec(rect, color);
}

void render_rect_bottom_indep_x(Renderer *renderer, int height, Color color) {
    Rectangle rect = { 0 };
    rect.height = height * renderer->scale;
    rect.width = GetScreenWidth();
    rect.x = 0;
    rect.y = GetScreenHeight() - rect.height;
    DrawRectangleRec(rect, color);
}

void render_utf8_bottom(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color) {
    float final_size = font_size * renderer->scale;
    float final_y = GetScreenHeight() - final_size - (y * renderer->scale);
    DrawTextCodepoints(renderer->font, text, utf8_len(text), (Vector2){x, final_y}, final_size, DEFAULT_FONT_SPACING, color);
}

void render_utf8_bottom_unscaled(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color) {
    float final_y = GetScreenHeight() - font_size - (y * renderer->scale);
    DrawTextCodepoints(renderer->font, text, utf8_len(text), (Vector2){x, final_y}, font_size, DEFAULT_FONT_SPACING, color);
}

void render_utf8_indep(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color) {
    DrawTextCodepoints(renderer->font, text, utf8_len(text), (Vector2){x, y}, font_size, DEFAULT_FONT_SPACING, color);
}

Rectangle scale_rect_for_renderer(Rectangle a, Renderer *renderer) {
    Vector2 zero_point = renderer->zero_point;
    float scale = renderer->scale;
    return (Rectangle){a.x * scale + zero_point.x, a.y * scale + zero_point.y, a.width * scale, a.height * scale};
}

Vector2 measure_text_utf8(Renderer *renderer, int font_size, utf8_t *str) {
    char *as_cstr = LoadUTF8(str, utf8_len(str));
    Vector2 res = MeasureTextEx(renderer->font, as_cstr, font_size, DEFAULT_FONT_SPACING);
    UnloadUTF8(as_cstr);

    return res;
}
