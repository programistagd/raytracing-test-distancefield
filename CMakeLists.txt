cmake_minimum_required(VERSION 2.6)
project(raytrace_df)

# Set version information in a config.h file
set(raytrace_df_VERSION_MAJOR 0)
set(raytrace_df_VERSION_MINOR 1)
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

configure_file(FreeSans.ttf FreeSans.ttf COPYONLY)
configure_file(shaders.sh shaders.sh COPYONLY)
#configure_file(post.frag post.frag COPYONLY)
#configure_file(config.txt config.txt COPYONLY)
#file(COPY assets/ DESTINATION assets/)

#set(CMAKE_CXX_COMPILER "/home/radek/.vim/bin/cc_args.py clang++")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -pthread")

set(SOURCES
	main.cpp
   RayTracer.cpp
   loadShaders.cpp
   Volumes.cpp
	)

# Define sources and executable
set(EXECUTABLE_NAME "raytrace")
add_executable(${EXECUTABLE_NAME} ${SOURCES})

#Add GLEW
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.1 REQUIRED system window graphics)# audio #network

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})


# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)


# CPack packaging
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
#set(CPACK_PACKAGE_VERSION_MAJOR "${raytrace_df_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${raytrace_df_VERSION_MINOR}")
#include(CPack)
