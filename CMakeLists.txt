
# Add components and libraries of the SparkFun SDK to the build environment


cmake_minimum_required(VERSION 3.12)
if (NOT TARGET _sparkfun_sdk_inclusion_marker)
    
    add_library(_sparkfun_sdk_inclusion_marker INTERFACE)
    
    project(sparkfun_sdk C CXX ASM)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 11)

    set(SPARKFUN_SDK 1 PARENT_SCOPE)

    add_compile_options(-Winline)

    # add the libraries in the source directory
    add_subdirectory(src)

endif()
