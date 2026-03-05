cmake_minimum_required(VERSION 3.15)
project(StatusOverrider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(BakkesModSDK/include)
include_directories(BakkesModSDK/include/imgui)
link_directories(BakkesModSDK/lib)

file(GLOB IMGUI_SOURCES "BakkesModSDK/include/imgui/*.cpp")

add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
    OnlineStatusChangerSettings.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME} pluginsdk)
