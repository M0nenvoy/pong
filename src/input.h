#pragma once

#include <GLFW/glfw3.h>

struct Input {
    // Left pad
    int ldir;
    // Right pad
    int rdir;
    
    int should_restart;

} extern INPUT;
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
