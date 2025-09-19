#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "Model.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

unsigned int WIDTH = 800;
unsigned int HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
float fov = 45.0f;

glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
float cameraDistance = 3.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

const float zoomSpeed = 0.5f;
const float rotationSpeed = 0.005f;

bool leftMousePressed = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void orbit_callback(GLFWwindow* window, double xPos, double yPos);
void zoom_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);
void DefaultCube(unsigned int& VAO, unsigned int& VBO, unsigned int& texture);

int main(int argc, char* argv[])
{
    std::string modelPath = "";
    if (argc > 1)
    {
        modelPath = argv[1];
        std::cout << "Loading model @: " << modelPath << std::endl;
    }
    else
    {
        std::cout << "Loading default cube..." << std::endl;
    }

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Model Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, orbit_callback);
    glfwSetScrollCallback(window, zoom_callback);


    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader ourShader;

    std::unique_ptr<Model> model = nullptr;
    unsigned int defaultVAO = 0, defaultVBO = 0, defaultTexture = 0;
    bool useModel = false;

    if (!modelPath.empty())
    {
        try
        {
            model = std::make_unique<Model>(modelPath);
            useModel = true;
            std::cout << "Model loaded successfully!" << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cout << "Failed to load model: " << e.what() << std::endl;
            std::cout << "Falling back to default cube." << std::endl;
        }
    }

    if (!useModel)
    {
        DefaultCube(defaultVAO, defaultVBO, defaultTexture);
    }

    ourShader.use();
    if (!useModel)
    {
        ourShader.setInt("texture1", 0);
    }

    {
        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            ourShader.use();

            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
            ourShader.setMat4("projection", projection);

            float horizontalDistance = cameraDistance * cos(cameraAngleY);
            float verticalDistance = cameraDistance * sin(cameraAngleY);

            cameraPos.x = cameraTarget.x + horizontalDistance * sin(cameraAngleX);
            cameraPos.y = cameraTarget.y + verticalDistance;
            cameraPos.z = cameraTarget.z + horizontalDistance * cos(cameraAngleX);

            glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

            ourShader.setMat4("view", view);

            if (useModel && model)
            {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                ourShader.setMat4("model", modelMatrix);
                model->Draw(ourShader);
            }
            else
            {
                GLCall(glActiveTexture(GL_TEXTURE0));
                GLCall(glBindTexture(GL_TEXTURE_2D, defaultTexture));
                GLCall(glBindVertexArray(defaultVAO));

                glm::vec3 cubePositions[] = {
                    glm::vec3(0.0f,  0.0f,  0.0f),
                    glm::vec3(2.0f,  5.0f, -15.0f),
                    glm::vec3(-1.5f, -2.2f, -2.5f),
                    glm::vec3(-3.8f, -2.0f, -12.3f),
                    glm::vec3(2.4f, -0.4f, -3.5f),
                    glm::vec3(-1.7f,  3.0f, -7.5f),
                    glm::vec3(1.3f, -2.0f, -2.5f),
                    glm::vec3(1.5f,  2.0f, -2.5f),
                    glm::vec3(1.5f,  0.2f, -1.5f),
                    glm::vec3(-1.3f,  1.0f, -1.5f)
                };

                for (unsigned int i = 0; i < 10; i++)
                {
                    glm::mat4 modelMatrix = glm::mat4(1.0f);
                    modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
                    float angle = 20.0f * (i + 1);
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle) * (float)glfwGetTime() / 2.0f, glm::vec3(1.0f, 0.3f, 0.5f));
                    ourShader.setMat4("model", modelMatrix);

                    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if (!useModel)
        {
            GLCall(glDeleteVertexArrays(1, &defaultVAO));
            GLCall(glDeleteBuffers(1, &defaultVBO));
            GLCall(glDeleteTextures(1, &defaultTexture));
        }
    } // Scope allows index buffer to be deleted before the GLFW context is cleared

	// Clear all GLFW resources
    glfwTerminate();
    return 0;
}

// GLFW: called whenever window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    WIDTH = width;
    HEIGHT = height;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        leftMousePressed = (action == GLFW_PRESS);
        firstMouse = true;
    }
}

void orbit_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (!leftMousePressed)
    {
        lastX = xPos;
        lastY = yPos;
        return;
    }

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
        return;
    }

    float xOffset = (float)(lastX - xPos);
    float yOffset = (float)(yPos - lastY);

    lastX = xPos;
    lastY = yPos;

    cameraAngleX += xOffset * rotationSpeed;
    cameraAngleY += yOffset * rotationSpeed;

    cameraAngleY = glm::clamp(cameraAngleY, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
}

void zoom_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    cameraDistance -= (float)yOffset * zoomSpeed;
    cameraDistance = glm::clamp(cameraDistance, 0.5f, 100.0f);
}

// Query relevant key inputs
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void DefaultCube(unsigned int& VAO, unsigned int& VBO, unsigned int& texture)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // Position attribute
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));

    // Texture coord attribute
    GLCall(glEnableVertexAttribArray(2));
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

    // Load default texture
    GLCall(glGenTextures(1, &texture));
    GLCall(glBindTexture(GL_TEXTURE_2D, texture));

    // Set texture parameters
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/tex/dirt.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        std::cout << "Failed to load default texture, using white texture" << std::endl;
        unsigned char whitePixel[] = { 255, 255, 255 };
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel));
    }
    stbi_image_free(data);

    GLCall(glBindVertexArray(0));
}