
#include "./../include/window.hpp"
#include <cstring>
#include <iostream>

Window::Window(glm::ivec2 const& windowsize)
  : m_window(nullptr)
  , m_size(windowsize)
  , m_title("Test"){
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_RESIZABLE, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.3);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

  m_window = glfwCreateWindow(windowsize.x, windowsize.y, m_title.c_str(), nullptr, nullptr);

  if (m_window) {
    glfwSetWindowUserPointer(m_window, this);
    assert(m_window != nullptr);
    glfwMakeContextCurrent(m_window);

    glewExperimental = GL_TRUE;
    glewInit();
  }
}

bool Window::shouldClose() const {
  return 0 != glfwWindowShouldClose(m_window);
}

void Window::stop() {
  glfwSetWindowShouldClose(m_window, GL_TRUE);
}

void Window::pause(){
  glfwWaitEvents();
}

void Window::update() {
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

glm::ivec2 Window::windowSize() const {
  glm::ivec2 size(0);
  glfwGetFramebufferSize(m_window, &size.x, &size.y);
  return size;
}

float Window::getTime() const {
  return float(glfwGetTime());
}

void Window::resize(glm::ivec2 const& windowsize) {
    glfwSetWindowSize(m_window, windowsize.x, windowsize.y);
}

Window::~Window() {
  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
  }
  glfwTerminate();
}
