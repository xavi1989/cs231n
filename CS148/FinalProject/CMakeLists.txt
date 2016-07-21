cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (CS148_final)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
find_package(GLEW REQUIRED)
message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
find_package(OpenGL REQUIRED)
message(STATUS "Found opengl in ${OPENGL_INCLUDE_DIR}")
find_package(X11 REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(LIBS ${OpenCV_LIBS} ${GLFW3_LIBRARY} GLEW glfw GL)

file(GLOB SOURCE
    "*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})

# copy shader files to build directory
file(GLOB SHADERS 
         "*.vs"
         "*.frag"
         "*.gs"
)
foreach(SHADER ${SHADERS})
  file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(SHADER)
