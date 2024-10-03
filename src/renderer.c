#include "renderer.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include <stdio.h>
#include <windows.h>  // Add this include for Windows-specific functions

static GLuint vao, vbo;
static GLuint shader_program;

int renderer_initialize(void) {
    shader_program = create_program("shaders/basic.vert", "shaders/basic.frag");
    if (!shader_program) {
        fprintf(stderr, "Failed to create shader program in renderer\n");
        // Print current working directory
        char cwd[MAX_PATH];
        if (GetCurrentDirectoryA(sizeof(cwd), cwd) != 0) {
            fprintf(stderr, "Current working directory: %s\n", cwd);
        } else {
            fprintf(stderr, "Failed to get current working directory\n");
        }
        return 0;
    }

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return 1;
}

void renderer_render(void) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void renderer_shutdown(void) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
}
