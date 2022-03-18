#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "util/file.h"
#include "setup_opengl.h"

Resource RESOURCE;

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

/// Rectangle
glm::vec3 RECTANGLE[] = {
    { -0.5f, -0.5f, 0.0f }, // Top left
    { -0.5f,  0.5f, 0.0f }, // Bottom left
    {  0.5f,  0.5f, 0.0f }, // Bottom right
    {  0.5f, -0.5f, 0.0f }, // Top right
};

glm::vec3 tlmotion(glm::vec3 v, float gravity) {
    glm::vec3 k = { 0.0f, 0.0f, 1.0f };
    glm::vec3 copy = v;
    glm::normalize(v);
    glm::vec3 cross = glm::cross(v, k);
    return copy + cross * gravity;
}

glm::vec3 clr[] = {
    {  1.0f,  0.0f, 0.0f }, // Red color
    {  0.0f,  1.0f, 0.0f }, // Green color
    {  0.0f,  0.0f, 1.0f }, // Blue color
    {  1.0f,  1.0f, 1.0f }, // White color
};

static float BG_COLOR[] = {0.2f, 0.2f, 0.2f, 1.0f};

int terminate(int status) {
    glfwTerminate();
    return status;
}

void render(int VAO0, int VAO1) {
    glClearColor(BG_COLOR[0], BG_COLOR[1], BG_COLOR[2], BG_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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

struct VBO {
    GLuint      layout_id;
    GLuint      handle;
    GLuint      component_type;
    int         arrsize;
    int         component_size;
    int         stride;
    void*       verticies;
    long int    offset = 0;
    GLuint      draw_mode = GL_STATIC_DRAW;

    void init() {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, arrsize, verticies, draw_mode);
        glVertexAttribPointer(layout_id, component_size, GL_FLOAT, GL_FALSE, stride, (void*) offset);
        glEnableVertexAttribArray(layout_id);
    }
};

/// @returns VBO array pointer
struct VBO*
setup_VAO(GLuint VAO[2]) {
    GLuint VBOp[2], VBOc, EBO;

    struct VBO * VBOs = (struct VBO*) malloc((sizeof(struct VBO) * 3));

    glGenVertexArrays(2, VAO);
    glGenBuffers(3, VBOp);
    glGenBuffers(1, &VBOc);
    glGenBuffers(1, &EBO);

    GLuint indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    /// Rectangle points
    VBOs[0] = {
        .layout_id      = 0,
        .handle         = VBOp[1],
        .component_type = GL_FLOAT,
        .arrsize        = sizeof(RECTANGLE),
        .component_size = 3,
        .stride         = sizeof(float) * 3,
        .verticies      = RECTANGLE,
        .draw_mode      = GL_DYNAMIC_DRAW,
    };

    /// Rectangle colors
    VBOs[1] = {
        .layout_id      = 1,
        .handle         = VBOc,
        .component_type = GL_FLOAT,
        .arrsize        = sizeof(clr),
        .component_size = 3,
        .stride         = sizeof(float) * 3,
        .verticies      = clr,
        .draw_mode      = GL_DYNAMIC_DRAW,
    };

    // Bind vertex array
    // glBindVertexArray(VAO[0]);
    // vbop1.bind();
    // vboc.bind();

    // Setup EBO

    glBindVertexArray(VAO[0]);
    VBOs[0].init();
    VBOs[1].init();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    return VBOs;
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

    GLuint VAO[2];
    struct VBO* vbos = setup_VAO(VAO);
    int VAO0 = VAO[0];
    int VAO1 = VAO[1];

    GLuint shader_program;

    status = setup_shaders(&shader_program);
    if (status) return terminate(status);

    fetch_errors();

    glUseProgram(shader_program);

    /// Time at the startup
    float time0 = glfwGetTime();
    float gravity = 0.001f;
    glm::vec3 motion = {0.0f, 1.0f, 0.0f};
    motion *= gravity;

    float clock_speed = 1.2f;
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        /// Time since the startup
        float time = glfwGetTime() - time0;

        motion = clock_speed * tlmotion(motion, gravity);
        RECTANGLE[0] += motion;
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0].handle);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float), RECTANGLE);
        render(VAO0, VAO1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free (vbos);
    terminate(0);
}
