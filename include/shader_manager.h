#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "gl_loader.h"

GLuint create_shader(GLenum type, const char* source);
GLuint create_program(const char* vertex_source, const char* fragment_source);

#endif // SHADER_MANAGER_H
