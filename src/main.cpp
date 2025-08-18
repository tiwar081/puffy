#include "viewer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool makePuffy = false;  // <- GLOBAL toggle flag
bool isDragging = false;
double lastX, lastY;
Viewer viewer;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
      if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        makePuffy = true;
        std::cout << "Puffy mode: " << (makePuffy ? "ON" : "OFF") << std::endl;
        viewer.enter3DMode();  // <-- this switches to 3D mode
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging && makePuffy) {
        float dx = xpos - lastX;
        float dy = ypos - lastY;
        viewer.updateCamera(dx, dy, 0.0f);
    }
    lastX = xpos;
    lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        isDragging = (action == GLFW_PRESS);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    viewer.updateCamera(0, 0, yoffset);
}


int main(int argc, char** argv) {

    int puffinessLevel = 0;

    if (argc >= 4 && std::string(argv[2]) == "-v") {
        puffinessLevel = std::stoi(argv[3]);  // could be 0–10 in future
    }


    std::string svgPath = argv[1];
    bool addCenterVertex = false;

    if (argc >= 4 && std::string(argv[2]) == "-v") {
        addCenterVertex = (std::string(argv[3]) == "1");
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "SVG Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set up coordinate system
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
glOrtho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!viewer.loadSVG(svgPath, puffinessLevel)) {
        std::cerr << "Failed to load SVG." << std::endl;
        return -1;
    }
    // Register callbacks BEFORE your main loop starts
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)) {
        if (makePuffy) {
            viewer.enter3DMode();  // Ensure we're in 3D mode every frame
        }
        glClear(GL_COLOR_BUFFER_BIT);
        viewer.draw(makePuffy);  // assuming draw(bool isPuffy)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
