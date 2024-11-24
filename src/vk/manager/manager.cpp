#include "vk/manager/GPU_manager.h"
#include "Rec3DConfig.h"

//Validation layer stuff
#ifdef NDEBUG
    constexpr bool bUseValidationLayers = false;
#else
    constexpr bool bUseValidationLayers = true;
#endif
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#pragma region GPU_manager
/*** INIT ***/
GPU_manager::GPU_manager() {
    init_Vulkan();
    init_DebugMessenger();
    init_PhysicalDevices();
    init_Devices();
    std::cout << "finished init" << std::endl;
}
void GPU_manager::init_Vulkan() {
    if(bUseValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    //application info
    VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = p_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(p_VERSION_MAJOR, p_VERSION_MINOR, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    //get needed Instance extentions
    std::vector<const char*> extensions;
    if (bUseValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    //the vulkan instance creation info
    VkInstanceCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo = &appInfo;
    //define the extentions
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    //for validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (bUseValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    //create instance
    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_instance));
}
void GPU_manager::init_DebugMessenger() {
    if(!bUseValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debug_messenger));
}
void GPU_manager::init_PhysicalDevices() {
    //get all devices
    uint32_t devicesCount = 0;
    vkEnumeratePhysicalDevices(_instance, &devicesCount, nullptr);
    if (devicesCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> phyDevices{devicesCount};
    vkEnumeratePhysicalDevices(_instance, &devicesCount, phyDevices.data());

    //start analysing all the devices  
    for(int i = 0; i < devicesCount; i++) {
        //get device as reference
        auto& device = phyDevices[i];
        DeviceInfo dInfo;
        
        //get device settings
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        dInfo._name = deviceProperties.deviceName;
        dInfo._physicalDevice = device;
        dInfo.deviceType = deviceProperties.deviceType;

        //get memory settings
        vkGetPhysicalDeviceMemoryProperties(device, &dInfo.memoryProperties);
        dInfo.m_Heaps.resize(dInfo.memoryProperties.memoryHeapCount);
        for (int h = 0; h < dInfo.m_Heaps.size(); h++) { //place all heaps
            dInfo.m_Heaps[h].data = dInfo.memoryProperties.memoryHeaps[h];
        }
        //may result into multiple of the same entry, but they should be ordered by performance
        //https://stackoverflow.com/questions/48242445/why-does-vkgetphysicaldevicememoryproperties-return-multiple-identical-memory-ty
        for (int t = 0; t < dInfo.memoryProperties.memoryTypeCount; t++) {
            VkMemoryType& type = dInfo.memoryProperties.memoryTypes[t];
            dInfo.m_Heaps[type.heapIndex].m_type.emplace_back(type);
            dInfo.m_Heaps[type.heapIndex].m_type.back().heapIndex = t; //replace heapIndex by type index, allowing to use it in VkMemoryAllocateInfo
        }

    /*
        //analyse queues
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies{queueFamilyCount};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int count = 0;
        for(auto& queueFamily : queueFamilies) {
            int value = 0;
            value += (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)? 100 : 0;
            value += (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)? 50 : 0;
            value += (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)? 1 : 0;

            DeviceInfo::QFP info{VK_NULL_HANDLE, queueFamily, count};

            count++;
            if(value == 0) continue; //if the queue as other ability that we clearly do not care for, remove them

            dInfo.queuesFamilly.emplace(value, info);
        }
    */
        
        //analyse result
        int value = 0;
        value += (dInfo.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ? 100 : 0; //can be use as main
        value += (deviceFeatures.multiDrawIndirect) ? 100 : 0; //can be use as main
        //value += (int) dInfo.queuesFamilly.size();

        if(deviceFeatures.geometryShader) _devices.emplace_back(dInfo);
    }

    
}
void GPU_manager::init_Devices() {
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.multiDrawIndirect = VK_TRUE;
    
    for (auto& device : _devices) {
        
        //struct QFInfo { VkQueueFamilyProperties data; int index; };
        std::vector<DeviceInfo::QFInfo> queueFamilies;
        {
            //get all QUEUES
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device._physicalDevice, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> tempQueueFamily{queueFamilyCount};
            vkGetPhysicalDeviceQueueFamilyProperties(device._physicalDevice, &queueFamilyCount, tempQueueFamily.data());

            //remove ALL queues that are not graphical, compute or transfer
            int i = 0;
            for (VkQueueFamilyProperties QFP : tempQueueFamily) {
                if (QFP.queueFlags & VK_QUEUE_GRAPHICS_BIT || QFP.queueFlags & VK_QUEUE_COMPUTE_BIT || QFP.queueFlags & VK_QUEUE_TRANSFER_BIT)
                    queueFamilies.push_back({ QFP, VK_NULL_HANDLE , i });
                i++;
            }
        }
        
        //the queue sumbition for the device creation
        int count = 0;
        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo{ queueFamilies.size() };
        for (auto& QFP : queuesCreateInfo) {
            auto& recInfo = queueFamilies[count];
            QFP = VkDeviceQueueCreateInfo{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };

            QFP.queueFamilyIndex = recInfo.index;
            QFP.queueCount = recInfo.data.queueCount;

            float queuePriority = 1.0f;
            QFP.pQueuePriorities = &queuePriority;
            count++;
        }

        //set all info in DEVICE
        VkDeviceCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        createInfo.pQueueCreateInfos = queuesCreateInfo.data();
        createInfo.queueCreateInfoCount = queuesCreateInfo.size();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (bUseValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        VK_CHECK(vkCreateDevice(device._physicalDevice, &createInfo, nullptr, &device._device));

        //retrieve all QUEUES
        for (auto& QFP : queueFamilies) {
            int index = -1;
            if (QFP.data.queueFlags & VK_QUEUE_GRAPHICS_BIT) index = DeviceInfo::GRAPHICS_QUEUES_INDEX;
            else if (QFP.data.queueFlags & VK_QUEUE_COMPUTE_BIT) index = DeviceInfo::COMPUTE_QUEUES_INDEX;
            else if (QFP.data.queueFlags & VK_QUEUE_TRANSFER_BIT) index = DeviceInfo::TRANSFER_QUEUES_INDEX;

            
            for (int i = 0; i < QFP.data.queueCount; i++) {
                device.queues[index].emplace_back(QFP);
                vkGetDeviceQueue(device._device, QFP.index, i, &device.queues[index].back().queue);
            }
        }
    }
}

/*** CLEANUP ***/
GPU_manager::~GPU_manager() {

    for (auto device : _devices) {
        vkDestroyDevice(device._device, nullptr);
    }

    if (bUseValidationLayers) {
        DestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);
    }

    vkDestroyInstance(_instance, nullptr);
}

/*** GET DEVICES INFORMATIONS ***/
DeviceID GPU_manager::get_DeviceRanked(uint32_t rank) {
    return 0; // :(
}
VkMemoryAllocateInfo GPU_manager::get_MemoryAllocateInfo( DeviceID _deviceID, const GPUm_MemoryAllocationInfo& _memoryAllocationInfo ) {
    //get needed info
    auto& device = _devices[_deviceID];
    VkMemoryAllocateInfo info{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, .allocationSize = _memoryAllocationInfo.size, .memoryTypeIndex = UINT32_MAX };
    //find type if one valid
    for(const auto& head : device.m_Heaps) {
        if((head.data.flags & _memoryAllocationInfo.heapFlags) != _memoryAllocationInfo.heapFlags) continue;
        //try find type if valid heap
        for(int i = 0; i < head.m_type.size(); i++) { //I use a normal for loop so that I can implement a better memory management with only one vector for every type, just ordered. using this technique, I will be able to just set the start_index of the heap in this vector and then go throw the size of the head, just more optimized memory management (less vector for the same result)
            if(_memoryAllocationInfo.typeFilter & (1 << head.m_type[i].heapIndex) && (head.m_type[i].propertyFlags & _memoryAllocationInfo.propertiesFlags) == _memoryAllocationInfo.propertiesFlags) { //do not forget, the heapIndex has been redefine as the type index
                info.memoryTypeIndex = head.m_type[i].heapIndex;
                return info; //return quickly if valid type found
            }
        }
    }
    
    throw std::runtime_error("Failed to find a suitable memory type!");
}


/*** VALIDATION LAYER ***/
bool GPU_manager::checkValidationLayerSupport() {
    //get info
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    //look into info
    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}
void GPU_manager::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = GPU_manager::debugCallback;
    //createInfo.pUserData = nullptr;
}
VkResult GPU_manager::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void GPU_manager::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
VKAPI_ATTR VkBool32 VKAPI_CALL GPU_manager::debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
#pragma endregion GPU_manager