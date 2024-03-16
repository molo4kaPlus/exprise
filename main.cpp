// don't listen to MS complains, we want cross-platform code
#define _CRT_SECURE_NO_DEPRECATE

// C++
//#include <filesystem>
#include <vector>
// SDL
#include <glad/glad.h>
#include <SDL.h>
// Dear ImGui
#include "lib/imgui/imgui_impl_sdl2.h"
#include "lib/imgui/imgui_impl_opengl3.h"

// MAC
#include <winsock2.h>
#include <iphlpapi.h>
#include <sstream>

// OpenCV
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <winuser.h>
#include <ctime>
#include <thread>

#pragma comment(lib, "iphlpapi.lib")
#pragma warning(disable : 4996)

using namespace cv;
using namespace std;

/*
error listing: 
    1: sdl err
*/
 
static string GetMACaddress() {
    IP_ADAPTER_INFO AdapterInfo[16];       // информация о 16 адаптерах макс
    DWORD dwBufLen = sizeof (AdapterInfo); // кол-во байт в буффере

    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
    if (dwStatus != ERROR_SUCCESS) {
        printf("GetAdaptersInfo failed. err=%d\n", GetLastError());
        return "";
    }
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // информация о текущем адаптере

    stringstream ss; 
    for (int i = 0; i < 6; ++i) {
        ss << hex << int(pAdapterInfo->Address[i]);
    }
    cout << ss.str() << endl;
    return ss.str();
}

class window_handler
{
    private:
        //sdl variables
        SDL_Window *window;
        SDL_Surface *surface;
        SDL_WindowFlags window_flags;
        SDL_GLContext gl_context;
        SDL_Event event;
        float window_height = 480.0f;
        float window_width = 640.0f;
        bool keep_window_open = true;
        const char* window_name = "Exprise";
        const char* glsl_version = "#version 130";

        //imgui variables
        ImGuiIO io;
        bool loop;
        bool show_window = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImVec4 gray_color = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        bool done = false;

        //window variables
        const char* version = "Exprise v: 0.2.1";
        ImGuiViewport* main_viewport;
        ImGuiWindowFlags imgui_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
        
        //exprise variables
        bool dummy_bool = false;
        bool is_exprise_enabled = false;
        bool stop = false;
        bool cheat_active = false;
        bool access_enabled = false;
        float color_sensivity = 40;
        int time_left = 0; // оставшееся время
        int scan_radius = 10;
        int trigger_delay = 0;
        int trigger_delay_after = 0;
        int resolution_choice = 0; // пусть 0 - fHD
        int resolution_x;
        int resolution_y;
        Vec3b color = {63, 254, 254};    // искомый цвет

        //opencv variables
        HWND hwnd;
        HDC hwindowDC;
        HDC hwindowCompatibleDC;
        Mat pixel_matrix;
        HBITMAP hbwindow;
        BITMAPINFOHEADER bi;
        thread exprise_thr;

        // MAC variables
        const string MAC_adress = GetMACaddress();
        const tm end_time;
        thread validator;
        string string_time_left = "0";
        
    public:
        char user_key[128] ="";

        void init_window();
        void main_loop();
        void cleanup();
        void ShowMainWindow(bool* p_open);

        void exprise();
        void init_exprise();
        BITMAPINFOHEADER createBitmapHeader(int scan_radius);
        Mat captureScreenMat(HWND hwnd, int scan_radius, int x, int y);
        void get_pixels(HDC &hwindowCompatibleDC, HDC &hwindowDC, BITMAPINFOHEADER &bi, HBITMAP &hbwindow, Mat &src, int scan_radius, int x, int y);
        double diff_CIE76(Vec3b C1, Vec3b C2);
        void have_to_fire(Mat pixel_mat, int scan_radius, Vec3b color);
        void exprise_loop();
        void start_exprise_thread();

