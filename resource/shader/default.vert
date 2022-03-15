#version 330 core
layout (location = 0) in vec3 aPos;
main() {
    gl_Position(aPos, 1.0);
}
