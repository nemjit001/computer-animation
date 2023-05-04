// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Application.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

int main(int argc, char * argv[])
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, mAppName, nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // initialize shaders
    Shader vertShader = Shader("./Glitter/shader.vert", GL_VERTEX_SHADER);
    Shader fragShader = Shader("./Glitter/shader.frag", GL_FRAGMENT_SHADER);

    vertShader.init();
    fragShader.init();

    // create and link shader program using our vert & frag shaders
    ShaderProgram shaderProgram = ShaderProgram();
    shaderProgram.init();
    shaderProgram.registerShader(vertShader);
    shaderProgram.registerShader(fragShader);
    shaderProgram.use();

    // Initialize our application and call its init function
    Application app = Application();
    app.init();

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Tick the application state before the graphics update
        app.tick();

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    
    // Teardown application and GLFW
    app.shutdown();

    shaderProgram.cleanup();
    vertShader.cleanup();
    fragShader.cleanup();
    glfwTerminate();

    return EXIT_SUCCESS;
}
