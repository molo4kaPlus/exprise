cmake_minimum_required(VERSION 3.12.0)
project(exprise VERSION 0.1.0 LANGUAGES C CXX)


include(CTest)
enable_testing()

add_library("glad" "E:/code/exprise/lib/glad/src/glad.c")

include_directories("E:/code/exprise/lib/glad/include")
include_directories("E:/code/exprise/lib/sdl/source/include")

find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
# SDL library
find_library(SDL SDL2 PATHS "E:/code/exprise/lib/sdl/win32/lib64")
# that is needed on Windows for main function
find_library(SDLmain SDL2main PATHS "E:/code/exprise/lib/sdl/win32/lib64")
find_library(GLFW glfw3 PATHS "E:/code/exprise/lib/glfw/lib-vc2022")

include_directories( ${OpenCV_INCLUDE_DIRS} )

set(sources
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imconfig.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imstb_rectpack.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imstb_textedit.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imstb_truetype.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_impl_sdl2.h
)


add_executable(exprise ${sources})
target_link_libraries(exprise ${SDL} ${SDLmain})
target_link_libraries(exprise "glad")
target_link_libraries(exprise ${OPENGL_gl_LIBRARY})
target_link_libraries(exprise ${GLFW})
target_link_libraries(exprise ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
