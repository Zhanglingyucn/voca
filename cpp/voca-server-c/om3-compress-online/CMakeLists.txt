cmake_minimum_required(VERSION 3.10)

# Set compilers before project() command
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(om3-compress)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP package
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP ${OpenMP_CXX_VERSION}")
else()
    message(FATAL_ERROR "OpenMP not found!")
endif()

# Build type (Debug or Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-fPIC)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB ALP_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Source files
set(SOURCE_FILES
    main.cpp
    MemoryManager.cpp
    utils.cpp
    ${ALP_CORE_SOURCES}
    AlpEncoder.cpp
    AlpDecoder.cpp
    CompressedData.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link OpenMP
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

# Set compiler flags for OpenMP
target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})

# Set the output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)

