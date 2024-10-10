#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <windows.h>
#include <stdbool.h>

typedef struct WindowManager WindowManager;


WindowManager* wm_create(void);
int wm_init(WindowManager* window_manager);
HWND wm_get_window(WindowManager* window_manager);
void wm_shutdown(WindowManager* window_manager);
void wm_destroy(WindowManager* window_manager);

int wm_get_width(WindowManager* window_manager);
int wm_get_height(WindowManager* window_manager);

#endif // WINDOW_MANAGER_H
