#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "gl_loader.h"

GLuint create_program(const char* vertex_file, const char* fragment_file);
GLuint create_compute_shader(const char* compute_file);

#endif // SHADER_MANAGER_H
