#version 330 core
// same version as the vertex shader

// the one output, whatever i write here becomes the color of the pixel (r, g, b, a)
out vec4 FragColor;

// main runs once per pixel the triangle covers, after rasterization figured out which pixels those are
void main() {
  // every covered pixel gets the same orange so the triangle is flat orange
  // each number goes from 0.0 to 1.0, alpha 1.0 = fully solid
  FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
