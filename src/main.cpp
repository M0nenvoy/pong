#include <stdio.h>
#include <string.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// - OH, no, you can't have global state - it's so evil.
// - Ha ha static go brrr
static char RESOURCE_DIR[512];
static int  RESOURCE_DIR_BYTES;
static float BG_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};

// Window
constexpr int WIDTH     = 400;
constexpr int HEIGHT    = 300;
constexpr char const* TITLE = "Sample text";

void render() {
    glClearColor(BG_COLOR[0], BG_COLOR[1], BG_COLOR[2], BG_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

/// @param path Path to the resource
char* get_resource(char* path, int bytes) {
    memcpy(RESOURCE_DIR + 1, path, bytes);

}

// Supply the path to the 'resources' folder via command line
// arguments.
int main(int argc, char **argv) {
    if (argc < 2) {
        fputs("Usage: app <path/to/the/resource_dir>", stderr);
    }

    // Save resource dir to a static storage.
    {
        int bytes = strlen(argv[1]);
        RESOURCE_DIR_BYTES = bytes;
        memcpy(RESOURCE_DIR, argv[1], bytes);
    }
    if (!glfwInit()) {
        fputs("ERROR:GLFW:INIT", stderr);
        return -1;
    }
    if (!glewInit()) {
        fputs("ERROR:GLEW:INIT", stderr);
        return -2;
    }

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glViewport(0, 0, WIDTH, HEIGHT);
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}
