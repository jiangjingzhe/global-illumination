# OpenGL代码讲解

本文档对提供的C++代码中与OpenGL相关的部分进行详细讲解，分析其功能和在程序中的作用。代码主要集中在`Display`类及其方法，用于实现实时渲染和图像显示，涉及OpenGL 3.3核心配置文件。

---

## 1. OpenGL相关成员变量

以下是`Display`类中与OpenGL直接相关的成员变量及其作用：

- **`quadVAO`, `quadVBO`（类型：`GLuint`）**
  - **作用**：`quadVAO`（顶点数组对象）和`quadVBO`（顶点缓冲对象）用于存储和渲染全屏四边形的顶点数据。全屏四边形覆盖整个屏幕，用于将纹理渲染到屏幕上。
  
- **`shaderProgram`（类型：`GLuint`）**
  - **作用**：着色器程序，包含顶点着色器和片段着色器。顶点着色器处理顶点位置和纹理坐标，片段着色器从纹理中采样颜色并输出到屏幕。

- **`pbo`（类型：`GLuint`）**
  - **作用**：像素缓冲对象（Pixel Buffer Object），用于高效地将CPU端的像素数据传输到GPU，支持异步数据传输以提升纹理更新性能。

- **`renderTexture`（类型：`GLuint`）**
  - **作用**：OpenGL纹理对象，存储渲染的图像数据。绑定到着色器中的`screenTexture` uniform变量，用于在全屏四边形上显示渲染结果。

---

## 2. OpenGL相关函数

### 2.1 构造函数 `Display::Display(int width, int height)`
- **功能**：
  - 初始化帧缓冲区`framebuffer`（`Vec`数组，存储像素颜色值）。
  - 调用`init_opengl()`初始化OpenGL环境。
  - 调用`compile_shaders()`编译着色器。
  - 调用`setup_quad()`设置全屏四边形顶点数据。
- **OpenGL相关部分**：通过调用相关函数初始化OpenGL资源。

### 2.2 析构函数 `Display::~Display()`
- **功能**：
  - 释放帧缓冲区内存。
  - 删除OpenGL资源（`quadVAO`、`quadVBO`、`shaderProgram`）。
  - 调用`glfwTerminate()`终止GLFW。
- **OpenGL相关部分**：清理OpenGL对象，确保资源正确释放。

### 2.3 `init_opengl()`
- **代码**：
  ```cpp
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(w, h, "GI", NULL, NULL);
  glfwMakeContextCurrent(window);
  gladLoadGL();

  glGenTextures(1, &renderTexture);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenBuffers(1, &pbo);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, w*h*3, NULL, GL_STREAM_DRAW);
  ```
- **功能**：
  - 初始化GLFW，设置OpenGL 3.3 Core Profile。
  - 创建GLFW窗口并设置当前上下文。
  - 使用GLAD加载OpenGL函数指针。
  - 创建并配置纹理`renderTexture`，使用RGB格式和线性过滤。
  - 创建并绑定PBO`pbo`，分配缓冲区用于纹理数据传输。

### 2.4 `compile_shaders()`
- **代码**：
  ```cpp
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  ```
- **功能**：
  - 创建并编译顶点着色器和片段着色器。
  - 创建着色器程序`shaderProgram`，链接两个着色器。
  - 删除不再需要的着色器对象。

### 2.5 `setup_quad()`
- **代码**：
  ```cpp
  float quadVertices[] = {
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f, 1.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f
  };
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
  ```
- **功能**：
  - 定义全屏四边形的顶点数据（位置和纹理坐标）。
  - 创建并绑定`quadVAO`和`quadVBO`。
  - 将顶点数据上传到VBO。
  - 配置顶点属性指针，指定位置和纹理坐标的布局。

### 2.6 `update_texture(int totalSamples)`
- **代码**：
  ```cpp
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  unsigned char* ptr = (unsigned char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
  for (int i = 0; i < w*h; ++i) {
      Vec color = framebuffer[i];
      if (totalSamples > 0) color = color / totalSamples;
      ptr[i*3]   = toInt(color.x);
      ptr[i*3+1] = toInt(color.y);
      ptr[i*3+2] = toInt(color.z);
  }
  glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  ```
- **功能**：
  - 绑定PBO并映射其内存，允许CPU写入数据。
  - 根据`totalSamples`归一化帧缓冲区中的颜色值。
  - 将颜色数据写入PBO。
  - 使用PBO数据更新纹理`renderTexture`。
  - 解除PBO绑定。

### 2.7 `render_frame()`
- **代码**：
  ```cpp
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(shaderProgram);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glfwSwapBuffers(window);
  ```
- **功能**：
  - 清除颜色缓冲区。
  - 使用着色器程序。
  - 绑定纹理`renderTexture`。
  - 绑定`quadVAO`并绘制全屏四边形（6个顶点组成两个三角形）。
  - 交换GLFW窗口缓冲区，显示渲染结果。

### 2.8 着色器源码
- **顶点着色器**：
  ```glsl
  #version 330 core
  layout (location = 0) in vec2 aPos;
  layout (location = 1) in vec2 aTexCoord;
  out vec2 TexCoord;
  void main() {
      gl_Position = vec4(aPos, 0.0, 1.0);
      TexCoord = aTexCoord;
  }
  ```
  - **功能**：接收顶点位置和纹理坐标，将位置输出到`gl_Position`，纹理坐标传递给片段着色器。

- **片段着色器**：
  ```glsl
  #version 330 core
  in vec2 TexCoord;
  out vec4 FragColor;
  uniform sampler2D screenTexture;
  void main() {
      FragColor = texture(screenTexture, TexCoord);
  }
  ```
  - **功能**：从`screenTexture`纹理中采样颜色，输出到`FragColor`。

---

## 3. OpenGL在程序中的作用

### 3.1 初始化和配置
- **GLFW窗口和上下文**：通过`glfwInit()`和`glfwCreateWindow()`创建窗口，`gladLoadGL()`加载OpenGL函数。
- **纹理和PBO**：初始化`renderTexture`和`pbo`，为渲染和数据传输准备资源。

### 3.2 着色器
- **顶点着色器**：处理全屏四边形的顶点位置和纹理坐标。
- **片段着色器**：从纹理中采样颜色，完成像素级渲染。
- **着色器程序**：将纹理渲染到全屏四边形上。

### 3.3 纹理更新
- 使用PBO将CPU端的`framebuffer`数据高效传输到GPU端的`renderTexture`。
- `update_texture()`函数每次渲染后更新纹理，确保显示最新的图像。

### 3.4 渲染
- 通过绘制全屏四边形（`glDrawArrays`），将`renderTexture`渲染到屏幕。
- `glfwSwapBuffers`交换缓冲区，显示最终结果。

---

## 4. 总结

OpenGL在代码中的核心作用是将CPU端计算的渲染结果（存储在`framebuffer`中）显示到屏幕上。具体流程如下：
1. 初始化OpenGL环境和资源（窗口、纹理、PBO等）。
2. 使用着色器和全屏四边形渲染纹理。
3. 通过PBO高效更新纹理数据。
4. 绘制并显示渲染结果。

OpenGL提供了一个高效的实时渲染管道，PBO的使用优化了数据传输性能，使得程序能够平滑显示动态渲染图像，例如基于路径追踪的全局光照效果。