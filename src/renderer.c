#include "renderer.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include "particle_system.h"
#include <stdio.h>
#include <windows.h>

static GLuint cursor_vao, cursor_vbo;
static GLuint cursor_shader_program;
static GLuint fbo, rbo, screen_texture;
static GLuint quad_vao, quad_vbo;
static GLuint glitch_shader_program;
static LARGE_INTEGER start_time, frequency;
static GLuint particle_vao;
static GLuint particle_render_shader;

void renderer_init_cursor(void);
void renderer_init_framebuffer(int width, int height);
void renderer_init_fullscreen_quad(void);
void renderer_render_particles(ParticleSystem* ps);

int renderer_initialize(int width, int height) {
    renderer_init_cursor();
    renderer_init_framebuffer(width, height);
    renderer_init_fullscreen_quad();

    glitch_shader_program = create_program("shaders/fullscreen_quad.vert", "shaders/glitch.frag");
    if (!glitch_shader_program) {
        printf("Failed to create glitch shader program\n");
        return 0;
    }
    printf("Glitch shader program created successfully: %u\n", glitch_shader_program);

    // Initialize particle rendering
    glGenVertexArrays(1, &particle_vao);
    glBindVertexArray(particle_vao);
    glBindVertexArray(0);

    // Load particle render shader
    particle_render_shader = create_program("shaders/particle.vert", "shaders/particle.frag");
    if (!particle_render_shader) {
        fprintf(stderr, "Failed to create render shader for particle system\n");
        return 0;
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    return 1;
}


void renderer_shutdown(void) {
    glDeleteVertexArrays(1, &cursor_vao);
    glDeleteBuffers(1, &cursor_vbo);
    glDeleteProgram(cursor_shader_program);

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &screen_texture);

    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteProgram(glitch_shader_program);

    glDeleteVertexArrays(1, &particle_vao);
    glDeleteProgram(particle_render_shader);
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

void renderer_init_framebuffer(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is not complete!\n");
    } else {
        printf("Framebuffer created successfully with dimensions %dx%d\n", width, height);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer_init_fullscreen_quad(void) {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void renderer_start_frame(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void renderer_end_frame(void) {
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render full-screen quad with glitch effect
    glUseProgram(glitch_shader_program);
    glBindVertexArray(quad_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glUniform1i(glGetUniformLocation(glitch_shader_program, "screenTexture"), 0);

    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    float elapsed_time = (float)(current_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
    GLint timeLocation = glGetUniformLocation(glitch_shader_program, "time");
    glUniform1f(timeLocation, elapsed_time);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        printf("OpenGL error in renderer_end_frame: %d\n", error);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void renderer_render_particles(ParticleSystem* ps) {
    glUseProgram(particle_render_shader);
    glBindVertexArray(particle_vao);
    
    // Bind position SSBO to the VAO
    GLuint position_ssbo = particle_system_get_position_ssbo(ps);
    glBindBuffer(GL_ARRAY_BUFFER, position_ssbo);
    
    // Update attribute pointers (only position now)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, particle_system_get_max_particles(ps));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}