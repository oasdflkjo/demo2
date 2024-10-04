#ifndef RENDERER_H
#define RENDERER_H

int renderer_initialize(void);
void renderer_shutdown(void);
void renderer_render_cursor(float x, float y);
void renderer_start_frame(void);
void renderer_end_frame(void);

#endif // RENDERER_H
