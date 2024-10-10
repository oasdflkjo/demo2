#include "input_manager.h"
#include <stdlib.h>

#define MAX_KEYS 256

struct InputManager {
    HWND window;
    bool keys[MAX_KEYS];
};

InputManager* input_manager_create(HWND window) {
    InputManager* im = malloc(sizeof(InputManager));
    if (im) {
        im->window = window;
        memset(im->keys, 0, sizeof(im->keys));
    }
    return im;
}

void input_manager_destroy(InputManager* im) {
    free(im);
}

void input_manager_update(InputManager* im) {
    for (int i = 0; i < MAX_KEYS; i++) {
        im->keys[i] = GetAsyncKeyState(i) & 0x8000;
    }
}

bool input_manager_is_key_pressed(InputManager* im, int key_code) {
    return im->keys[key_code];
}
