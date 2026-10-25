#version 330 core

out vec4 FragColor;

// set from the cpu with glUniform4f every frame
// the name must match glGetUniformLocation(shaderProgram, "ourColor") exactly
uniform vec4 ourColor;

void main() {
  FragColor = ourColor;
}
