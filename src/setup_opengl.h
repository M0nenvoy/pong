#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/// Window properties
constexpr int WIDTH         = 800;
constexpr int HEIGHT        = 450;
constexpr char const* TITLE = "Sample text";

int setup_opengl(GLFWwindow*& window);
