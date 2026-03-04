cmake_minimum_required(VERSION 3.15)
project(OnlineStatusChanger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point the compiler to the SDK and the ImGui folder
include_directories(BakkesModSDK/include)
include_directories(BakkesModSDK/include/imgui)
link_directories(BakkesModSDK/lib)

# Grab all the ImGui source files we downloaded
file(GLOB IMGUI_SOURCES "BakkesModSDK/include/imgui/*.cpp")

# Tell it to build the DLL from our files PLUS the ImGui files
add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
    OnlineStatusChangerSettings.cpp
    ${IMGUI_SOURCES}
)

# Link the BakkesMod plugin SDK
target_link_libraries(${PROJECT_NAME} pluginsdk)
