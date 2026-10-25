#version 330 core

// location 0 matches glVertexAttribPointer(0, ...) on the cpu side
layout (location = 0) in vec3 aPos;

void main() {
  // gl_Position needs a vec4, w = 1.0 because this is a point not a direction
  gl_Position = vec4(aPos, 1.0);
}
