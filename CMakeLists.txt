cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
project(ink)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/includes)

# Add executable
add_executable(ink stb_image/stb_image.cpp Shader.h glad.c ink.cpp)

# Find and link GLFW
find_package(glfw3 REQUIRED)

# macOS-specific configurations
if (APPLE)
    # Find and link macOS frameworks
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    # Link frameworks and libraries
    target_link_libraries(ink 
        ${COCOA_LIBRARY} 
        ${OPENGL_LIBRARY} 
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        glfw
    )
else()
    # For non-macOS systems (Linux)
    find_package(OpenGL REQUIRED)
    target_link_libraries(ink OpenGL::GL glfw dl)
endif()