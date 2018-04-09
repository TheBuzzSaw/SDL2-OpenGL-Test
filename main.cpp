#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
using namespace std;

static constexpr auto PixelFormat = SDL_PIXELFORMAT_ABGR8888;
static const GLenum TexParams[] = {
    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE,
    GL_TEXTURE_MAG_FILTER, GL_NEAREST,
    GL_TEXTURE_MIN_FILTER, GL_NEAREST,
    0, 0 };

static void SetParams(const GLenum* params)
{
    for (int i = 0; params[i]; i += 2)
        glTexParameteri(GL_TEXTURE_2D, params[i], params[i + 1]);
}

static void LoadTexture(const char* path)
{
    auto surface = IMG_Load(path);

    if (surface)
    {
        if (surface->format->format != PixelFormat)
        {
            cout << "Converting format for " << path << '\n';
            auto convertedSurface = SDL_ConvertSurfaceFormat(
                surface,
                PixelFormat,
                0);

            SDL_FreeSurface(surface);
            surface = convertedSurface;
        }

        if (surface)
        {
            cout << "Loaded " << path << " successfully!\n";
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                surface->w,
                surface->h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                surface->pixels);
        }
        else
        {
            cout << "Failed to convert image format for " << path << '\n';
        }

        SDL_FreeSurface(surface);
    }
    else
    {
        cout << "Failed to load image " << path << '\n';
    }
}

static GLuint LoadShader(const char* source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        string errors;
        errors.resize(length);
        glGetShaderInfoLog(shader, length, &length, &errors[0]);

        cout << "-- shader compilation errors --\n" << errors << '\n';
    }
    else
    {
        cout << "successfully compiled shader\n";
    }

    return shader;
}

static GLuint LoadProgram(
    const char* vertexShaderSource,
    const char* fragmentShaderSource)
{
    GLuint program = glCreateProgram();

     GLuint vertexShader =
        LoadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader =
        LoadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        string errors;
        errors.resize(length);
        glGetProgramInfoLog(program, length, &length, &errors[0]);

        cout << "-- program linker errors --\n" << errors << '\n';
    }
    else
    {
        cout << "successfully linked shader program\n";
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return program;
}

static string FileToString(const char* path)
{
    string result;

    if (path && *path)
    {
        ifstream stream(path, ifstream::binary);

        if (stream)
        {
            ostringstream oss;
            oss << stream.rdbuf();
            stream.close();
            result = oss.str();
        }
    }

    return result;
}

static GLuint LoadProgramFromFiles(
    const char* vertexShaderPath,
    const char* fragmentShaderPath)
{
    string vertexShaderSource = FileToString(vertexShaderPath);
    string fragmentShaderSource = FileToString(fragmentShaderPath);

    return LoadProgram(
        vertexShaderSource.c_str(),
        fragmentShaderSource.c_str());
}

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
    
    auto program = LoadProgramFromFiles(
        "vertex.shader",
        "fragment.shader");
    
    auto matrixUniform = glGetUniformLocation(program, "theMatrix");
    auto textureUniform = glGetUniformLocation(program, "theTexture");
    auto positionAttribute = glGetAttribLocation(program, "position");
    auto colorAttribute = glGetAttribLocation(program, "color");
    auto textureCoordinateAttribute = glGetAttribLocation(
        program, "textureCoordinates");
    
    GLuint texture;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    SetParams(TexParams);
    LoadTexture("stick-man.png");
    
    glViewport(0, 0, 1024, 768);
    glClearColor(0.0f, 0.25f, 0.25f, 1.0f);
    glUseProgram(program);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableVertexAttribArray(positionAttribute);
    glEnableVertexAttribArray(colorAttribute);
    glEnableVertexAttribArray(textureCoordinateAttribute);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureUniform, 0);
    GLfloat matrix[16] = {};
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, matrix);
    
    GLfloat attribs[7 * 6] =
    {
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, +0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        +0.5f, +0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        +0.5f, +0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        +0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    
    constexpr auto Stride = sizeof(GLfloat) * 7;
    glVertexAttribPointer(
        positionAttribute,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        attribs);
    glVertexAttribPointer(
        textureCoordinateAttribute,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        attribs + 2);
    glVertexAttribPointer(
        colorAttribute,
        3,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        attribs + 4);
    
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
            glDrawArrays(GL_TRIANGLES, 0, 6);
            SDL_GL_SwapWindow(window);
        }
        
        SDL_Delay(1);
    }
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    return 0;
}

