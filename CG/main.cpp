#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

#include "shader.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor,1.0f);\n"
"}\n\0";

const int num_subD = 10000;

std::vector<glm::vec3>controlPoint;
std::vector<glm::vec3>curPoint;

glm::vec3 threeB(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 temp;
    temp.x = 1.0f / 6.0f * ((1.0f * 1.0f + (-3.0f) * t + 3.0f * t * t - t * t * t) * p0.x +
        (4.0f - 6.0f * t * t + 3 * t * t * t) * p1.x +
        (1.0f + 3.0f * t + 3.0f * t * t - 3.0f * t * t * t) * p2.x +
        (t * t * t) * p3.x
        );
    temp.y = 1.0f / 6.0f * ((1.0f * 1.0f + (-3.0f) * t + 3.0f * t * t - t * t * t) * p0.y +
        (4.0f - 6.0f * t * t + 3 * t * t * t) * p1.y +
        (1.0f + 3.0f * t + 3.0f * t * t - 3.0f * t * t * t) * p2.y +
        (t * t * t) * p3.y
        );
    temp.z = 1.0f / 6.0f * ((1.0f * 1.0f + (-3.0f) * t + 3.0f * t * t - t * t * t) * p0.z +
        (4.0f - 6.0f * t * t + 3 * t * t * t) * p1.z +
        (1.0f + 3.0f * t + 3.0f * t * t - 3.0f * t * t * t) * p2.z +
        (t * t * t) * p3.z
        );

    return temp;
}

std::vector<glm::vec3> genThreeB(std::vector<glm::vec3>conPoint, int num_ponits) {
    std::vector<glm::vec3> curTemp;
    glm::vec3 temp;

    for (int i = 0; i < conPoint.size() - 3; ++i)
    {
        for (int j = 0; j <= num_ponits; ++j)
        {
            float t = static_cast<float> (j) / num_ponits;
            temp = threeB(t, conPoint[i], conPoint[i + 1], conPoint[i + 2], conPoint[i + 3]);
            curTemp.push_back(temp);
        }
    }
    return curTemp;
}


GLuint curVAO, curVBO;
GLuint conVAO, conVBO;

GLuint xVAO, xVBO;
GLuint yVAO, yVBO;
GLuint zVAo, VBO;
const int num_change = 1000;

int selectPoint = -1;
bool isDragging = false;

glm::vec3 transformToAxis(double xPos, double yPos, GLFWwindow* window);

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        glm::vec3 mousPos = transformToAxis(xPos, yPos, window);

        float threshold = 0.05f;
        for (int i = 0; i < controlPoint.size(); ++i) {
            if (glm::distance(mousPos, controlPoint[i]) < threshold) {
                selectPoint = i;
                isDragging = true;
                break;
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isDragging == false;
        selectPoint = -1;
    }
}

void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos) {
    if (isDragging && selectPoint != -1) {
        glm::vec3 mousePos = transformToAxis(xPos, yPos, window);
        controlPoint[selectPoint] = mousePos;

        curPoint = genThreeB(controlPoint, num_subD);

        glBindBuffer(GL_ARRAY_BUFFER, curVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, curPoint.size() * sizeof(glm::vec3), &curPoint[0]);

        glBindBuffer(GL_ARRAY_BUFFER, conVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, controlPoint.size() * sizeof(glm::vec3), &controlPoint[0]);
    }
}

void init()
{
    glGenVertexArrays(1, &curVAO);
    glGenBuffers(1, &curVBO);

    glBindVertexArray(curVAO);
    glBindBuffer(GL_ARRAY_BUFFER, curVBO);
    curPoint = genThreeB(controlPoint, num_subD);
    glBufferData(GL_ARRAY_BUFFER, curPoint.size() * sizeof(glm::vec3), &curPoint[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &conVAO);
    glGenBuffers(1, &conVBO);

    glBindVertexArray(conVAO);
    glBindBuffer(GL_ARRAY_BUFFER, conVBO);
    glBufferData(GL_ARRAY_BUFFER, controlPoint.size() * sizeof(glm::vec3), &controlPoint[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void controlPointInit() {

    controlPoint.push_back(glm::vec3(-0.9, 0.8, 0));
    controlPoint.push_back(glm::vec3(-0.8, 0.2, 0));
    controlPoint.push_back(glm::vec3(-0.5, -0.6, 0));
    controlPoint.push_back(glm::vec3(-0.3, 0.4, 0));
    controlPoint.push_back(glm::vec3(0.3, 0.2, 0));
    controlPoint.push_back(glm::vec3(0.5, -0.7, 0));
    controlPoint.push_back(glm::vec3(0.8, 0.8, 0));
}


int moveAction = 0; // 0:È¡ÏûäÖÈ¾£¬ 1£ºäÖÈ¾xÖá£¬ 2£ºäÖÈ¾yÖá

GLuint moveByAxisVAO, moveByAxisVBO;

void callbackSet(GLFWwindow*window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

}


int main()
{

    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    callbackSet(window);

   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    controlPointInit();
    init();

    
    Shader ourShader("shader\\4.2.texture.vs", "shader\\4.2.texture.fs");

    
    float vertices[] = {
                
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  
    };
    unsigned int indices[] = {
        0, 1, 3, 
        1, 2, 3 
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


   
    unsigned int texture1, texture2;
    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);     
    unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
   

    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    data = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    
    ourShader.use();   
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    
    ourShader.setInt("texture2", 1);



    
    while (!glfwWindowShouldClose(window))
    {
        
        processInput(window);

        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(curVAO);
        glDrawArrays(GL_LINE_STRIP, 0, curPoint.size());

        glBindVertexArray(conVAO);
        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, controlPoint.size());


        
        

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &curVAO);
    glDeleteBuffers(1, &curVBO);
    glDeleteVertexArrays(1, &conVAO);
    glDeleteVertexArrays(1, &conVBO);

    
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}

glm::vec3 transformToAxis(double xPos, double yPos, GLFWwindow* window) {
    float x, y;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    x = (2.0f * xPos) / width - 1.0f;
    y = 1.0f - (2.0f * yPos) / height;
    return glm::vec3(x, y, 0);
}