#include "renderer.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include <stdio.h>
#include <windows.h>  // Add this include for Windows-specific functions

static GLuint cursor_vao, cursor_vbo;
static GLuint cursor_shader_program;

void renderer_init_cursor(void);  // Add this forward declaration

int renderer_initialize(void) {
    // Remove the shader program for the triangle
    // shader_program = create_program("shaders/basic.vert", "shaders/basic.frag");
    // if (!shader_program) {
    //     fprintf(stderr, "Failed to create shader program in renderer\n");
    //     // Print current working directory
    //     char cwd[MAX_PATH];
    //     if (GetCurrentDirectoryA(sizeof(cwd), cwd) != 0) {
    //         fprintf(stderr, "Current working directory: %s\n", cwd);
    //     } else {
    //         fprintf(stderr, "Failed to get current working directory\n");
    //     }
    //     return 0;
    // }

    // Remove the triangle vertex setup
    // float vertices[] = {
    //     -0.5f, -0.5f,
    //      0.5f, -0.5f,
    //      0.0f,  0.5f
    // };

    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);

    // glBindVertexArray(vao);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    renderer_init_cursor();

    return 1;
}

void renderer_render(void) {
    // Remove the triangle rendering
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    // glUseProgram(shader_program);
    // glBindVertexArray(vao);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
}

void renderer_shutdown(void) {
    // Remove the deletion of triangle-related objects
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
    // glDeleteProgram(shader_program);

    glDeleteVertexArrays(1, &cursor_vao);
    glDeleteBuffers(1, &cursor_vbo);
    glDeleteProgram(cursor_shader_program);
}

void renderer_init_cursor(void) {
    float cursor_vertices[] = {
         0.0f,  0.01f,
        -0.01f, -0.01f,
         0.01f, -0.01f
    };

    glGenVertexArrays(1, &cursor_vao);
    glGenBuffers(1, &cursor_vbo);

    glBindVertexArray(cursor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cursor_vertices), cursor_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    cursor_shader_program = create_program("shaders/cursor.vert", "shaders/cursor.frag");
}

void renderer_render_cursor(float x, float y) {
    glUseProgram(cursor_shader_program);
    
    GLint offsetUniform = glGetUniformLocation(cursor_shader_program, "offset");
    
    glUniform2f(offsetUniform, x, y);

    glBindVertexArray(cursor_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    
    glUseProgram(0);
}
