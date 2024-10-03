#include "input_manager.h"
#include <stdlib.h>

struct InputManager {
    HWND window;
    MousePosition mouse_position;
    bool keys[256];
};

InputManager* input_manager_create(HWND window) {
    InputManager* input_manager = (InputManager*)malloc(sizeof(InputManager));
    if (input_manager) {
        input_manager->window = window;
        input_manager->mouse_position.x = 0;
        input_manager->mouse_position.y = 0;
        memset(input_manager->keys, 0, sizeof(input_manager->keys));
    }
    return input_manager;
}

void input_manager_destroy(InputManager* input_manager) {
    free(input_manager);
}

void input_manager_update(InputManager* input_manager) {
    POINT cursor_pos;
    if (GetCursorPos(&cursor_pos)) {
        if (ScreenToClient(input_manager->window, &cursor_pos)) {
            input_manager->mouse_position.x = cursor_pos.x;
            input_manager->mouse_position.y = cursor_pos.y;
        }
    }

    // Update key states
    for (int i = 0; i < 256; i++) {
        input_manager->keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

MousePosition input_manager_get_mouse_position(InputManager* input_manager) {
    return input_manager->mouse_position;
}

bool input_manager_is_key_pressed(InputManager* input_manager, int key) {
    return input_manager->keys[key];
}
