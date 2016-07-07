cmake_minimum_required(VERSION 2.8.12)

PROJECT(teapot)
#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
#########################################################
# Include Files
#########################################################
set(allsourcefiles
    a-initialization.cpp
    b-reshape.cpp
    c-animation.cpp
    d-keyboard.cpp
    e-thecodesofar.cpp
    f-thecodesofarII.cpp
    g-thecodesofarIII.cpp
    h-thecodesofarIV.cpp
    i-thecodesofarV.cpp
    j-thecodesofarVI.cpp
    k-subwindowscode.cpp
    l-thecodesofarVII.cpp
)

foreach( testsourcefile ${allsourcefiles} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    add_executable( ${testname} ${testsourcefile} )
    # Make sure YourLib is linked to each app
    ########################################################
    # Linking & stuff
    #########################################################
    target_link_libraries( ${testname} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
endforeach( testsourcefile ${APP_SOURCES} )
