#include "input.h"
#include <stdio.h>
#include <memory.h>

// Input mask
static char MASK = 0;
constexpr char LEFT_PADDLE_DOWN     = 0b10000000;
constexpr char RIGHT_PADDLE_DOWN    = 0b01000000;
constexpr char LEFT_PADDLE_UP       = 0b00100000;
constexpr char RIGHT_PADDLE_UP      = 0b00010000;
constexpr char RESET_INPUT          = 0b00001000;


Input INPUT = {0};
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    char mask = 0;

    switch (key) {
        case(GLFW_KEY_R):
            mask = RESET_INPUT;
            break;
        case(GLFW_KEY_J):
            mask = RIGHT_PADDLE_DOWN;
            break;
        case(GLFW_KEY_K):
            mask = RIGHT_PADDLE_UP;
            break;
        case(GLFW_KEY_A):
            mask = LEFT_PADDLE_DOWN;
            break;
        case(GLFW_KEY_S):
            mask = LEFT_PADDLE_UP;
            break;
    }
    switch (action) {
        case(GLFW_PRESS):
            MASK = MASK | mask;
            break;
        case(GLFW_RELEASE):
            MASK = MASK & (MASK ^ mask);
            break;
    }
    memset(&INPUT, 0, sizeof(INPUT));

    if (MASK & LEFT_PADDLE_UP)      INPUT.ldir              =  1;
    if (MASK & LEFT_PADDLE_DOWN)    INPUT.ldir              = -1;
    if (MASK & RIGHT_PADDLE_UP)     INPUT.rdir              =  1;
    if (MASK & RIGHT_PADDLE_DOWN)   INPUT.rdir              = -1;
    if (MASK & RESET_INPUT)         INPUT.should_restart    =  1;
}
