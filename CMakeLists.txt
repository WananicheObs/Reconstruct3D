cmake_minimum_required(VERSION 3.20)
set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

include(Includes.cmake)

#for the compiler
set(Rec3D Reconstruct3D)
set(Rec3D_VERSION_MAJOR 0)
set(Rec3D_VERSION_MINOR 1)
project(${Rec3D} VERSION ${Rec3D_VERSION_MAJOR}.${Rec3D_VERSION_MINOR})

file(GLOB SOURCES
    src/*.cpp
    src/*.h
    src/vk/*.cpp
    src/vk/*.h
)
add_executable(${Rec3D} ${SOURCES})

target_include_directories(${Rec3D} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    ${Vulkan_DIRS}
)
target_link_libraries(${Rec3D} PRIVATE
    ${Vulkan_LIBS}
)

#added propreties
set_property(TARGET ${Rec3D} PROPERTY CXX_STANDARD 20)

#configure files with cmake data
configure_file(src/Rec3Dconfig.h.in Rec3Dconfig.h)