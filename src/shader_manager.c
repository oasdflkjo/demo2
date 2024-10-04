#include "shader_manager.h"
#include "gl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s (Error: %s)\n", filename, strerror(errno));
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    size_t read_length = fread(buffer, 1, length, file);
    if (read_length != (size_t)length) {
        fprintf(stderr, "Failed to read entire file: %s (Read %zu of %ld bytes)\n", 
                filename, read_length, length);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[length] = '\0';

    fclose(file);
    return buffer;
}

GLuint create_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        return 0;
    }

    return shader;
}

GLuint create_program(const char* vertex_file, const char* fragment_file) {
    char* vertex_source = read_file(vertex_file);
    char* fragment_source = read_file(fragment_file);

    if (!vertex_source || !fragment_source) {
        free(vertex_source);
        free(fragment_source);
        return 0;
    }

    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_source);

    free(vertex_source);
    free(fragment_source);

    if (!vertex_shader || !fragment_shader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

// Add this function to create compute shaders
GLuint create_compute_shader(const char* compute_file) {
    char* compute_source = read_file(compute_file);
    if (!compute_source) {
        return 0;
    }

    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, (const GLchar**)&compute_source, NULL);
    glCompileShader(compute_shader);

    GLint success;
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(compute_shader, 512, NULL, infoLog);
        fprintf(stderr, "Compute shader compilation failed: %s\n", infoLog);
        free(compute_source);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, compute_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Compute shader program linking failed: %s\n", infoLog);
        free(compute_source);
        return 0;
    }

    glDeleteShader(compute_shader);
    free(compute_source);

    return program;
}
