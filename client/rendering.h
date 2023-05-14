#ifndef _RENDERING_H
#define _RENDERING_H

#include <raylib.h>
#include "utf8.h"

#define DEFAULT_FONT_SIZE 30
#define DEFAULT_FONT_SPACING 1

typedef struct Renderer {
    float scale;
    Vector2 zero_point;
    Font font;
    float initial_w;
    float initial_h;
} Renderer;

Renderer init_renderer(int w, int h);
void handle_resize_event(Renderer *renderer);

void render_rect(Renderer *renderer, int x, int y, int width, int height, Color color);
void render_rect_bottom_indep_x(Renderer *renderer, int height, Color color);
void render_utf8_bottom(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color);
void render_utf8_bottom_unscaled(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color);
void render_utf8_indep(Renderer *renderer, utf8_t *text, int font_size, int x, int y, Color color);

Rectangle scale_rect_for_renderer(Rectangle a, Renderer *renderer);
Vector2 measure_text_utf8(Renderer *renderer, int font_size, utf8_t *str);

#endif
