#include "vk/vk_Engine.h"
#include "vk/manager/GPU_manager.h"

#include "Rec3Dconfig.h"

//#include "vk/vk_type.h"
DeviceInfo* _deviceInfo = nullptr;
VkDevice _device = VK_NULL_HANDLE;

//image
const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 1024;

VkImage image = VK_NULL_HANDLE;
VkDeviceMemory imageMemory = VK_NULL_HANDLE;

/*
void testAllocateImage() {
    //image create info
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D; // or VK_IMAGE_TYPE_3D, etc.
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // Image format
    imageInfo.extent.width = WIDTH; // Width of the image
    imageInfo.extent.height = HEIGHT; // Height of the image
    imageInfo.extent.depth = 1; // Depth (for 2D images, this is usually 1)
    imageInfo.mipLevels = 1; // Number of mipmap levels
    imageInfo.arrayLayers = 1; // Number of layers (for 3D textures, this can be > 1)
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Number of samples per pixel
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Tiling (optimal or linear)
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Usage flags
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Sharing mode
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout

    VkImage image;
    VK_CHECK( vkCreateImage(_device, &imageInfo, nullptr, &image) );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device, image, &memRequirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory imageMemory;
    if (vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(_device, image, imageMemory, 0);
}
void testAllocateBuffer() {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    // Step 1: Create the buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 1024; // Set your buffer size
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // Example usage
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    // Step 2: Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

    // Step 3: Find a suitable memory type
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

    uint32_t memoryTypeIndex = 0;
    for (memoryTypeIndex = 0; memoryTypeIndex < memProperties.memoryTypeCount; memoryTypeIndex++) {
        if ((memRequirements.memoryTypeBits & (1 << memoryTypeIndex)) &&
            (memProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (memProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            break;
        }
    }

    // Step 4: Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    // Step 5: Bind memory to buffer
    vkBindBufferMemory(_device, buffer, bufferMemory, 0);
}
*/

void createImage() {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D; // or VK_IMAGE_TYPE_3D, etc.
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // Image format
    imageInfo.extent.width = WIDTH; // Width of the image
    imageInfo.extent.height = HEIGHT; // Height of the image
    imageInfo.extent.depth = 1; // Depth (for 2D images, this is usually 1)
    imageInfo.mipLevels = 1; // Number of mipmap levels
    imageInfo.arrayLayers = 1; // Number of layers (for 3D textures, this can be > 1)
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Number of samples per pixel
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Tiling (optimal or linear)
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Usage flags
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Sharing mode
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout

    VK_CHECK( vkCreateImage(_device, &imageInfo, nullptr, &image) );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device, image, &memRequirements);
    memRequirements.alignment = memRequirements.alignment;

    GPUm_MemoryAllocationInfo info{ .size = memRequirements.size, .typeFilter = memRequirements.memoryTypeBits };
    info.heapFlags  = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
    info.propertiesFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkMemoryAllocateInfo allocInfo = vk_manager.get_MemoryAllocateInfo(0, info);
    VK_CHECK( vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) );
    VK_CHECK( vkBindImageMemory(_device, image, imageMemory, 0) );
}

vk_Engine::vk_Engine() {
    _deviceInfo = &vk_manager._devices[/*vk_manager.get_DeviceRanked(0)*/0];
    _device = _deviceInfo->_device;
    
    createImage();
}
vk_Engine::~vk_Engine() {
    vkDestroyImage(_device, image, nullptr);
    vkFreeMemory(_device, imageMemory, nullptr);
}