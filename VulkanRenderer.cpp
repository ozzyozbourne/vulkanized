#include "VulkanRenderer.h"
#include "Utilities.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};


int VulkanRenderer::init(GLFWwindow* newWindow)
{
    window = newWindow;
    try{
        createInstance();
        getPhysicalDevice();
        createLogicalDevice();
    }catch(const std::runtime_error &e){
        printf("ERROR %s\n", e.what());
        return EXIT_FAILURE;
    }
    return 0;
}

VulkanRenderer::VulkanRenderer()
{

}

VulkanRenderer::~VulkanRenderer()
{

}

void VulkanRenderer::createInstance()
{
    //Check for Validations layers 
    if(enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested but not available");
    }
    //Information about the application itself
    //Most data here does not affect the program and is for developer convenience
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engime";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    //Create information for a VkInstance(Vulkan instance)
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Create list to hold instance extensions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();

    //Set up extensions Instance will use 
    uint32_t glfwExtensionCount = 0;  // GLFW may require multiple extensions
    const char** glfwExtensions;     //Extensions passed as array of cstrings, so need pointer (the array ) to pointer (the cstring)

    //Get GLFW extensions to list of extensions
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Add GLFW extension to list of extensions
    for(size_t t = 0; t < glfwExtensionCount; t++)
    {
        instanceExtensions.push_back(glfwExtensions[t]);
    }
    
    //check Instabnnce Extensions supported
    if(!checkInstanceExtensionSupport(&instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions");
    }

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    //Create instance
    VkResult result =vkCreateInstance(&createInfo,nullptr, &instance);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Vulkan instance");
    }
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
    // Need to get number of extensions to create array of correct size to hold extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //Create a list of VkExtensionProperties using count
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    //Check if given extensions are in list of available extensions
    for(const auto &checkExtension: *checkExtensions)
    {
        bool hasExtension = false;
        for(const auto &extension: extensions)
        {
            if(strcmp(checkExtension, extension.extensionName))
            {
                hasExtension = true;
                break;
            }
        }

        if(!hasExtension)
        {
            return false;
        }
    }
    return true;
}

void VulkanRenderer::cleanup()
{
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::getPhysicalDevice()
{
    //Enumerate Physical device the vkInstance can access
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    //if no devices available, then none support vulkan 
    if(deviceCount == 0)
    {
        throw std::runtime_error("Can't find any GPUs that support Vulkan");
    }

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    for(const auto &device: deviceList)
    {
        if(checkDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
    //Information about the device itself (ID, name, type, vendor, etc)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    //Information about what the device can do (geometry shader, tesselation shader, wide lines, etc)
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices indices = getQueueFamilies(device);
    return indices.isValid();
}


QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device){

    QueueFamilyIndices indices;

    //Get all Queue Family Property info for the given device
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    //Go through each queue family and check if it has at least 1 of the required types of queue
    int i = 0;

    for(const auto &queueFamily: queueFamilyList)
    {
        //First check each queue family has at least 1 queue in that family (could have no queue)
        // Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if it has required type
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;    //If queue familty is valid, then get index
        }

        //check if queue family indces are in a valid state and stop searching if so 
        if(indices.isValid())
        {
            break;
        }
        i++;
    }
    return indices;
}


void VulkanRenderer::createLogicalDevice()
{

    //Get the queue family indices for the chosen Physical device
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    // Queue the logical device needs to create and info to do so (only 1 for now, will add more later)    
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;        // The index of the family to create a queue from
    queueCreateInfo.queueCount = 1;                                   // Number of queues to create
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;                     //Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)
                                                                      
    // Information to create a logical device (sometimes called "device")
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;                       // Number of enabled logical device extensions
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;

    //Physical Device Features the logical device will be using 
    VkPhysicalDeviceFeatures deviceFeatures = {};

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;             // Physical device features Logical device will use
    
    // Create the Logical device for the given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Logical device!");
    }

    //Queue are create at the same time as the device...
    //So we want handle to queues
    //From given logical device, of given queue family, of given queue index (0 since only one queue), place reference in given VkQueue 
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

bool VulkanRenderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers ) 
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) 
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) 
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) 
        {
            return false;
        }
    }

    return true;
}
