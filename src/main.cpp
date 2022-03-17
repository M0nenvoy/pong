#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <glm/vec3.hpp>

#include "util/file.h"
#include "setup_opengl.h"

Resource RESOURCE;

static float BG_COLOR[] = {0.2f, 0.2f, 0.2f, 1.0f};

int terminate(int status) {
    glfwTerminate();
    return status;
}

void render() {
    glClearColor(BG_COLOR[0], BG_COLOR[1], BG_COLOR[2], BG_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLuint compile_shader(const char *const source, GLint length, GLuint type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char error_desc[512];
        glGetShaderInfoLog(shader, 512, nullptr, error_desc);
        fprintf(stderr, "ERROR:SHADER:COMPILE %s\n", error_desc);
        fprintf(stderr, "%s\n", source);

        return 0;
    }

    return shader;
}

int setup_shaders() {
    // Vertex shader
    char* shader_v = (char*) malloc(512);
    char* shader_f = (char*) malloc(512);

    GLint vbytes; // Bytes of vertex shader
    GLint fbytes; // Bytes of frag   shader

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
    if (!shf) { status = -5; goto OUT; }
    shv = compile_shader(shader_v, vbytes, GL_VERTEX_SHADER);
    if (!shv) { status = -6; goto OUT; }

    program = glCreateProgram();
    glAttachShader(program, shv);
    glAttachShader(program, shf);
    glLinkProgram(program);

    glUseProgram(program);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, verticies);
    glEnableVertexAttribArray(0);

    return VAO;
}


// Supply the path to the 'resources' folder via command line
// arguments.
int main(int argc, char **argv) {
    if (argc < 2) {
        fputs("Usage: app <path/to/the/resource_dir>\n", stderr);
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

    // Window
    GLFWwindow* window;

    int status = setup_opengl(window);
    if (status) return terminate(status);

    setup_VAO();

    status = setup_shaders();
    if (status) return terminate(status);

    glViewport(0, 0, WIDTH, HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    terminate(0);
}
