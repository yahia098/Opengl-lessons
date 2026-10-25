#version 330 core
// the version line has to come first, 330 core matches the opengl 3.3 core context i asked glfw for

// input attribute 0, the location number is the link to glVertexAttribPointer(0, ...) on the cpu side
layout (location = 0) in vec3 aPos;

// main runs once per vertex, the gpu calls it for every vertex in parallel
void main() {
  // gl_Position is the built in output i have to set, a vec4 (x, y, z, w)
  // vec4(aPos, 1.0) turns the vec3 into a vec4, w = 1.0 means the perspective divide changes nothing
  gl_Position = vec4(aPos, 1.0);
}
