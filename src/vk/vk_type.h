#pragma once

//std
#include <iostream>
#include <vector>
#include <cstring>
#include <optional>

//vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

//glm
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err != VK_SUCCESS) {                                        \
            std::cout << "[ERROR] " << string_VkResult(err) << std::endl; \
            abort();                                                    \
        }                                                               \
    } while (0)