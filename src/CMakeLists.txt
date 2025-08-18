option(BUILD_LIBCGL "Build with libCGL" OFF)

cmake_minimum_required(VERSION 3.10)
project(SvgInflate)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
include_directories(/opt/homebrew/include)


set(SVG_INFLATE_SOURCE
    main.cpp
    viewer.cpp
    svg_shape.cpp
)
set(SVG_INFLATE_HEADERS
    viewer.h
    svg_shape.h
)

include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

# Add executable
add_executable(svg_inflate
    ${SVG_INFLATE_SOURCE}
    ${SVG_INFLATE_HEADERS}
)

if(APPLE)
target_link_libraries(svg_inflate
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    glfw
)
else()
target_link_libraries(svg_inflate
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/dirent/glut32.lib
)
endif(APPLE)


target_link_libraries(svg_inflate
    OpenGL::GL
    glfw
    GLEW
    tinyxml2
)

if(APPLE)
  set_property(TARGET svg_inflate APPEND_STRING PROPERTY COMPILE_FLAGS
               "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Output to root build/
set(EXECUTABLE_OUTPUT_PATH ..)

# Install target (optional)
install(TARGETS svg_inflate DESTINATION ${ColladaViewer_SOURCE_DIR})
