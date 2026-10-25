// uniforms: a triangle whose green channel pulses with time, color comes from
// the cpu every frame
//
// build (from the project root, with the glad folder containing glad.h and
// glad.c in the root):
//   g++ src/main.cpp glad/glad.c -I. -o app -lglfw -ldl
// -I. makes "glad/glad.h" resolve from the project root, -ldl is for glad
// loading functions at runtime
//
// run it from the project root too: the shader paths are relative to where i
// run it, not to main.cpp
//   ./app
//
// shaders/frag.glsl needs the uniform declared in it:
//   #version 330 core
//   out vec4 FragColor;
//   uniform vec4 ourColor;
//   void main() {
//     FragColor = ourColor;
//   }

// glad MUST come before glfw or glfw includes the system gl header and they
// fight
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// when the window is resized the framebuffer changes but the viewport doesnt
// so i update the viewport myself, glfw calls this for me
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// asks glfw if a key is pressed, escape sets the should close flag so the loop
// ends
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// reads a whole file into a string
// why: opengl wants the shader as text and >> would stop at the first space
// how: copy the file buffer into a stringstream then take the string out of it
std::string readFile(const std::string &path) {
  std::ifstream file(path);
  // wrong path or ran from the wrong folder ends up here instead of a silent
  // empty shader
  if (!file) {
    std::cout << "Failed to open " << path << std::endl;
    return "";
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

// compiles one shader from its source text
// why: every gpu vendor has its own machine code so the driver compiles the
// glsl at runtime
GLuint compileShader(GLenum type, const std::string &source, const char *name) {
  // make an empty shader object, all i get back is an id
  GLuint shader = glCreateShader(type);

  // glShaderSource wants a pointer to a const char* so i keep the pointer in a
  // variable and pass its address the string has to stay alive until the
  // compile call because this pointer points inside it
  const char *src = source.c_str();

  // 1 = one string, NULL = the string is null terminated so no lengths needed
  // this only attaches the text, nothing is compiled yet
  glShaderSource(shader, 1, &src, NULL);

  // now the driver actually compiles it
  glCompileShader(shader);

  // a glsl typo doesnt crash anything, the shader is just broken and nothing
  // draws so i ask for the status and print the log, it usually tells the exact
  // line
  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    std::cout << name << " shader error:\n" << log << std::endl;
  }
  return shader;
}

// builds the full shader program from two files
// why: the gpu cant run a single shader alone, the pipeline needs vertex +
// fragment joined together
GLuint createProgram(const std::string &vertexPath,
                     const std::string &fragmentPath) {
  // read both files and compile them
  GLuint vertexShader =
      compileShader(GL_VERTEX_SHADER, readFile(vertexPath), "Vertex");
  GLuint fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, readFile(fragmentPath), "Fragment");

  // a program is the complete pipeline, i activate it with glUseProgram later
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  // attaching alone does nothing, linking connects the vertex outputs to the
  // fragment inputs and makes one executable pipeline (easy to forget this one)
  glLinkProgram(program);

  // same silent failure problem as compiling so i check the link status too
  GLint ok;
  glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetProgramInfoLog(program, 512, NULL, log);
    std::cout << "Link error:\n" << log << std::endl;
  }

  // the program already holds the linked result so the separate shaders are not
  // needed anymore delete them AFTER linking, not before
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

