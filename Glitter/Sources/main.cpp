// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
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

    // create and link shader program using our vert & frag shaders
    Shader defaultShader = Shader();
    defaultShader.init();

    defaultShader
        .registerShader("Shaders/shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/shader.frag", GL_FRAGMENT_SHADER)
        .link();

    defaultShader.use();

    // Initialize our application and call its init function
    Application app = Application();
    app.init();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VertexArrayObject;
    unsigned int VertexBufferObject;

    glGenVertexArrays(1, &VertexArrayObject);
    glBindVertexArray(VertexArrayObject);

    glGenBuffers(1, &VertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Tick the application state before the graphics update
        app.tick();

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultShader.use();

        glBindVertexArray(VertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);

        // Flip Buffers and Draw
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    
    // Teardown application and GLFW
    app.shutdown();

    defaultShader.cleanup();
    glfwTerminate();

    return EXIT_SUCCESS;
}
