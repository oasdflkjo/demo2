#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <windows.h>
#include <stdbool.h>

typedef struct InputManager InputManager;

InputManager* input_manager_create(HWND window);
void input_manager_destroy(InputManager* im);
void input_manager_update(InputManager* im);
bool input_manager_is_key_pressed(InputManager* im, int key_code);

#endif // INPUT_MANAGER_H