int main() {
  // ---------- window and context ----------

  // glfw has to be initialized first or no other glfw function works
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return 1;
  }

  // hints must come BEFORE creating the window because they describe what kind
  // of context to build i ask for at least opengl 3.3 (the driver can give a
  // newer one)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // core profile = only the modern shader way, no old glBegin/glEnd stuff
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // width then height then title, the 2 NULLs are: monitor (NULL = windowed)
  // and a window to share objects with (NULL = nobody)
  GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
  // creating a window can fail (driver doesnt support 3.3, no display...) and
  // using a NULL pointer would crash
  if (window == NULL) {
    std::cout << "Could Not Create a OpenGL Window" << std::endl;
    glfwTerminate();
    return 1;
  }

  // the window made a context but it isnt active yet
  // every gl call works on the current context so this must come before glad
  // and any gl call
  glfwMakeContextCurrent(window);

  // load opengl functions via glad
  // why: the functions live in the gpu driver so their addresses are only known
  // at runtime, until this they are NULL
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return 1;
  }

  // viewport maps opengl's -1 to 1 coordinates (normalized device coordinates)
  // to the window pixels
  glViewport(0, 0, 800, 800);
  // and keep it right when the window gets resized
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // ---------- shaders ----------

  // read, compile and link the shaders from the shaders folder into one program
  GLuint shaderProgram =
      createProgram("shaders/vertex.glsl", "shaders/frag.glsl");

  // ---------- uniform ----------

  // a uniform is a global variable in the shader that i set from the cpu
  // why: same value for every vertex/fragment in a draw call and it can change
  // every frame (attributes cant do that cheaply) glGetUniformLocation asks the
  // linked program where "ourColor" lives, it returns an int id the name must
  // match the glsl EXACTLY, a typo gives -1 and glUniform* with -1 silently
  // does nothing also glsl throws away uniforms it never uses so an unused one
  // gives -1 too the location never changes after linking so i ask ONCE here
  // instead of every frame like a string lookup in the loop
  GLint colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
  if (colorLocation == -1)
    std::cout << "Uniform ourColor not found (typo or unused in the shader)"
              << std::endl;

  // ---------- vertex data ----------

  // 3 vertices with x y z each, in normalized device coordinates (-1 to 1 on
  // every axis) z is 0 because the triangle is flat, counter clockwise order =
  // front face right now this is just 9 floats in cpu ram, opengl doesnt know
  // they are vertices yet
  std::vector<float> vertices = {
      -0.5f, -0.5f, 0.0f, // bottom left
      0.5f,  -0.5f, 0.0f, // bottom right
      0.0f,  0.5f,  0.0f  // top
  };

  // ---------- VAO and VBO ----------

  // VBO = the vertex bytes living on the gpu, VAO = the saved recipe of how to
  // read them
  GLuint VAO, VBO;
  // glGen* only reserves a name (an id), a VAO has its own function, not
  // glGenBuffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // bind the VAO FIRST so it records everything that comes after
  // core profile needs a VAO bound to configure attributes and to draw
  glBindVertexArray(VAO);

  // opengl is a state machine: bind the VBO to the array buffer socket
  // so the next calls act on it (glBufferData never gets told which buffer)
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // allocate gpu memory and copy the cpu floats into it, done once so no vertex
  // data crosses to the gpu every frame size is in BYTES (size() *
  // sizeof(float)), data() gives the pointer to the floats GL_STATIC_DRAW = set
  // once drawn many times
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  // now tell opengl what the bytes mean: attribute 0 = 3 floats per vertex
  // 0 = location in the vertex shader (layout (location = 0))
  // 3 = components (x y z), GL_FLOAT = their type, GL_FALSE = dont normalize
  // 3 * sizeof(float) = stride, bytes from one vertex to the next
  // (void *)0 = byte offset where the attribute starts in a vertex
  // this captures the VBO that is bound right now into the VAO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  // attributes are off by default, without this the shader gets nothing
  glEnableVertexAttribArray(0);

  // unbind the VBO, safe because the VAO already remembers it
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // unbind the VAO so later code cant accidentally change this setup
  glBindVertexArray(0);

  // ---------- the LOOP ----------

  // without a loop main ends and the window flashes for a second
  // the flag becomes true when i click the x or when processInput sets it
  while (!glfwWindowShouldClose(window)) {
    // input first
    processInput(window);

    // rendering
    // glClearColor only SETS the color, glClear is what actually fills the
    // screen with it (forgetting it = black window)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // which shaders to run
    // this MUST come before glUniform because glUniform sets the uniform of the
    // program that is active right now
    glUseProgram(shaderProgram);

    // seconds since glfwInit as a double, grows forever
    double timeValue = glfwGetTime();
    // sin goes from -1 to 1 but a color channel needs 0 to 1
    // so /2 shrinks it to -0.5..0.5 and +0.5 shifts it up to 0..1
    float greenValue = (float)(std::sin(timeValue) / 2.0 + 0.5);
    // 4f = four floats (r g b a), red and blue stay 0 so only the green pulses,
    // alpha 1 = opaque
    glUniform4f(colorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    // restores the whole vertex setup in one call, that is why the VAO exists
    glBindVertexArray(VAO);
    // the trigger: GL_TRIANGLES groups vertices in threes, start at vertex 0
    // the count is in VERTICES not floats so 9 floats / 3 = 3
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 3));

    // draws happen on the back buffer, swapping shows the finished frame (no
    // flickering from half drawn frames)
    glfwSwapBuffers(window);
    // handle os events (close button, keys, resize) or the window stops
    // responding
    glfwPollEvents();
  }

  // ---------- clean up ----------

  // free the gpu objects i made
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // its over
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
