#include "window_manager.h"
#include "gl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <windows.h>

#pragma GCC diagnostic ignored "-Wcast-function-type"

#define WINDOW_TITLE "OpenGL Particle System"

struct WindowManager {
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    int width;
    int height;
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
        manager->width = 0;
        manager->height = 0;
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

    // Get the primary monitor's dimensions
    HMONITOR hmon = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { 
        .cbSize = sizeof(mi),
        .rcMonitor = {0}
    };
    if (!GetMonitorInfo(hmon, &mi)) {
        fprintf(stderr, "Failed to get monitor info\n");
        return 0;
    }

    int width = mi.rcMonitor.right - mi.rcMonitor.left;
    int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    // Create a borderless window that covers the entire screen
    window_manager->hwnd = CreateWindowExA(
        0,
        "OpenGLWindow",
        WINDOW_TITLE,
        WS_POPUP | WS_VISIBLE,  // Borderless fullscreen
        mi.rcMonitor.left, mi.rcMonitor.top, width, height,
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
    window_manager->width = width;
    window_manager->height = height;

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

    // Create a temporary context
    HGLRC tempContext = wglCreateContext(window_manager->hdc);
    wglMakeCurrent(window_manager->hdc, tempContext);

    // Load wglCreateContextAttribsARB function
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB) {
        fprintf(stderr, "Failed to get wglCreateContextAttribsARB function\n");
        return 0;
    }

    // Set attributes for OpenGL 4.6 core profile
    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    // Create OpenGL 4.6 core profile context
    window_manager->hrc = wglCreateContextAttribsARB(window_manager->hdc, 0, attribs);

    // Delete temporary context and make new context current
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempContext);
    wglMakeCurrent(window_manager->hdc, window_manager->hrc);

    // Load modern OpenGL functions
    LoadOpenGLFunctions();

    ShowWindow(window_manager->hwnd, SW_SHOW);

    // Print OpenGL version information
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    printf("OpenGL Version: %s\n", version);
    printf("OpenGL Renderer: %s\n", renderer);

    window_manager_hide_cursor(window_manager);

    return 1;
}

bool window_manager_update(WindowManager* window_manager) {
    MSG msg;
    while (PeekMessage(&msg, window_manager->hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            g_should_close = 1;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return !g_should_close;
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

HDC window_manager_get_dc(WindowManager* window_manager) {
    return window_manager->hdc;
}

// Add this function at the end of the file
void window_manager_swap_buffers(WindowManager* window_manager) {
    SwapBuffers(window_manager->hdc);
}

int window_manager_get_width(WindowManager* window_manager) {
    return window_manager->width;
}

int window_manager_get_height(WindowManager* window_manager) {
    return window_manager->height;
}

// Add this function
void window_manager_hide_cursor(WindowManager* window_manager) {
    (void)window_manager;  // Suppress unused parameter warning
    ShowCursor(FALSE);
}