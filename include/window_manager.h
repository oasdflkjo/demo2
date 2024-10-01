#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <windows.h>
#include <stdbool.h>

typedef struct WindowManager WindowManager;

WindowManager* window_manager_create(void);
int window_manager_initialize(WindowManager* window_manager);
bool window_manager_update(WindowManager* window_manager);  // Add this line
void window_manager_shutdown(WindowManager* window_manager);
void window_manager_destroy(WindowManager* window_manager);
HWND window_manager_get_window(WindowManager* window_manager);
int window_manager_should_close(WindowManager* window_manager);
HDC window_manager_get_dc(WindowManager* window_manager);
void window_manager_swap_buffers(WindowManager* window_manager);

#endif // WINDOW_MANAGER_H
