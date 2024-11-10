#include <iostream>
#include "vk/vk_Engine.h"

int main(int argc, char* argv[]) {
    vk_Engine engine{};
    engine.init();
    engine.getVulkanInfo();
    engine.cleanup();
}