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

int setup_shaders(GLuint *shader_program) {
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

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        fprintf(stderr, "ERROR:SHADER:LINK %s\n", infoLog);
    }

    glUseProgram(program);

    // Goto clean
OUT:
    free(shader_v);
    free(shader_f);

    *shader_program = program;

    return status;
}

template <typename GLType>
void VBO(GLuint layout_id, GLuint VBO, GLuint component_type, GLType verticies[], int arrsize, int component_size, int stride, long int offset = 0) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, arrsize, verticies, GL_STATIC_DRAW);
    glVertexAttribPointer(layout_id, component_size, GL_FLOAT, GL_FALSE, stride, (void*) offset);
    glEnableVertexAttribArray(layout_id);
}

// @returns VAO id
GLuint* setup_VAO() {
    GLuint *VAO, *VBOp, VBOc, EBO;
    VAO     = (GLuint*) malloc(sizeof(GLuint) * 2);
    VBOp    = (GLuint*) malloc(sizeof(GLuint) * 2);

    /// Triangle 1
    glm::vec3 t1[] = {
        {  0.0f, -0.5f, 0.0f },
        { -0.5f,  0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
    };

    /// Trianle 2
    glm::vec3 t2[] = {
        { -0.8f, -0.5f, 0.0f },
        { -0.8f,  0.5f, 0.0f },
        { -0.6f,  0.5f, 0.0f },
    };

    glm::vec3 clr[] = {
        {  1.0f,  0.0f, 0.0f }, // Red color
        {  0.0f,  1.0f, 0.0f }, // Green color
        {  0.0f,  0.0f, 1.0f }, // Blue color
    };

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBOp);
    glGenBuffers(1, &VBOc);

    glBindVertexArray(VAO[0]);
    VBO(0, VBOp[0], GL_FLOAT, t1, sizeof(t1), 3, sizeof(float) * 3);
    VBO(1, VBOc, GL_FLOAT, clr, sizeof(clr), 3, sizeof(float) * 3);

    glBindVertexArray(VAO[1]);
    VBO(0, VBOp[1], GL_FLOAT, t2, sizeof(t2), 3, sizeof(float) * 3);
    VBO(1, VBOc, GL_FLOAT, clr, sizeof(clr), 3, sizeof(float) * 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

void fetch_errors() {
    fputs("Fetching errors...\n", stderr);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "%x\n", err);
    }
}

// Update window's viewport after resizing
void resize_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
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

    glfwSetFramebufferSizeCallback(window, resize_callback);

    glViewport(0, 0, WIDTH, HEIGHT);

    GLuint* VAO = setup_VAO();
    int VAO0 = VAO[0];
    int VAO1 = VAO[1];

    GLuint shader_program;

    status = setup_shaders(&shader_program);
    if (status) return terminate(status);

    fetch_errors();

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shader_program);
        render();

        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glBindVertexArray(VAO0);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    terminate(0);
}
