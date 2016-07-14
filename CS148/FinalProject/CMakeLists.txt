cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (CS148_final)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(LIBS ${OpenCV_LIBS})

file(GLOB SOURCE
    "*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})
