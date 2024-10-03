#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <windows.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
} MousePosition;

typedef struct InputManager InputManager;

InputManager* input_manager_create(HWND window);
void input_manager_destroy(InputManager* input_manager);
void input_manager_update(InputManager* input_manager);
MousePosition input_manager_get_mouse_position(InputManager* input_manager);
bool input_manager_is_key_pressed(InputManager* input_manager, int key);

#endif // INPUT_MANAGER_H
