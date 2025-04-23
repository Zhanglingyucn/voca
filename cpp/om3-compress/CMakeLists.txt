cmake_minimum_required(VERSION 3.10)

# Set compilers before project() command
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(om3-compress)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type (Debug or Release).  Crucial for debugging!
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # Default to Debug if not specified
endif()

add_compile_options(-fPIC)

# Include directories (where your header files are)
include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB ALP_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Source files (list all your .cpp files)
set(SOURCE_FILES
    main.cpp
    # main.SmallArray.cpp
    MemoryManager.cpp
    # MemoryManager.nocompress.cpp
    utils.cpp
    ${ALP_CORE_SOURCES}
    AlpEncoder.cpp
    AlpDecoder.cpp
    CompressedData.cpp
    utils.cpp
    # Add other .cpp files here if you have them
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Set the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    # CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
    # CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
)