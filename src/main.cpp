#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include "util/file.h"
#include "setup_opengl.h"
#include "input.h"
#include "collision.h"

Resource RESOURCE;

struct Ball {
    glm::vec2 pos;
    glm::vec2 vel;
};

/** Convert pixel coordinates to clip coordinates
 * @param window_size Size of the window along the converted axis
 */
inline float to_clip(int coord, int window_size) {
    return (((float) coord / (float) window_size) - 0.5f) * 2.0f;
}

/**
 * Generate the verticies for the rectangle
 * @param w Width in pixels
 * @param h Height in pixels
 * @param window_w Window width in pixels
 * @param window_h Window height in pixels
 * @param posx Position x of the top left pixel
 * @param posy Position y of the top left pixel
 * @param ptr store generated verticies here
 */
template <typename T>
void gen_rectangle_verticies(
        T w,
        T h,
        T window_w,
        T window_h,
        T posx,
        T posy,
        glm::vec3* ptr) {

    float posxcl = to_clip(posx,        window_w); // Left clip position
    float posxcr = to_clip(posx + w,    window_w); // Right clip position
    float posyct = to_clip(posy,        window_h); // Top clip position
    float posycb = to_clip(posy + h,    window_h); // Bottom clip position

    glm::vec3 verticies[4] = {
        {  posxcl,      posyct,      0.0f },
        {  posxcl,      posycb,      0.0f },
        {  posxcr,      posycb,      0.0f },
        {  posxcr,      posyct,      0.0f },
    };
    memcpy(ptr, verticies, sizeof(verticies));
}

constexpr int PADDLE_W = 10; // Width of the paddle in pixels
constexpr int PADDLE_H = 50; // Height of the paddle in pixels

constexpr int BALL_W        = 10;
constexpr int BALL_H        = 10;
constexpr float BALL_SPEED  = 10.0f;
constexpr float SPEED_MOD   = 1.2f;

constexpr int PADDING = 30;  // Distance from the edge of the screen in pixels


constexpr float PADSPEED = 30.0f;
/**
 * Update the paddles according to the current input.
 * @param lpp Left paddle position vertically
 * @param rpp Right paddle position vertically
 * @param delta_time Time between two last updates
 * @returns If any displacement is present
 */
bool update_paddles(Input INPUT, int &lpp, int &rpp, float delta_time) {
    static float remainder_r {0};
    static float remainder_l {0};

    float ldisplacement = (INPUT.ldir) * PADSPEED;
    float rdisplacement = (INPUT.rdir) * PADSPEED;

    float flr_dis = ldisplacement * delta_time + remainder_l; // Real left displacement
    float frr_dis = rdisplacement * delta_time + remainder_r; // Real right displacement

    int lr_dis = floor(flr_dis);
    int rr_dis = floor(frr_dis);

    remainder_r = frr_dis - rr_dis;
    remainder_l = flr_dis - lr_dis;
                          // + 5 for bias                             // + 5 for the same reason
    lr_dis *= !(lpp + lr_dis + 5 < 0 || lpp + lr_dis > HEIGHT - PADDLE_H + 5);    // Don't get out of the screen
    rr_dis *= !(rpp + rr_dis + 5 < 0 || rpp + rr_dis > HEIGHT - PADDLE_H + 5);    // Don't get out of the screen

    lpp += lr_dis;
    rpp += rr_dis;

    return (lr_dis != 0 || rr_dis != 0);
}

void update_ball(Ball* ball, int lpad, int rpad, float delta_time) {

    // Move the ball->with its velocity
    ball->pos += ball->vel * delta_time;

    AABB ball_aabb      = { ball->pos, { BALL_W, BALL_H } };
    AABB lpaddle_aabb   = { { PADDING, lpad }, {PADDLE_W, PADDLE_H} };
    AABB rpaddle_aabb   = { { WIDTH - PADDING - PADDLE_W, rpad }, {PADDLE_W, PADDLE_H} };
    // Check if the ball->collided with the left paddle
    // if (ball->vel.x < 0 && ball->pos.x < PADDING + PADDLE_W                // Collide horizontally
    //     && (ball->pos.y < lpad + PADDLE_H && ball->pos.y + BALL_H > lpad)) // Collide vertically
    // {
    //     ball->vel.x = -ball->vel.x;
    //     return;
    if (collision(ball_aabb, lpaddle_aabb)) {
        ball->vel.x = -ball->vel.x;
        glm::vec2 random_v = { ball->vel.x, ball->vel.y };
        ball->vel += random_v;
    }
    if (collision(ball_aabb, rpaddle_aabb)) {
        ball->vel.x = SPEED_MOD * -ball->vel.x;
    }
    // }
    // Check if the ball->collided with the right paddle
    // if (ball->vel.x > 0 && ball->pos.x + BALL_W > (WIDTH - PADDING - PADDLE_W)      // Collidre horizontally
    //     && (ball->pos.y < rpad + PADDLE_H && ball->pos.y + BALL_H > rpad))          // Collide vertically
    // {
    //     ball->vel.x = -ball->vel.x;
    //     return;
    // }

    // Check if the ball->collided with the ceiling
    if (ball->pos.y < 0) {
        ball->vel.y = -ball->vel.y;
        return;
    }
    // Check if the ball->collided with the floor
    if (ball->pos.y + BALL_H > HEIGHT) {
        ball->vel.y = -ball->vel.y;
        return;
    }
}

