#pragma once

#include "vk/vk_type.h"
#include "vk/manager/GPU_manager_info.h"

//#include <map>

#define DeviceID const uint32_t


struct DeviceInfo {
    //core
    std::string _name;
    VkPhysicalDevice _physicalDevice;
    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDeviceType deviceType;

    //queues
    /* this hole section will need to be replace to a single QFInfo* with 3 index to where graphics, computes and transfers QFInfo start */
    static const int GRAPHICS_QUEUES_INDEX = 0;
    static const int COMPUTE_QUEUES_INDEX = 1;
    static const int TRANSFER_QUEUES_INDEX = 2;
    
    struct QFInfo { VkQueueFamilyProperties data; VkQueue queue; int index; };
    std::vector<QFInfo> queues[3];

    /*
    QFInfo* queues; // Pointer to a contiguous array of QFInfo
    int graphicsQueueStart; // Index where graphics queues start
    int computeQueueStart; // Index where compute queues start
    int transferQueueStart; // Index where transfer queues start
    int totalQueueCount; // Total number of queues
    */

    //memory
    VkPhysicalDeviceMemoryProperties memoryProperties;
    struct MH {
        VkMemoryHeap data;
        std::vector<VkMemoryType> m_type;

        //size
        float limit_hardware_GB;
        //float limit_logical;
    };
    std::vector<MH> m_Heaps;
};

class GPU_manager {
//variables
public:
    VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle

    std::vector<DeviceInfo> _devices;

//func
    /*** INIT && cleanup ***/
public:
    GPU_manager();
    ~GPU_manager();
private:
    void init_Vulkan();
    void init_DebugMessenger();
    void init_PhysicalDevices();
    void init_Devices();


    /*** GET DEVICES INFORMATIONS ***/
public:
    DeviceID get_DeviceRanked(uint32_t rank);
    VkMemoryAllocateInfo get_MemoryAllocateInfo( DeviceID _deviceID, const GPUm_MemoryAllocationInfo& _memoryAllocationInfo );

    /*** VALIDATION LAYER ***/
private:
    static bool checkValidationLayerSupport();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
public:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

};

//so that everything can access it's data
extern GPU_manager vk_manager;

//allocation strategie for the buffers: https://youtu.be/K-2bxdmosH8?si=j-5hSo7zEb1o0a9d&t=1254
//since creating the objects all individualy can be bad, create some allocation strategy and use it in the buffer