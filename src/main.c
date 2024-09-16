#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include "shader.h"

#define NO_OF_BOIDS 800

int scr_width = 1280;
int scr_height = 720;

float boid_size = 20.0f;

float protected_range = 8.0f;
float visible_range = 40.0f;
float avoid_fct = 0.05f;
float matching_fct = 0.05f;
float centering_fct = 0.0005f;
float turn_fct = 0.2f;
float max_speed = 6.0f;
float min_speed = 3.0f;
float max_bias = 0.01f;
float bias_increment = 0.00004f;

enum Group {
	RIGHT = 0,
	LEFT,
	BOTTOM,
	TOP,
};

struct Boid {
	vec3s pos;
	vec3s vel;

	float bias;
	enum Group group;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	scr_width = width;
	scr_height = height;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(int argc, char **argv) {
	if (!glfwInit()) {
		fprintf(stderr, "failed to initialize glfw");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "Conway's game of life", NULL, NULL);
	if (!window) {
		fprintf(stderr, "failed to create window");
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	glViewport(0, 0, scr_width, scr_height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	Shader shader_pg;
	shader_init(&shader_pg, "assets/shader.vert", "assets/shader.frag");

	float vertices[] = {
		0.5f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};

	GLuint boid_vao, boid_vbo;
	glGenBuffers(1, &boid_vbo);
	glGenVertexArrays(1, &boid_vao);

	glBindVertexArray(boid_vao);
	glBindBuffer(GL_ARRAY_BUFFER, boid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	struct Boid *boids = calloc(NO_OF_BOIDS, sizeof(struct Boid));

	srand(time(NULL));

	for (int i = 0; i < NO_OF_BOIDS; i++) {
		struct Boid *boid = &boids[i];
		boid->bias = 0.001;
		boid->group = i % 4;
	}

	while(!glfwWindowShouldClose(window)) {
		for (int i = 0; i < NO_OF_BOIDS; i++) {
			struct Boid *boid = &boids[i];

			vec3s avg_pos = {0}, avg_vel = {0}, close_d = {0};
			int visible_count = 0;

			for (int j = 0; j < NO_OF_BOIDS; j++) {
				struct Boid *boid_o = &boids[j];

				float distance = fabsf(glms_vec3_distance(boid->pos, boid_o->pos));

				if (distance < visible_range) {
					if (distance < protected_range) {
						close_d = glms_vec3_add(close_d, glms_vec3_sub(boid->pos, boid_o->pos));
					} else {
						avg_pos = glms_vec3_add(avg_pos, boid_o->pos);
						avg_vel = glms_vec3_add(avg_vel, boid_o->vel);

						visible_count++;
					}
				}
			}

			if (visible_count > 0) {
				avg_pos = glms_vec3_divs(avg_pos, visible_count);
				avg_vel = glms_vec3_divs(avg_vel, visible_count);

				boid->vel = glms_vec3_add(boid->vel, glms_vec3_add(glms_vec3_scale(glms_vec3_sub(avg_pos, boid->pos), centering_fct), glms_vec3_scale(glms_vec3_sub(avg_vel, boid->vel), matching_fct)));
			}

			boid->vel = glms_vec3_add(boid->vel, glms_vec3_scale(close_d, avoid_fct));

			if (boid->pos.y < 100) {
				boid->vel.y += turn_fct;
			} else if (boid->pos.y > scr_height - 100) {
				boid->vel.y -= turn_fct;
			}

			if (boid->pos.x < 100) {
				boid->vel.x += turn_fct;
			} else if (boid->pos.x > scr_width - 100) {
				boid->vel.x -= turn_fct;
			}

			if (boid->group == RIGHT) {
				if (boid->vel.x > 0) {
					boid->bias = glm_min(max_bias, boid->bias + bias_increment);
				} else {
					boid->bias = glm_max(bias_increment, boid->bias - bias_increment);
				}
			} else if (boid->group == LEFT) {
				if (boid->vel.x < 0) {
					boid->bias = glm_min(max_bias, boid->bias + bias_increment);
				} else {
					boid->bias = glm_max(bias_increment, boid->bias - bias_increment);
				}
			} else if (boid->group == BOTTOM) {
				if (boid->vel.y > 0) {
					boid->bias = glm_min(max_bias, boid->bias + bias_increment);
				} else {
					boid->bias = glm_max(bias_increment, boid->bias - bias_increment);
				}
			} else if (boid->group == TOP) {
				if (boid->vel.y < 0) {
					boid->bias = glm_min(max_bias, boid->bias + bias_increment);
				} else {
					boid->bias = glm_max(bias_increment, boid->bias - bias_increment);
				}
			}

			if (boid->group == RIGHT) {
				boid->vel.x = (1 - boid->bias)*boid->vel.x + (boid->bias * 1);
			} else if (boid->group == LEFT) {
				boid->vel.x = (1 - boid->bias)*boid->vel.x + (boid->bias * -1);
			} else if (boid->group == BOTTOM) {
				boid->vel.y = (1 - boid->bias)*boid->vel.y + (boid->bias * 1);
			} else if (boid->group == TOP) {
				boid->vel.y = (1 - boid->bias)*boid->vel.y + (boid->bias * -1);
			}

			float speed = glms_vec3_distance((vec3s){0}, boid->vel);

			if (speed < min_speed) {
				boid->vel = glms_vec3_scale(glms_vec3_divs(boid->vel, speed), min_speed);
			} else if (speed > max_speed) {
				boid->vel = glms_vec3_scale(glms_vec3_divs(boid->vel, speed), max_speed);
			}

			boid->pos = glms_vec3_add(boid->pos, boid->vel);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_pg);
		glBindVertexArray(boid_vao);

		mat4 model, projection;
		glm_ortho(0.0f, scr_width, scr_height, 0.0f, -1.0f, 1.0f, projection);
		shader_set_mat4(shader_pg, "projection", projection);

		for (int i = 0; i < NO_OF_BOIDS; i++) {
			struct Boid *boid = &boids[i];

			vec3 normed_vel;
			glm_vec3_normalize_to(boid->vel.raw, normed_vel);

			glm_mat4_identity(model);
			glm_translate(model, boid->pos.raw);
			glm_scale(model, (vec3){boid_size, boid_size, 0.0f});
			glm_rotate(model, atan2(normed_vel[1], normed_vel[0]) + glm_rad(90), (vec3){0.0f, 0.0f, 1.0f});
			shader_set_mat4(shader_pg, "model", model);

			if (boid->group == RIGHT) {
				shader_set_vec4(shader_pg, "color", (vec4){1.0f, 1.0f, 0.0f, 1.0f});
			} else if (boid->group == LEFT) {
				shader_set_vec4(shader_pg, "color", (vec4){0.0f, 1.0f, 1.0f, 1.0f});
			} else if (boid->group == BOTTOM) {
				shader_set_vec4(shader_pg, "color", (vec4){0.0f, 1.0f, 0.0f, 1.0f});
			} else if (boid->group == TOP) {
				shader_set_vec4(shader_pg, "color", (vec4){1.0f, 0.0f, 0.0f, 1.0f});
			}

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free(boids);

	glDeleteBuffers(1, &boid_vbo);
	glDeleteVertexArrays(1, &boid_vao);
	glDeleteProgram(shader_pg);
	glfwTerminate();

	return 0;
}

