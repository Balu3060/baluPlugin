cmake_minimum_required(VERSION 3.15)
project(StatusOverrider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define where the SDK is
set(SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BakkesModSDK")

# Include directories for headers
include_directories("${SDK_PATH}/include")
include_directories("${SDK_PATH}/include/imgui")

# Link directories - This points to the folder containing pluginsdk.lib
link_directories("${SDK_PATH}/lib")

# Find ImGui source files
file(GLOB IMGUI_SOURCES "${SDK_PATH}/include/imgui/*.cpp")

# Create the DLL
add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
    OnlineStatusChangerSettings.cpp
    ${IMGUI_SOURCES}
)

# Link the BakkesMod library
target_link_libraries(${PROJECT_NAME} pluginsdk)

# Ensure the output filename is StatusOverrider.dll
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "StatusOverrider")
