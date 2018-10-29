#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <array>
#include <string>

class Window {

  public:
    Window(glm::ivec2 const& windowsize = glm::ivec2(640, 480));
    ~Window();

    bool shouldClose() const;
    void stop();
    void update();
    void pause();
    glm::ivec2 windowSize() const;
    float getTime() const;
    GLFWwindow* getGLFWwindow() { return m_window; }
    void resize(glm::ivec2 const& windowsize = glm::ivec2(640, 480));

  private:
    GLFWwindow* m_window;
    glm::ivec2 m_size;
    std::string const m_title;
};

#endif
