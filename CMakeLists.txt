cmake_minimum_required(VERSION 3.15)
project(asteroids C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Collect all source files from code/
file(GLOB_RECURSE SOURCES code/*.cpp)

add_executable(asteroids ${SOURCES})

# --- Dependencies ---

# GLAD (compiled library)
add_library(glad deps/glad/src/glad.c)
target_include_directories(glad PUBLIC deps/glad/include)

# GLFW (pulled in with add_subdirectory since it has its own CMakeLists.txt)
add_subdirectory(deps/glfw)

# --- Include paths for headers ---
target_include_directories(asteroids PRIVATE
    ${CMAKE_SOURCE_DIR}/code        # your own headers
    ${CMAKE_SOURCE_DIR}/deps/glfw/include
    ${CMAKE_SOURCE_DIR}/deps/glad/include
    ${CMAKE_SOURCE_DIR}/deps/stb
    ${CMAKE_SOURCE_DIR}/deps/
)

target_link_directories(asteroids PRIVATE deps/glm)
# --- Linking ---
target_link_libraries(asteroids PRIVATE
    glad
    glfw
    opengl32       # Windows OpenGL
)

