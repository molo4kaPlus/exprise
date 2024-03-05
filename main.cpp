// don't listen to MS complains, we want cross-platform code
#define _CRT_SECURE_NO_DEPRECATE

// C++
//#include <filesystem>
#include <vector>
// SDL
#include <glad/glad.h>
#include <SDL.h>
// Dear ImGui
#include "lib/imgui/imgui-style.h"
#include "lib/imgui/imgui_impl_sdl2.h"
#include "lib/imgui/imgui_impl_opengl3.h"

// OpenCV
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <winuser.h>
#include <ctime>
#include <thread>

#include "functions.h"

#pragma warning(disable : 4996)

using namespace std;

/*
error listing: 
    1: sdl err
*/

class window_handler
{
    private:
        //sdl variables
        SDL_Window *window;
        SDL_Surface *surface;
        SDL_WindowFlags window_flags;
        SDL_GLContext gl_context;
        SDL_Event event;
        int window_height = 480;
        int window_width = 640;
        bool keep_window_open = true;
        const char* window_name = "Exprise";
        const char* glsl_version = "#version 130";

        //imgui variables
        ImGuiIO io;
        bool loop;
        bool show_window = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool done = false;

        //window variables
        const char* version = "Exprise v: 0.1.0";
        ImGuiViewport* main_viewport;

    public:
        void init_window();
        void main_loop();
        void cleanup();
        void ShowMainWindow(bool* p_open);
};

void window_handler::init_window()
{
    //setting up SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        {
            printf("Error: %s\n", SDL_GetError());
        }

        window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        window = SDL_CreateWindow(
            window_name,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            window_flags
    );
    if (window == nullptr) { cout << "Could not create window\n"; Sleep(100); }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize glad" << std::endl;
    }
    else
    {
        std::cout << "[INFO] glad initialized\n";
    }

    glViewport(0, 0, window_width, window_height);

    //setting up IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui::StyleColorsDark();                                 // set dark color

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void window_handler::main_loop()
{
    while(!done){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    glViewport(0, 0, window_width, window_height);
                    break;
                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    loop = false;
                    break;
                }
                break;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // standard demo window
        if (show_window) { window_handler::ShowMainWindow(&show_window); }

        // rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}

void window_handler::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void window_handler::ShowMainWindow(bool* p_open)
{
    main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_Always);
    if (!ImGui::Begin(version, p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
}

void ShowDebugWindow()
{
    
}

int main(int argc, char *argv[])
{
    window_handler window_handler;
    window_handler.init_window();
    window_handler.main_loop();
    window_handler.cleanup();

    return 0;
}