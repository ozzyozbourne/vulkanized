#pragma once
#define GLFW_INCLUDE_VULAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
class VulkanRenderer
{
    public:
        VulkanRenderer();
        int init(GLFWwindow* newWindow);
        ~VulkanRenderer();

    private:
        GLFWwindow* window;

        //vulkan components
        VkInstance instance;

        //Vulkan Functions
        void createInstance();
};
