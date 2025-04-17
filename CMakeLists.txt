cmake_minimum_required(VERSION 3.10)
project(papaya VERSION 1.0)

include(FetchContent)

FetchContent_Declare(glfw 
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(glfw)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# add_library(papaya_renderer STATIC src/main.c) 
add_executable(papaya main.c glad.c resources.geresource.c)
target_link_libraries(papaya glfw PkgConfig::GTK4)

find_package(OpenGL REQUIRED)
target_link_libraries(papaya OpenGL::GL)
