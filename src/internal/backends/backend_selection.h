#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct GLFWmonitor GLFWmonitor;
typedef struct GLFWwindow GLFWwindow;
typedef struct rlr_backend_t rlr_backend_t;

bool rlr_internal_backend_selection(GLFWwindow** window, rlr_backend_t** backend, GLFWmonitor* monitor, uint32_t width, uint32_t height, const char* title);