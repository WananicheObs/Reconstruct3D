/*
    This file will include all needed struct by the GPU_manager.

*/
#pragma once
#include "vk/vk_type.h" //may need to replace that, since it come with a lot of useless stuff for this file, since we just need the vulkan header

//the info needed to get a vkMemoryAllocationInfo
typedef struct GPUm_MemoryAllocationInfo {
    VkDeviceSize size;                          //size of needed allocation
    uint32_t typeFilter;                        //some filter stuff I'm still not sure about
    VkMemoryPropertyFlags propertiesFlags;      //property about the memory type we need
    VkMemoryHeapFlags heapFlags;                //property about the memory heap we need
} GPUm_MemoryAllocationInfo;