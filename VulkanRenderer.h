#pragma once
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

#include "Utilities.h"



class VulkanRenderer
{
    public:
        VulkanRenderer();
        static constexpr bool enableValidationLayers = true;
        int init(GLFWwindow* newWindow);
        void cleanup();

        ~VulkanRenderer();

    private:
        GLFWwindow* window;

        //vulkan components
        VkInstance instance;
        struct{
            VkPhysicalDevice physicalDevice;
            VkDevice logicalDevice;
        } mainDevice;
        VkQueue graphicsQueue;

        //Vulkan Functions
        void createInstance();
        void createLogicalDevice();
        // Get Functions
        void getPhysicalDevice();


        // Support Functions 
        bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
        bool checkDeviceSuitable(VkPhysicalDevice device);
        bool checkValidationLayerSupport();
        QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};
