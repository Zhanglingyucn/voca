cmake_minimum_required(VERSION 3.10)
project(voca-server-c)

# 设置C++标准为17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Find OpenMP package
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP ${OpenMP_CXX_VERSION}")
else()
    message(FATAL_ERROR "OpenMP not found!")
endif()

# 定义ASIO_STANDALONE宏
add_definitions(-DASIO_STANDALONE)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/crow
    ${CMAKE_SOURCE_DIR}/asio/include
    ${CMAKE_SOURCE_DIR}/om3-compress-online/include
)



# 添加可执行文件
add_executable(voca-server-c server.cpp 
        ${CMAKE_SOURCE_DIR}/om3-compress-online/MemoryManager.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/utils.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/AlpEncoder.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/AlpDecoder.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/CompressedData.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/src/falp.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/src/fastlanes_ffor.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/src/fastlanes_generated_ffor.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/src/fastlanes_generated_unffor.cpp
        ${CMAKE_SOURCE_DIR}/om3-compress-online/src/fastlanes_unffor.cpp
        )

# 链接库
target_link_libraries(voca-server-c PRIVATE pthread)


# Link OpenMP
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

# Set compiler flags for OpenMP
target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})


# Windows特殊配置
if(WIN32)
    target_link_libraries(voca-server-c PRIVATE ws2_32 mswsock)
endif()

