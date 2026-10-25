
#include "glad/glad.h"  // OpenGL function declarations loaded by GLAD
#include <GLFW/glfw3.h> // Window creation, OpenGL context, and input/events

#include <fstream>  // Read shader files from disk
#include <iostream> // Print messages to the terminal
#include <sstream>  // Read a whole file into a string
#include <string>   // Store shader source code
#include <vector>   // Store vertex data dynamically

int main() {
  // 1. INITIALIZE GLFW
  // GLFW manages the window and creates the OpenGL context.
  glfwInit();

  // Request an OpenGL 3.3 context using the CORE profile.
  // Core means we use modern OpenGL instead of deprecated functionality.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 2. CREATE THE WINDOW
  // Arguments: width, height, title, monitor, shared context.
  GLFWwindow *window =
      glfwCreateWindow(800, 800, "LearnOpenGL", nullptr, nullptr);

  // Always check whether window creation succeeded.
  if (window == nullptr) {
    std::cout << "Could not create an OpenGL window\n";
    glfwTerminate();
    return 1; // Non-zero means the program failed.
  }

  // 3. ACTIVATE THE OPENGL CONTEXT
  // OpenGL commands operate on the context that is current.
  glfwMakeContextCurrent(window);

  // GLAD loads the actual OpenGL function pointers for this context.
  // Check the return value so we don't continue if loading fails.
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  // Set the region of the window that OpenGL will render into.
  // x, y, width, height.
  glViewport(0, 0, 800, 800);

  // 4. LOAD THE VERTEX SHADER SOURCE FROM A FILE
  // A shader is GLSL code that will execute on the GPU.
  // The file contains source code, not a compiled shader.
  std::ifstream FileStream("shaders/vertex.glsl");
  std::stringstream ss;
  std::string vertex_shader_source;

  // Copy the file's entire contents into the stringstream.
  ss << FileStream.rdbuf();
  vertex_shader_source = ss.str();

  // Print the source to help debug file loading.
  std::cout << vertex_shader_source;

  // 5. LOAD THE FRAGMENT SHADER SOURCE
  std::string fragment_shader_source;

  // Reuse the same file stream and stringstream.
  // close() closes the old file; clear() resets stream error flags.
  FileStream.close();
  FileStream.clear();

  // Open the fragment shader file.
  FileStream.open("shaders/frag.glsl");

  // Reset the stringstream's contents and its state.
  ss.str("");
  ss.clear();

  // Read the fragment shader source into a string.
  ss << FileStream.rdbuf();
  fragment_shader_source = ss.str();

  std::cout << fragment_shader_source;

  // IMPORTANT:
  // Check whether each file opened successfully in a robust version.
  // Relative paths are resolved from the program's working directory.

  // 6. DEFINE THE VERTEX DATA
  // OpenGL normally expects 3D positions as x, y, z coordinates.
  // Each vertex here contains exactly 3 floats.
  //
  // First triangle: vertices 0, 1, 2.
  // Second triangle: vertices 3, 4, 5.
  //
  // OpenGL draws each group of 3 vertices as one triangle.
  std::vector<GLfloat> vertices = {
      -0.5f, 0.0f,  0.0f, // Vertex 0: left
      0.5f,  0.0f,  0.0f, // Vertex 1: right
      0.0f,  1.0f,  0.0f, // Vertex 2: top

      -0.5f, -0.1f, 0.0f, // Vertex 3: left
      0.5f,  -0.1f, 0.0f, // Vertex 4: right
      0.0f,  -1.0f, 0.0f  // Vertex 5: bottom
  };

  // 7. CREATE THE SHADERS
  // glCreateShader creates an OpenGL shader object.
  // It does NOT compile the GLSL source yet.
  GLuint VERTEX_SHADER = glCreateShader(GL_VERTEX_SHADER);
  GLuint FRAGMENT_SHADER = glCreateShader(GL_FRAGMENT_SHADER);

  // OpenGL accepts C-style strings, so c_str() provides a
  // const char* view of each std::string's contents.
  const char *vsrc = vertex_shader_source.c_str();
  const char *fsrc = fragment_shader_source.c_str();

  // Give each shader its source code.
  // Arguments: shader, number of strings, source strings, lengths.
  // nullptr means the strings are null-terminated.
  glShaderSource(VERTEX_SHADER, 1, &vsrc, nullptr);
  glShaderSource(FRAGMENT_SHADER, 1, &fsrc, nullptr);

  // 8. COMPILE THE SHADERS
  // The vertex shader processes vertex data.
  // The fragment shader determines the output color of fragments.
  glCompileShader(VERTEX_SHADER);
  glCompileShader(FRAGMENT_SHADER);

  // A real project should check GL_COMPILE_STATUS and print each
  // shader's info log if compilation fails.

  // 9. CREATE AND LINK THE SHADER PROGRAM
  // A program combines the compiled shader stages into one pipeline.
  GLuint SHADER_PROGRAM = glCreateProgram();

  // Attach both shader objects to the program.
  glAttachShader(SHADER_PROGRAM, VERTEX_SHADER);
  glAttachShader(SHADER_PROGRAM, FRAGMENT_SHADER);

  // Linking connects the shader stages into an executable program.
  glLinkProgram(SHADER_PROGRAM);

  // A real project should check GL_LINK_STATUS and print the
  // program's info log if linking fails.

  // After a successful link, the program retains the linked shaders.
  // The separate shader objects can now be deleted.
  glDeleteShader(VERTEX_SHADER);
  glDeleteShader(FRAGMENT_SHADER);

  // 10. CREATE THE VAO AND VBO
  // VBO = Vertex Buffer Object: stores vertex data in GPU memory.
  // VAO = Vertex Array Object: remembers vertex attribute configuration.
  GLuint VBO, VAO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind the VAO before configuring its vertex attributes.
  glBindVertexArray(VAO);

  // Bind the VBO as the source of array data.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Upload the CPU-side vertex array to GPU memory.
  // size is measured in bytes, not in the number of floats.
  // GL_STATIC_DRAW means the data is expected to change infrequently.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
               vertices.data(), GL_STATIC_DRAW);

  // Describe vertex attribute 0:
  // - 3 components per vertex (x, y, z)
  // - each component is a float
  // - no normalization
  // - stride: 3 floats between consecutive vertices
  // - offset: start reading at byte 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // Enable attribute 0 so the vertex shader receives the positions.
  glEnableVertexAttribArray(0);

  // Unbind the VBO, then the VAO.
  // The VAO retains the VBO association configured above.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // 11. THE RENDER LOOP
  // Keep rendering until the window receives a close request.
  while (!glfwWindowShouldClose(window)) {
    // Process window events and input.
    glfwPollEvents();

    // Choose the background color: red, green, blue, alpha.
    // Values range from 0.0 to 1.0.
    glClearColor(0.5f, 0.12f, 0.0f, 0.65f);

    // Clear the previous frame's color buffer.
    glClear(GL_COLOR_BUFFER_BIT);

    // Select the shader program that OpenGL should use.
    glUseProgram(SHADER_PROGRAM);

    // Activate the VAO containing our vertex layout.
    glBindVertexArray(VAO);

    // Draw 6 vertices as 2 triangles.
    // GL_TRIANGLES treats every 3 vertices as a separate triangle.
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Present the rendered frame on the screen.
    glfwSwapBuffers(window);
  }

  // 12. CLEAN UP GPU AND WINDOW RESOURCES
  // Delete the objects we created when they are no longer needed.
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(SHADER_PROGRAM);

  // Destroy the window and terminate GLFW.
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0; // Successful program exit.
}
