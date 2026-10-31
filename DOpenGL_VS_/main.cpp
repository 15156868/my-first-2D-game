#include <glad/gl.h>   
#include <GLFW/glfw3.h>
#include <iostream>
#include "math3d.h"    // 引用自己写的数学库

// 摄像机全局变量
Vec3 cameraPos(0.0f, 0.0f, 3.0f);
Vec3 cameraFront(0.0f, 0.0f, -1.0f);
Vec3 cameraUp(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float sensitivity = 0.1f;

// 立方体顶点数据
float vertices[] = {
    // 前面
    -0.5f,-0.5f, 0.5f,   0.5f,-0.5f, 0.5f,   0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,  -0.5f,-0.5f, 0.5f,
     // 后面
     -0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f, 0.5f,-0.5f,
      0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,
      // 左面
      -0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,
      -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,
      // 右面
       0.5f, 0.5f, 0.5f,   0.5f, 0.5f,-0.5f,   0.5f,-0.5f,-0.5f,
       0.5f,-0.5f,-0.5f,   0.5f,-0.5f, 0.5f,   0.5f, 0.5f, 0.5f,
       // 下面
       -0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,  -0.5f,-0.5f, 0.5f,  -0.5f,-0.5f,-0.5f,
        // 上面
        -0.5f, 0.5f,-0.5f,   0.5f, 0.5f,-0.5f,   0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,-0.5f
};

// 着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
uniform vec3 objectColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

// 编译着色器辅助函数
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    int ok; char log[512];
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(s, 512, NULL, log);
        std::cout << "Shader error: " << log << std::endl;
    }
    return s;
}

// 鼠标回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 屏幕Y轴向下，取反
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    // 限制俯仰角
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // 根据 yaw 和 pitch 重算朝向
    Vec3 front;
    front.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
    front.y = std::sin(radians(pitch));
    front.z = std::sin(radians(yaw)) * std::cos(radians(pitch));
    cameraFront = normalize(front);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 修正了这里的变量名
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, 1);

    GLFWwindow* window = glfwCreateWindow(800, 600, "My First OpenGL Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback);

    // 编译着色器
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // 初始化顶点数据
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // 计算帧间隔
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float speed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraFront * speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraFront * speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= normalize(cross(cameraFront, cameraUp)) * speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += normalize(cross(cameraFront, cameraUp)) * speed;

        // 渲染
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);

        Mat4 projection = perspective(radians(70.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 画地面
        Mat4 model = Mat4::identity();
        model = translate(model, Vec3(0.0f, -0.5f, 0.0f));
        model = scale(model, Vec3(10.0f, 0.1f, 10.0f));

        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, valuePtr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, valuePtr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, valuePtr(projection));
        glUniform3f(glGetUniformLocation(program, "objectColor"), 0.3f, 0.4f, 0.3f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 画障碍物立方体
        model = Mat4::identity();
        model = translate(model, Vec3(2.0f, 0.0f, -3.0f));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, valuePtr(model));
        glUniform3f(glGetUniformLocation(program, "objectColor"), 0.8f, 0.3f, 0.2f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}