/// Set the positions of all the objects to the default ones
void reset(int & lpad, int & rpad, Ball * ball) {
    lpad = (HEIGHT - PADDLE_H) >> 1;
    rpad = lpad;

    ball->pos.x = (float) ((WIDTH  - BALL_W) >> 1);
    ball->pos.y = (float) ((HEIGHT - BALL_H) >> 1);
    // Reset the velocity of a ball
    ball->vel = glm::vec2(-1.0f) * BALL_SPEED;
}

static float BG_COLOR[] = {0.2f, 0.2f, 0.2f, 1.0f};

int terminate(int status) {
    glfwTerminate();
    return status;
}

void render(int VAO) {
    glClearColor(BG_COLOR[0], BG_COLOR[1], BG_COLOR[2], BG_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
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
    GLuint program = 0; // Handle to a shader program

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

/**
 * @param verticies Verticies containing two paddles and a ball
 * @param size size of verticies
 */
GLuint
setup_VAO(VBO& vbo, glm::vec3* verticies, int size) {
    GLuint VBOhandle, EBO;

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOhandle);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    GLuint indicies[] = {
        // Left paddle
        0, 1, 2,
        2, 3, 0,

        // Right paddle
        4, 5, 6,
        6, 7, 4,

        // Ball
        8, 9, 10,
        10, 11, 8
    };

    /// Rectangle points
    vbo = {
        .layout_id      = 0,
        .handle         = VBOhandle,
        .component_type = GL_FLOAT,
        .arrsize        = size,
        .component_size = 3,
        .stride         = sizeof(float) * 3,
        .verticies      = verticies,
        .draw_mode      = GL_DYNAMIC_DRAW,
    };

    vbo.init();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

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
    glfwSetKeyCallback(window, key_callback);

    glm::vec3 verticies[12];

    // Setup the paddles and the ball
    int lpad, rpad; // Positions of the top left pixel of the left and the right paddle
    Ball ball;

    reset(lpad, rpad, &ball);

    // Left paddle
    gen_rectangle_verticies(
        PADDLE_W,
        PADDLE_H,
        WIDTH,
        HEIGHT,
        PADDING,
        lpad,
        verticies);

    // Right paddle
    gen_rectangle_verticies(
        PADDLE_W,
        PADDLE_H,
        WIDTH,
        HEIGHT,
        WIDTH - PADDING - PADDLE_W,
        rpad,
        verticies + 4);
    // Ball
    gen_rectangle_verticies<float>(
        BALL_W,
        BALL_H,
        WIDTH,
        HEIGHT,
        ball.pos.x,
        ball.pos.y,
        verticies + 8);

    VBO vbo;
    GLuint VAOhandle = setup_VAO(vbo, verticies, sizeof(verticies));
    GLuint shader_program;

    status = setup_shaders(&shader_program);
    if (status) return terminate(status);

    glUseProgram(shader_program);

    // Time at the startup
    float time0 = glfwGetTime();
    float time = 0;

    fetch_errors();
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        if (INPUT.should_restart) reset(lpad, rpad, &ball);
        // Time since the startup
        float delta_time = glfwGetTime() - time;
        time = glfwGetTime() - time0;

        update_paddles(INPUT, lpad, rpad, delta_time);
        update_ball(&ball, lpad, rpad, delta_time);

        // Left paddle
        gen_rectangle_verticies(PADDLE_W, PADDLE_H, WIDTH, HEIGHT, PADDING, lpad, verticies);

        // Right paddle
        gen_rectangle_verticies(PADDLE_W, PADDLE_H, WIDTH, HEIGHT, WIDTH - PADDING - PADDLE_W, rpad, verticies + 4);

        // Ball
        gen_rectangle_verticies<float>(BALL_W, BALL_H, WIDTH, HEIGHT, ball.pos.x, ball.pos.y, verticies + 8);

        // Supply VBO with new data
        glBindBuffer(GL_ARRAY_BUFFER, vbo.handle);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies), verticies);

        render(VAOhandle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    terminate(0);
}
