// Copyright <2017>[Tim Hu]
/*
使用uniform定义一个水平偏移量，在顶点着色器中使用这个偏移量把三角形移动到屏幕右侧

*/

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// #include <GL/glut.h>
// stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <learnopengl/shader_s.h>
#include <math.h>
#include <iostream>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;


// 实例化GLFW窗口
int main() {
  std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
  // Init GLFW
  glfwInit();
  // Sel all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL",
                                        nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // 设置当前的上下文
  glfwMakeContextCurrent(window);
  // 注册函数至适合的回调
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


  // 初始化GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  
  // 建立和编译着色程序
  // ===================
  Shader ourShader("shader_texture.vs", "shader_texture.fs");
  // 设置顶点数据和属性指针
  // =====================
  // 顶点输入
  GLfloat vertices[] = {
    // -------位置--------    -------颜色------   --纹理坐标--
    0.5f,   0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f,  1.0f,  // 右上角
    0.5f,  -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f,  0.0f,  // 左下角
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f,  1.0f   // 左上角
  };
  unsigned int indices[] = {
    0, 1, 3,    // 第一个三角形
    1, 2, 3     // 第二个三角形
  };
  // 生成一个VBO顶点对象，VBO顶点数组对象
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // 先绑定顶点数组对象，再绑定和设置顶点缓存和属性指针
  glBindVertexArray(VAO);

  // 把顶点数组复制到缓存中供OpenGL使用
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
               vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
               indices, GL_STATIC_DRAW);

  // 设置顶点位置属性指针
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // 设置颜色属性指针
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  // **(3 * sizeof(float) 的意义???**
  glEnableVertexAttribArray(1);
  // 设置纹理属性指针
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  // **(6 * sizeof(float) 的意义???**
  glEnableVertexAttribArray(2);

  // 加载和创建纹理
  // =============
  unsigned int texture;
  glGenTextures(1, &texture);                 // generate
  glBindTexture(GL_TEXTURE_2D, texture);      // bind
  // 为当前绑定设置环绕和过滤方式
  // 设置环绕参数
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // 设置过滤参数
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // 加载图片stbi_image库
  int width, height, nrChannels;
  unsigned char* data = stbi_load(
      "../../../resources/textures/container.jpg",
      &width, &height, &nrChannels, 0);
  // 生成纹理
  if (data) {
    // 图片导入成功
    glTexImage2D(GL_TEXTURE_2D,
               0, GL_RGB, width, height,
               0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // 为当前绑定的纹理自动生成所需要的多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    // 图片导入失败
    std::cout << "Failed to load texture" << std::endl;
  }
  // 释放图像内存
  stbi_image_free(data);

  // 渲染loop
  // ==============================================================
  while (!glfwWindowShouldClose(window)) {
    // input
    // ------
    processInput(window);

    // 渲染
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // 清空颜色缓冲
    glClear(GL_COLOR_BUFFER_BIT);

    // 绑定纹理, 会自动把纹理赋值给片段着色器的采样器
    glBindTexture(GL_TEXTURE_2D, texture);

    ourShader.use();
    glBindVertexArray(VAO);
    // 绘制纹理
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // 设置uniform smapler2D?
    // glUniform1f(glGetUniformLocation(ourShader.ID, "xOffset"),
    //            offset);          // 查询获取位置和更新Uniform

    // 绘制三角形
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0);
    // ghUniform

    // 交换screen buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  // 释放GLFW分配的内存
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  // 当用户按下esc键, 我们设置window窗口的windowShouldCloseni属性为True
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}


/*
void f() {
  unsigned int texture;
  glGenTextures(1, &texture);                 // generate
  glBindTexture(GL_TEXTURE_2D, texture);      // bind
  // 为当前绑定设置环绕和过滤方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // 加载图片stbi_image库
  int width, height, nrChannels;
  unsigned char* data = stbi_load(
      "../../../resources/textures/container.jpg",
      &width, &height, &nrChannels, 0);
  // 生成纹理
  if (data) {
    // 图片导入成功
    glTexImage2D(GL_TEXTURE_2D,
               0, GL_RGB, width, height,
               0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // 为当前绑定的纹理自动生成所需要的多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    // 图片导入失败
    std::cout << "Failed to load texture" << std::endl;
  }
  // 释放图像内存
  stbi_image_free(data);
}
  

void apply_texture {
  // 应用纹理
  float vertices[] = {
// -------位置--------    -------颜色------   --纹理坐标--
    0.5f,   0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f,  1.0f,  // 右上角
    0.5f,  -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, -1.0f,  // 右下角
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  -1.0f, -1.0f,  // 左下角
    -0.5f,  0.5f, 0.0f   1.0f, 1.0f, 0.0f,  -1.0f,  1.0f   // 左上角
  };

  // 新的顶点格式
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        8*sizeof(float), (void*)(6*(sizeof(float))));
  glEnableVertexAttribArray(2);

  // 绑定纹理, 会自动把纹理赋值给片段着色器的采样器
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(VAO);

  // 调用glDrawElements绘制
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
*/
// ..
