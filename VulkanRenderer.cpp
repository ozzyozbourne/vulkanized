#include "VulkanRenderer.h"
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

int VulkanRenderer::init(GLFWwindow* newWindow)
{
    window = newWindow;
    try{
        createInstance();
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
    //Create information for a VkInstance(Vulkan instance)
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
}
