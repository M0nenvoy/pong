#include "setup_opengl.h"

#include <stdio.h>
#include <malloc.h>

/// Setup the boilerplate
int setup_opengl(GLFWwindow*& window) {
    printf("Initiating glfw...\n");
    if (!glfwInit()) {
        fputs("ERROR:GLFW:INIT", stderr);
        return -1;
    }

    printf("Creating the glfw window");

    // glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    if (!window) {
        fputs("ERROR:GLFW:CREATE_WINDOW\n", stderr);
        const char** error_desc = (const char**)malloc(1);
        error_desc[0] = (const char*) malloc(512);

        glfwGetError(error_desc);
        fprintf(stderr, "%s\n", error_desc[0]);
        return -2;
    }

    glfwMakeContextCurrent(window);
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    glewExperimental = true;

    printf("Initiating glew...\n");
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        fputs("ERROR:GLEW:INIT\n", stderr);
        const GLubyte* error_desc = glewGetErrorString(error);
        fprintf(stderr, "Error desc: %s\n", error_desc);
        return -3;
    }

    return 0;
}
