#include <iostream>
#include "vk/manager/GPU_manager.h"
GPU_manager vk_manager = GPU_manager();

#include "vk/vk_Engine.h"

int main(int argc, char* argv[]) {
    vk_Engine engine{};

    //vk_manager.~GPU_manager(); not needed since it is global, it automatically destroy itself
}

/*
idea:
    I need to find the border between multiple objects, an edge comparison algorithm is needed.
    I would love to use two cameras, one glued onto the other, and use some complex math to compare the results from both to get distance, object differences, etc.
    However, I am limited to one image, so I need to directly compare each pixel against the others. 
    I do not think that comparing the raw pixel data is effective, especially with a grayscale image.
    
    My solution would be to:
    1. Transform the image data by calculating the distances between color channels (red-blue, blue-green, green-red) and normalize these values. 
       This will minimize the effects of luminosity and lighting variations, creating a consistent representation that emphasizes the relationships between colors.
    2. Implement an edge detection algorithm, such as the Canny edge detector or Sobel operator, to identify significant transitions in the normalized data. 
       This will help accurately detect the edges between objects in the image based on the enhanced representation.
    3. Explore the possibility of developing a custom edge detection method that leverages the normalized distance metrics, focusing on how these metrics can uniquely highlight object boundaries.
    4. After edge detection, compress the image by aggregating edge information from groups of pixels, prioritizing areas with higher edge counts to focus on complex regions for further analysis.
    This approach should enhance the algorithm's ability to detect and differentiate objects within a single image effectively.

    I can also implement an edge detection algorithm that can generate multiple outputs with varying sensitivities. 
    This will allow for adaptive analysis of the image, enabling the detection of both fine details and significant edges.
    By adjusting sensitivity levels, I can highlight different features based on the specific requirements of the application.
    This approach will improve object detection accuracy and provide flexibility for post-processing techniques

interesting idea that pop out of my mind while talking with GPT:
    1. but doesn't his approach generate problem if a black and white check board is analyse, what do you think of that
    ...
    no, I mean like in number with is all 255 255 255 and black is 0 0 0, so if we do the distance red-blue, blue-green, green-red, the
    result will be 0,0,0 the same problem if we send it a Gray-scaled image
    2. or, I just use rgb of the resulting analysis for the normalized distance and "a" for the intensity
    (like how far it is from 0 or something like that i did not think of that yet)
    3. so if two pixel has similar normalized RGB distances, but different "a" values, we can conclude that the one with the stronger a resive
    more light, so the directions of the light is near, this will give a massive advantage I think while trying to negate the effects of white
    light (still problem will coloured light) and will reduced the impact of black-white board
    4. I just thought of something, since the proposed algorithm uses the normalized distance between color chanel, I can probably analyse a constant
    shift in color as if for example we take some white that turns red, between the tree chanel (red-blue rb, blue-green bg, green-red gr), RB will
    increase, BG will remain the same, and GR will decrease My only problem with this is that if my algorithm can do it, why not the other? react to
    the full message, not just the end

source:
    https://royalsocietypublishing.org/doi/10.1098/rsfs.2018.0008
    https://cocodataset.org/#explore
*/