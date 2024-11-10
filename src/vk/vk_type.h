#pragma once

#include "iostream"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            std::cout << "Detected Vulkan error: " << string_VkResult(err) << std::endl; \
            abort();                                                    \
        }                                                               \
    } while (0)