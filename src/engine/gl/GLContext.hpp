#ifndef GL_CONTEXT_HPP
#define GL_CONTEXT_HPP

#include <GLFW/glfw3.h>

class GLContext {
public:
    static bool init();
    static void enable_default_render_settings();
};

#endif