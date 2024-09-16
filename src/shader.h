#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <glad/gl.h>
#include <cglm/cglm.h>

typedef GLuint Shader;

void shader_init(Shader *s, const char *vert_path, const char *frag_path);

void shader_set_bool(Shader s, const char *name, bool v);
void shader_set_int(Shader s, const char *name, int v);
void shader_set_float(Shader s, const char *name, float v);

void shader_set_vec2(Shader s, const char *name, vec2 v);
void shader_set_2f(Shader s, const char *name, float x, float y);

void shader_set_vec3(Shader s, const char *name, vec3 v);
void shader_set_3f(Shader s, const char *name, float x, float y, float z);

void shader_set_vec4(Shader s, const char *name, vec4 v);
void shader_set_4f(Shader s, const char *name, float x, float y, float z, float w);

void shader_set_mat2(Shader s, const char *name, mat2 m);
void shader_set_mat3(Shader s, const char *name, mat3 m);
void shader_set_mat4(Shader s, const char *name, mat4 m);

#endif
