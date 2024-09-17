cmake_minimum_required(VERSION 3.30)
project(boids)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC
	glad
	glfw
	cglm_headers
	m
)

# GLAD
set(GLAD_SOURCES_DIR "${PROJECT_SOURCE_DIR}/vendor/glad/")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" EXCLUDE_FROM_ALL)
glad_add_library(glad REPRODUCIBLE API gl:core=4.6)

# GLFW
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/glfw/" EXCLUDE_FROM_ALL)

# GLM
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/cglm/ EXCLUDE_FROM_ALL)

