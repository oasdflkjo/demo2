#ifndef RENDERER_H
#define RENDERER_H

int renderer_initialize(void);
void renderer_render(void);
void renderer_render_cursor(float x, float y);
void renderer_shutdown(void);
void renderer_init_cursor(void);  // Add this line

#endif // RENDERER_H
