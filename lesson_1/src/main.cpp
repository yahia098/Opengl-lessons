#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Could Not Create a OpenGL Window" << std::endl;
    glfwTerminate();
    return 1;
  }
  // load opengl functions via glad
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glViewport(0, 0, 800, 800);
  // graphics stuff now

  // loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClearColor(0.5f, 0.12f, 0, 0.65f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
  }
  // terminate and clean up
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
