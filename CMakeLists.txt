cmake_minimum_required(VERSION 3.21.0)
project(exprise VERSION 0.2.1 LANGUAGES C CXX)


include(CTest)
enable_testing()

add_library("glad" "lib/glad/src/glad.c")

include_directories("lib/glad/include")
include_directories("lib/sdl/source/include")

find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
# SDL library
find_library(SDL SDL2 PATHS "lib/sdl/win32/lib64")
# that is needed on Windows for main function
find_library(SDLmain SDL2main PATHS "lib/sdl/win32/lib64")
find_library(GLFW glfw3 PATHS "lib/glfw/lib-vc2022")

add_subdirectory("lib/cryptopp")

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
set(generator_source ${CMAKE_SOURCE_DIR}/key_generator.cpp)

add_executable(exprise ${sources})
add_executable(key_generator ${generator_source})
target_link_libraries(key_generator "cryptopp")
target_link_libraries(exprise ${SDL} ${SDLmain})
target_link_libraries(exprise "cryptopp")
target_link_libraries(exprise "glad")
target_link_libraries(exprise ${OPENGL_gl_LIBRARY})
target_link_libraries(exprise ${GLFW})
target_link_libraries(exprise ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
