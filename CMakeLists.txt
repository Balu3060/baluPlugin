cmake_minimum_required(VERSION 3.15)
project(StatusOverrider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BakkesModSDK")

include_directories("${SDK_PATH}/include")
link_directories("${SDK_PATH}/lib")

add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
)

target_link_libraries(${PROJECT_NAME} pluginsdk)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "StatusOverrider")
