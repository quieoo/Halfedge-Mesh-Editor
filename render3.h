#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.h"
#include "shader_m.h"
#include "camera.h"

#include <iostream>
#include "Loader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

unsigned int VBO[3];
unsigned int VAO[3];
unsigned int EBO[3];
int IndicedNumber[3]={0};

void InitialVAO(halfedgemesh &mesh, int index)
{
    float *vertices = new float[mesh.vertexs.size() * 3];
    long i = 0;
    float maxv = 0; //用于缩放面片
    for (list<vertex>::iterator iv = mesh.vertexs.begin(); iv != mesh.vertexs.end(); iv++)
    {
        maxv = max(maxv, abs(iv->pos_x));
        maxv = max(maxv, abs(iv->pos_y));
        maxv = max(maxv, abs(iv->pos_z));
        vertices[i++] = iv->pos_x;
        vertices[i++] = iv->pos_y;
        vertices[i++] = iv->pos_z;
    }

    //缩放面片，使其在-1～1之间
    for (int ii = 0; ii < i; ii++)
    {
        vertices[ii] /= maxv;
    }

    unsigned int *indices = new unsigned int[mesh.faces.size() * 3];
    for (list<face>::iterator it = mesh.faces.begin(); it != mesh.faces.end(); it++)
    {
        indices[IndicedNumber[index]++] = it->indices[0] - 1;
        indices[IndicedNumber[index]++] = it->indices[1] - 1;
        indices[IndicedNumber[index]++] = it->indices[2] - 1;
    }

    glGenVertexArrays(1, &VAO[index]);
    glGenBuffers(1, &VBO[index]);
    glGenBuffers(1, &EBO[index]);

    glBindVertexArray(VAO[index]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);
    glBufferData(GL_ARRAY_BUFFER, i * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[index]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicedNumber[index] * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

int render3(halfedgemesh &mesh1, halfedgemesh &mesh2, halfedgemesh &mesh3)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "task5", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("7.4.camera.vs", "7.4.camera.fs");

    InitialVAO(mesh1, 0);
    InitialVAO(mesh2, 1);
    InitialVAO(mesh3, 2);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        for (int i = 0; i < 3; i++)
        {
            glBindVertexArray(VAO[i]);

            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(i * (3.0f), 0.0f, 0.0f));
            //float angle = 20.0f * i;
            model = glm::rotate(model, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            ourShader.setMat4("model", model);
            //绘制线框
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            ourShader.setVec4("color", glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
            glDrawElements(GL_TRIANGLES, IndicedNumber[i], GL_UNSIGNED_INT, 0);

            //绘制三角面片
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            glDrawElements(GL_TRIANGLES, IndicedNumber[i], GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (int i = 0; i < 3; i++)
    {
        glDeleteVertexArrays(1, &VAO[i]);
        glDeleteBuffers(1, &VBO[i]);
        glDeleteBuffers(1, &EBO[i]);
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}