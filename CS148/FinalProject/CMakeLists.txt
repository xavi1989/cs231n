cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (PM)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(LIBS ${OpenCV_LIBS}

file(GLOB SOURCE
    "src/${CHAPTER}/${DEMO}/*.h"
    "src/${CHAPTER}/${DEMO}/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})
