cmake_minimum_required(VERSION 3.15)
project(OnlineStatusChanger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point the compiler to the BakkesMod SDK we will download in the Action
include_directories(BakkesModSDK/include)
link_directories(BakkesModSDK/lib)

# Tell it to build a DLL from our C++ files
add_library(${PROJECT_NAME} SHARED 
    OnlineStatusChanger.cpp 
    OnlineStatusChangerSettings.cpp
)

# Link the BakkesMod plugin SDK
target_link_libraries(${PROJECT_NAME} pluginsdk)
