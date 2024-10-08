#include "window_manager.h"
#include "gl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <windows.h>

#pragma GCC diagnostic ignored "-Wcast-function-type"

#define WINDOW_TITLE "OpenGL Particle System"
#define WINDOW_CLASS_NAME "OpenGLWindow"

struct WindowManager
{
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    int width;
    int height;
};

static int g_should_close = 0;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        g_should_close = 1;
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static BOOL SetupPixelFormat(HDC hdc)
{
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
        0, 0, 0};

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (!pf || !SetPixelFormat(hdc, pf, &pfd))
    {
        fprintf(stderr, "Failed to set pixel format\n");
        return FALSE;
    }
    return TRUE;
}

static HGLRC CreateOpenGLContext(HDC hdc)
{
    HGLRC tempContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempContext);

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB)
    {
        fprintf(stderr, "Failed to get wglCreateContextAttribsARB function\n");
        return NULL;
    }

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0};

    HGLRC hrc = wglCreateContextAttribsARB(hdc, 0, attribs);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempContext);
    wglMakeCurrent(hdc, hrc);

    return hrc;
}

WindowManager *window_manager_create(void)
{
    WindowManager *manager = calloc(1, sizeof(WindowManager));
    return manager;
}

int window_manager_initialize(WindowManager *wm)
{
    SetProcessDPIAware();

    WNDCLASSA wc = {
        .lpfnWndProc = WindowProc,
        .hInstance = GetModuleHandle(NULL),
        .lpszClassName = WINDOW_CLASS_NAME};

    if (!RegisterClassA(&wc))
    {
        fprintf(stderr, "Failed to register window class\n");
        return 0;
    }

    // Get the actual screen resolution
    DEVMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm) == 0)
    {
        fprintf(stderr, "Failed to get display settings\n");
        return 0;
    }
    wm->width = dm.dmPelsWidth;
    wm->height = dm.dmPelsHeight;

    wm->hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_APPWINDOW,
        WINDOW_CLASS_NAME, WINDOW_TITLE,
        WS_POPUP | WS_VISIBLE,
        0, 0, wm->width, wm->height,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!wm->hwnd)
    {
        fprintf(stderr, "Failed to create window\n");
        return 0;
    }

    wm->hdc = GetDC(wm->hwnd);

    if (!SetupPixelFormat(wm->hdc) || !(wm->hrc = CreateOpenGLContext(wm->hdc)))
    {
        return 0;
    }

    LoadOpenGLFunctions();

    ShowWindow(wm->hwnd, SW_SHOW);
    window_manager_hide_cursor(wm);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));

    return 1;
}

bool window_manager_update(WindowManager *wm)
{
    MSG msg;
    while (PeekMessage(&msg, wm->hwnd, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            g_should_close = 1;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return !g_should_close;
}

void window_manager_shutdown(WindowManager *wm)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(wm->hrc);
    ReleaseDC(wm->hwnd, wm->hdc);
    DestroyWindow(wm->hwnd);
}

void window_manager_destroy(WindowManager *wm)
{
    free(wm);
}

HWND window_manager_get_window(WindowManager *wm) { return wm->hwnd; }
HDC window_manager_get_dc(WindowManager *wm) { return wm->hdc; }
int window_manager_get_width(WindowManager *wm) { return wm->width; }
int window_manager_get_height(WindowManager *wm) { return wm->height; }

int window_manager_should_close(WindowManager *wm)
{
    (void)wm;
    return g_should_close;
}

void window_manager_swap_buffers(WindowManager *wm) { SwapBuffers(wm->hdc); }

void window_manager_hide_cursor(WindowManager *wm)
{
    (void)wm;
    ShowCursor(FALSE);
}