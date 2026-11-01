#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// --- 全局变量：控制相机 ---
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // 相机位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 相机朝向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 世界上方

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f;

// --- 鼠标控制回调 ---
float yaw = -90.0f; // 偏航角
float pitch = 0.0f;  // 俯仰角
float lastX = 400, lastY = 300; // 屏幕中心（假设窗口800x600）
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// --- 键盘输入处理 ---
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// --- 顶点着色器代码 ---
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

// --- 片段着色器代码 ---
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口 (800x600)
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 3D Scene", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        });

    // 初始化 GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 开启深度测试（3D必须开启）
    glEnable(GL_DEPTH_TEST);

    // 隐藏鼠标并捕获
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // --- 编译着色器 ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- 立方体的顶点数据 (位置 + 颜色) ---
    float vertices[] = {
        // 前面
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // 0
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // 1
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // 2
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // 3
        // 后面
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // 4
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // 5
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // 6
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f  // 7
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, // 前面
        4, 5, 6, 6, 7, 4, // 后面
        3, 2, 6, 6, 7, 3, // 上面
        0, 1, 5, 5, 4, 0, // 下面
        1, 2, 6, 6, 5, 1, // 右面
        0, 3, 7, 7, 4, 0  // 左面
    };

    // 配置 VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- 渲染循环 ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 1. 模型矩阵 (放在原点，随时间旋转)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        // 2. 观察矩阵 (相机)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 3. 投影矩阵 (透视投影)
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        // 传给着色器
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}