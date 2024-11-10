#include "vk/vk_Engine.h"
#include "Rec3Dconfig.h"

#include "vk/vk_type.h"
//temporary
#include "iostream"

constexpr bool bUseValidationLayers = true;

#pragma region Init
void vk_Engine::init() {
    init_vulkan();

    _isInitialized = true;
}
void vk_Engine::init_vulkan() {
    
}

#pragma region Miscellaneous
void vk_Engine::getVulkanInfo() {
    VkPhysicalDeviceProperties2 GPUprop{};
	vkGetPhysicalDeviceProperties2(_chosenGPU, &GPUprop);
	std::cout << GPUprop.properties.deviceName << std::endl;
}

#pragma region Cleanup
void vk_Engine::cleanup() {
    if(_isInitialized) {
        //make sure the gpu has stopped doing its things
		vkDeviceWaitIdle(_device);

        destroy_vulkan();
    }
}
void vk_Engine::destroy_vulkan() {

}