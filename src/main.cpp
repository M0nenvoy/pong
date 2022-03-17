#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <GL/eglew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include "util/file.h"

Resource RESOURCE;

static float BG_COLOR[] = {0.2f, 0.2f, 0.2f, 1.0f};

// Window
constexpr int WIDTH     = 400;
constexpr int HEIGHT    = 300;
constexpr char const* TITLE = "Sample text";

void render() {
    glClearColor(BG_COLOR[0], BG_COLOR[1], BG_COLOR[2], BG_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLuint compile_shader(const char *const source, int length, GLuint type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    return shader;
}

int setup_shaders() {
    // Vertex shader
    char* shader_v = (char*) malloc(512);
    char* shader_f = (char*) malloc(512);

    int vbytes; // Bytes of vertex shader
    int fbytes; // Bytes of frag   shader

    GLuint shf; // Handle to a fragment shader
    GLuint shv; // Handle to a vertex shader
    GLuint program; // Handle to a shader program

    int status;
    {
        status = get_resource("shader/default.vert", shader_v, vbytes);
        if (status) goto OUT;

        status = get_resource("shader/default.frag", shader_f, fbytes);
        if (status) goto OUT;
    }
    shf = compile_shader(shader_f, fbytes, GL_FRAGMENT_SHADER);
    shv = compile_shader(shader_v, vbytes, GL_VERTEX_SHADER);

    program = glCreateProgram();
    glAttachShader(program, shv);
    glAttachShader(program, shf);
    glLinkProgram(program);

    // Goto clean
OUT:
    free(shader_v);
    free(shader_f);

    return status;
}

// @returns VAO id
GLuint setup_VAO() {
    GLuint VAO;
    glGenBuffers(1, &VAO);

    glm::vec3 verticies[] = {
        { -0.5f, -0.5f, 1.0f },
        {  0.0f,  0.5f, 1.0f },
        {  0.5f, -0.5f, 1.0f },
    };

    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);


    return VAO;
}


// Supply the path to the 'resources' folder via command line
// arguments.
int main(int argc, char **argv) {
    /*
    if (argc < 2) {
        fputs("Usage: app <path/to/the/resource_dir>", stderr);
        return -1;
    }

    // Save resource dir to a static storage.
    {
        int bytes = strlen(argv[1]); // Bytes used by an argument ( including the '\0' )

        // Check if the user added the '/' at the end
        if (argv[1][bytes - 1] != '/') {
            argv[1][bytes + 0] = '/';
            argv[1][bytes + 1] = 0;

            bytes++;
        }
        RESOURCE.BYTES = bytes;
        memcpy(RESOURCE.DIR, argv[1], bytes);
    }

    */

    printf("Initiating glfw...\n");
    if (!glfwInit()) {
        fputs("ERROR:GLFW:INIT", stderr);
        return -1;
    }

    printf("Creating the glfw window");

    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
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

    setup_VAO();
    // setup_shaders();

    glViewport(0, 0, WIDTH, HEIGHT);
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}
