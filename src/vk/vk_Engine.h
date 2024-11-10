#pragma once

#include "vk/vk_type.h"

class vk_Engine {
    bool _isInitialized{ false };

    VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands

    VkQueue _generalQueue;
    uint32_t _generalQueueFamily;

public:
    //init
    void init();
    void init_vulkan();

    //miscellaneous
    void getVulkanInfo();

    //cleanup
    void cleanup();
    void destroy_vulkan();
};