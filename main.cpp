#include <cstdlib>
#define GLFW_INCLUDE_VULAN

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

#include "VulkanRenderer.h"

GLFWwindow* window;

VulkanRenderer vulkanRenderer;

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 600 )
{

    //Initialise GLFW
    glfwInit();

    //Set GLFW to not work with Opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

int main() 
{
    //Creted Window
    initWindow();

    // Create Vulkan Renderer Instance
    if (vulkanRenderer.init(window) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Loop untill closed
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    //Destroy GLFW window and stop GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
