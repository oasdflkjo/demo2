#include "window_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <gl/gl.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "OpenGL Particle System"

struct WindowManager {
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
};

static int g_should_close = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            g_should_close = 1;
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowManager* window_manager_create(void) {
    WindowManager* manager = malloc(sizeof(WindowManager));
    if (manager) {
        manager->hwnd = NULL;
        manager->hdc = NULL;
        manager->hrc = NULL;
    }
    return manager;
}

int window_manager_initialize(WindowManager* window_manager) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "OpenGLWindow";
    
    if (!RegisterClassA(&wc)) {
        fprintf(stderr, "Failed to register window class\n");
        return 0;
    }

    window_manager->hwnd = CreateWindowExA(
        0,
        "OpenGLWindow",
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!window_manager->hwnd) {
        fprintf(stderr, "Failed to create window\n");
        return 0;
    }

    window_manager->hdc = GetDC(window_manager->hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pf = ChoosePixelFormat(window_manager->hdc, &pfd);
    if (!pf) {
        fprintf(stderr, "Failed to choose pixel format\n");
        return 0;
    }

    if (!SetPixelFormat(window_manager->hdc, pf, &pfd)) {
        fprintf(stderr, "Failed to set pixel format\n");
        return 0;
    }

    window_manager->hrc = wglCreateContext(window_manager->hdc);
    if (!window_manager->hrc) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        return 0;
    }

    if (!wglMakeCurrent(window_manager->hdc, window_manager->hrc)) {
        fprintf(stderr, "Failed to make OpenGL context current\n");
        return 0;
    }

    ShowWindow(window_manager->hwnd, SW_SHOW);

    return 1;
}

void window_manager_update(WindowManager* window_manager) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    SwapBuffers(window_manager->hdc);
}

void window_manager_shutdown(WindowManager* window_manager) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(window_manager->hrc);
    ReleaseDC(window_manager->hwnd, window_manager->hdc);
    DestroyWindow(window_manager->hwnd);
}

void window_manager_destroy(WindowManager* window_manager) {
    if (window_manager) {
        free(window_manager);
    }
}

HWND window_manager_get_window(WindowManager* window_manager) {
    return window_manager->hwnd;
}

int window_manager_should_close(WindowManager* window_manager) {
    MSG msg;
    while (PeekMessage(&msg, window_manager->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return g_should_close;
}
