cmake_minimum_required(VERSION 3.15)
project(StatusOverrider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BakkesModSDK")

include_directories("${SDK_PATH}/include")
include_directories("${SDK_PATH}/include/imgui")

link_directories("${SDK_PATH}/lib")

file(GLOB IMGUI_SOURCES "${SDK_PATH}/include/imgui/*.cpp")

add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
    OnlineStatusChangerSettings.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME} pluginsdk)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "StatusOverrider")