        void validation_procces();
        void validate_key(string key);
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

void window_handler::ShowMainWindow(bool* p_open = NULL)
{
    main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_Always);
    if (!ImGui::Begin(version, p_open, imgui_window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    ImGui::SeparatorText(version);
    ImGui::SeparatorText(MAC_adress.c_str());
    /*
    if (ImGui::BeginTable("split", 2))
    {
        ImGui::TableNextColumn(); 
        ImGui::Checkbox("Trigger bot", &cheat_active);
        ImGui::TableNextColumn(); 
        ImGui::EndTable();
    }
    */
    ImGui::InputTextWithHint("User key", "Input your key here.", user_key, IM_ARRAYSIZE(user_key));
    ImGui::SliderFloat("Color sensivity", &color_sensivity, 0.0, 200.0);
    ImGui::SliderInt("Scan radius", &scan_radius, 1, 100);
    ImGui::SliderInt("Trigger delay(ms)", &trigger_delay, 0, 400);
    ImGui::SliderInt("Trigger delay after(ms)", &trigger_delay_after, 0, 400);
    const char* resolution_array[] = { "1920x1080", "2560x1440", "3840x2160" };
    ImGui::Combo("Resolution", &resolution_choice, resolution_array, IM_ARRAYSIZE(resolution_array));
    if (is_exprise_enabled)
    {
        if (ImGui::BeginTable("split", 2)){
            if(ImGui::Button({"Validate key"})) { validate_key(user_key); }
            ImGui::TableNextColumn(); 
            ImGui::PushStyleColor(ImGuiCol_Button, gray_color);
            //if(ImGui::Button({"Initialize exprise"})) {  }
            ImGui::TableNextColumn(); 
            if (access_enabled)
                if(ImGui::Button({"Start exprise"})) {  }
            ImGui::PopStyleColor();
            if (access_enabled)
                if(ImGui::Button({"Stop         "})) { stop = true; is_exprise_enabled = false; }
            ImGui::EndTable();
        }
    }
    else
    {
        if (ImGui::BeginTable("split", 2)){
            ImGui::TableNextColumn(); 
            if(ImGui::Button({"Validate key"})) { validator = thread(&window_handler::validation_procces, this); validator.detach(); }
            ImGui::Text(string_time_left.c_str());
            ImGui::TableNextColumn();
            if (access_enabled) 
            {
                if(ImGui::Button({"Start exprise"})) { 
                    stop = false; 
                    is_exprise_enabled = true; 
                    init_exprise();
                    exprise_thr = thread(&window_handler::exprise_loop, this);
                    exprise_thr.detach();
                    }
            }
            ImGui::PushStyleColor(ImGuiCol_Button, gray_color);
            if (access_enabled)
                if(ImGui::Button({"Stop         "})) {  }
            ImGui::PopStyleColor();
            ImGui::EndTable();
        }
    }
}

BITMAPINFOHEADER window_handler::createBitmapHeader(int scan_radius)
{
    BITMAPINFOHEADER  bi;

    // create a bitmap
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = scan_radius;
    bi.biHeight = -scan_radius;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

Mat window_handler::captureScreenMat(HWND hwnd, int scan_radius, int x, int y)
{
    Mat src;

    // get handles to a device context (DC)
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // create mat object
    src.create(scan_radius, scan_radius, CV_8UC4);

    // create a bitmap
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, scan_radius, scan_radius);
    BITMAPINFOHEADER bi = createBitmapHeader(scan_radius);

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, scan_radius, scan_radius, hwindowDC, x/2 - scan_radius/2 , y/2 - scan_radius/2, scan_radius, scan_radius, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, scan_radius, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);                                                      //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

void window_handler::get_pixels(HDC &hwindowCompatibleDC, HDC &hwindowDC, BITMAPINFOHEADER &bi, HBITMAP &hbwindow, Mat &src, int scan_radius, int x, int y)
{
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, scan_radius, scan_radius, hwindowDC, x / 2 - scan_radius / 2, y / 2 - scan_radius / 2, scan_radius, scan_radius, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, scan_radius, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);                                                      //copy from hwindowCompatibleDC to hbwindow

}

double window_handler::diff_CIE76(Vec3b C1, Vec3b C2)
{
    int b = abs(C1[0] - C2[0]);
    int g = abs(C1[1] - C2[1]);
    int r = abs(C1[2] - C2[2]);
    double D = (sqrt(r*r + g*g) +sqrt(b*b + g*g) + sqrt(r*r + b*b));
    //cout << "Color difference: " << D << endl;
    return D;
    //sqrt((C1[0] - C2[0]) * (C1[0] - C2[0]) + (C1[1] - C2[1]) * (C1[1] - C2[1]) + (C1[2] - C2[2]) * (C1[2] - C2[2]));
}

void window_handler::have_to_fire(Mat pixel_mat, int scan_radius, Vec3b color)
{
    /*
    for (int i = 0; i < scan_radius; i++) {
        for (int j = 0; j < scan_radius; j++) {
            if (color[0] == pixel_mat.at<Vec3b>(i,j)[0] || color[1] == pixel_mat.at<Vec3b>(i,j)[1] || color[2] == pixel_mat.at<Vec3b>(i,j)[2]) {
                Sleep(trigger_delay); 
                keybd_event(0x4A, 0, 0, 0); keybd_event(0x4A, 0, KEYEVENTF_KEYUP, 0);
                return;                
            }
        }
    }
    */
    for (int i = 0; i < scan_radius; i++) {
        for (int j = 0; j < scan_radius; j++) {
            if (diff_CIE76(color, pixel_mat.at<Vec3b>(i, j)) <= color_sensivity) {
                this_thread::sleep_for(chrono::milliseconds(trigger_delay)); 
                keybd_event(0x4A, 0, 0, 0); keybd_event(0x4A, 0, KEYEVENTF_KEYUP, 0);
                this_thread::sleep_for(chrono::milliseconds(trigger_delay_after));
                return;
            }
        }
    }
}

void window_handler::init_exprise()
{
    hwnd = GetDesktopWindow();
    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
    pixel_matrix.create(scan_radius, scan_radius, CV_8UC4);
    hbwindow = CreateCompatibleBitmap(hwindowDC, scan_radius, scan_radius);
    bi = createBitmapHeader(scan_radius);
    SelectObject(hwindowCompatibleDC, hbwindow);
    switch (resolution_choice)
    {
        case 0:
            resolution_x = 1920;
            resolution_y = 1080;
            break;
        case 1:
            resolution_x = 2560;
            resolution_y = 1440;
            break;
        case 2:
            resolution_x = 3840;
            resolution_y = 2160;
            break;
        default:
            resolution_x = 1920;
            resolution_y = 1080;
            break;
    }
    cout << "[INFO] exprise initialized" << endl;
}

void window_handler::exprise_loop()
{
    cout << "[INFO] exprise started" << endl;
    while(!stop)
    {
        if (GetAsyncKeyState(VK_XBUTTON1) < 0)
        {
            get_pixels(hwindowCompatibleDC, hwindowDC, bi, hbwindow, pixel_matrix, scan_radius, resolution_x, resolution_y);
            //cout << "screen captured." << endl;
            have_to_fire(pixel_matrix, scan_radius, color);
        }        
    }
    cout << "[INFO] exprise stopped" << endl;
    return;
}

void window_handler::validate_key(string key)
{
    if (key == "") { time_left = 0; cout << "1" << endl; return; }

    string temp = key;
    string MAC;
    int j = 0;

    

    time_left = stoi(temp1) - time(NULL);    
}

void window_handler::validation_procces()
{
    while (true)
    {
        validate_key(user_key);
        if (time_left > 0) { access_enabled = true; }
        else { access_enabled = false; stop = true; is_exprise_enabled = false; return; }
        string_time_left = "time left: " + to_string(time_left / 3600) + "hrs." + to_string((time_left % 3600) / 60) + "min."; 
        cout << "time left: " << to_string(time_left) << endl;
        this_thread::sleep_for(chrono::seconds(15));
    }
}

int main(int argc, char *argv[])
{
    window_handler window_handler;
    window_handler.init_window();
    window_handler.main_loop();
    window_handler.cleanup();

    return 0;
}