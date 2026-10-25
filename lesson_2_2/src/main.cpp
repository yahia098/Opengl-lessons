#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
std::string readFile(const char *path) {
  std::ifstream file(path);
  if (!file) {
    std::cout << "Couldn't open " << path << '\n';
    return "";
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

GLuint compileShader(GLenum type, const char *path) {
  std::string src = readFile(path);
  const char *code = src.c_str();
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, nullptr);
  glCompileShader(shader);

  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    std::cout << path << ":\n" << log << '\n';
  }
  return shader;
}
int main() {
  if (!glfwInit()) {
    std::cout << "Couldn't initialize GLFW\n";
    return 1;
  }
  // tell glfw to tell opengl to use at least opengl 4.6 core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // create a window
  GLFWwindow *Window =
      glfwCreateWindow(1280, 720, "LearnOpenGL", nullptr, nullptr);

  if (!Window) {
    std::cout << "Couldn't create window\n";
    glfwTerminate();
    return 1;
  }
  // tell glfw to use this window
  glfwMakeContextCurrent(Window);
  // load opengl functions with glad 2.0
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Couldn't load OpenGL functions\n";
    glfwDestroyWindow(Window);
    glfwTerminate();
    return 1;
  }

  std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';

  glViewport(0, 0, 1280, 720);
  glClearColor(0.12f, 0.55f, 0.87f, 1.0f);
  // vertices
  std::vector<GLfloat> Vertices = {
      -0.5f, -0.5f, // first
      0.5f,  -0.5f, // second
      0.5f,  0.5f,  // third
      -0.5f, 0.5f   // fourth

  };
  std::vector<GLuint> Elements = {0, 1, 3, 1, 2, 3};
  GLuint VertexBufferObject, ElementBufferObject;
  glGenBuffers(1, &VertexBufferObject);
  glGenBuffers(1, &ElementBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat),
               Vertices.data(), GL_STATIC_DRAW);

  GLuint VertexArrayObject;
  glGenVertexArrays(1, &VertexArrayObject);
  glBindVertexArray(VertexArrayObject);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(GLuint),
               Elements.data(), GL_STATIC_DRAW);
  GLuint vert = compileShader(GL_VERTEX_SHADER, "shaders/shader.vert");
  GLuint frag = compileShader(GL_FRAGMENT_SHADER, "shaders/shader.frag");

  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  GLint ok;
  glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    std::cout << "Link error:\n" << log << '\n';
  }
  glDeleteShader(vert);
  glDeleteShader(frag);
  // loop
  while (!glfwWindowShouldClose(Window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VertexArrayObject);
    glUseProgram(program);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glfwSwapBuffers(Window);
    glfwPollEvents();
  }

  glfwDestroyWindow(Window);
  glfwTerminate();
  return 0;
}
