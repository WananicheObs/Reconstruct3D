#include "vk/vk_Engine.h"
#include "Rec3Dconfig.h"

#include "vk/vk_type.h"
//temporary
#include "iostream"

//validation
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
    constexpr bool bUseValidationLayers = false;
#else
    constexpr bool bUseValidationLayers = true;
#endif

#pragma region Init
void vk_Engine::init() {
    init_vulkan();
    init_debugMessenger();

    _isInitialized = true;
}
bool checkValidationLayerSupport() {
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
std::vector<const char*> getRequiredExtensions() {
    std::vector<const char*> extensions;
    //extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    if (bUseValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vk_Engine::debugCallback;
}
void vk_Engine::init_vulkan() {
    if (bUseValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    //application info
    VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = p_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(p_VERSION_MAJOR, p_VERSION_MINOR, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    //tell vulkan what the are the hole project requirement, not just device specific
    VkInstanceCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo = &appInfo;
    //define the extentions
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    //for validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (bUseValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
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
//vulkan validation layers initialization function, explained here : https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/02_Validation_layers.html
//but there are other ways to do it, look at this for example: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap50.html#VK_EXT_debug_utils
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void vk_Engine::init_debugMessenger() {
    if(!bUseValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debug_messenger));
}

#pragma region Miscellaneous
void vk_Engine::getVulkanInfo() {
    //all vulkan instance supported extension
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    //device properties
    /*
    VkPhysicalDeviceProperties2 GPUprop{};
	vkGetPhysicalDeviceProperties2(_chosenGPU, &GPUprop);
	std::cout << GPUprop.properties.deviceName << std::endl;
    */
}
//vulkan validation layers analysis function, explained here : https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/02_Validation_layers.html
VKAPI_ATTR VkBool32 VKAPI_CALL vk_Engine::debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

#pragma region Cleanup
void vk_Engine::cleanup() {
    if(_isInitialized) {
        //make sure the gpu has stopped doing its things
		//vkDeviceWaitIdle(_device);

        destroy_vulkan();
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
void vk_Engine::destroy_vulkan() {
    if(bUseValidationLayers) DestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);

    vkDestroyInstance(_instance, nullptr);
}