#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
using namespace std;

static const char* GetString(GLenum name)
{
    auto text = (const char*)glGetString(name);
    return text ? text : "(null)";
}

static int GetInt(GLenum name)
{
    GLint value;
    glGetIntegerv(name, &value);
    return value;
}

static void MyCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* msg,
    const void* data)
{
    if (msg && *msg) cout << "[OpenGL] " << msg << '\n';
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    auto window = SDL_CreateWindow(
        "SDL2 OpenGL Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    auto context = SDL_GL_CreateContext(window);
    glewInit();
    glEnable(GL_DEBUG_OUTPUT);
    
    cout
        << "OpenGL Vendor: " << GetString(GL_VENDOR)
        << "\nOpenGL Renderer: " << GetString(GL_RENDERER)
        << "\nOpenGL Version: " << GetString(GL_VERSION)
        << "\nOpenGL Shading Language: "
        << GetString(GL_SHADING_LANGUAGE_VERSION)
        << "\nOpenGL Max Texture Size: " << GetInt(GL_MAX_TEXTURE_SIZE)
        << '\n';
    
    cout << "OpenGL debug context flag ";
    GLint v;
    glGetIntegerv(GL_CONTEXT_FLAGS, &v);
    if (v & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        cout << "enabled\n";
        glDebugMessageCallback((GLDEBUGPROC)MyCallback, nullptr);
    }
    else
    {
        cout << "disabled\n";
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 1024, 768);
    glClearColor(0.0f, 0.25f, 0.25f, 1.0f);
    
    bool run = true;
    bool doDraw = true;
    while (run)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_EXPOSED:
                            doDraw = true;
                            break;
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            doDraw = true;
                            glViewport(
                                0,
                                0,
                                event.window.data1,
                                event.window.data2);
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) run = false;
                    break;
                case SDL_QUIT:
                    run = false;
                    break;
                default: break;
            }
        }
        
        if (doDraw)
        {
            doDraw = false;
            glClear(GL_COLOR_BUFFER_BIT);
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f(+0.5f, -0.5f);
            glVertex2f(0.0f, 0.5f);
            glEnd();
            SDL_GL_SwapWindow(window);
        }
        
        SDL_Delay(1);
    }
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    return 0;
}

