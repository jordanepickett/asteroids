# OpenGL Learning Project

This is a small learning project where I’m experimenting with **OpenGL**, **GLFW**, and **GLAD**.

## Features
- Basic OpenGL setup
- GLFW window creation and input handling
- GLAD for managing OpenGL function pointers
- Incremental learning experiments (triangles, shaders, etc.)

## Requirements
Make sure you have the following installed:
- CMake (>= 3.10)
- A C++17 (or later) compiler
- [GLFW](https://www.glfw.org/) (for window/context/input)
- [GLAD](https://glad.dav1d.de/) (for OpenGL function loading)
- OpenGL (your GPU/driver should support at least 3.3)

## Building
Clone the repo and build with CMake:

```bash
cd asteroids
mkdir build && cd build
cmake ..
cmake --build .
