#
SET(THE_PROJECT libcircular-buffer)

cmake_minimum_required(VERSION 3.26)

PROJECT(${THE_PROJECT} VERSION 1.0.0 DESCRIPTION "libcircular-buffer: a lib for circular buffer")

SET(LIBRARY_NAME circular-buffer)

# Set the variable source_files to the list of names of your C++ source code
# Note the lack of commas or other delimiters
SET(SOURCE_FILES
   circularBuffer.cpp
)

#ADD_LIBRARY(${LIBRARY_NAME} STATIC ${SOURCE_FILES})
ADD_LIBRARY(${LIBRARY_NAME} ${SOURCE_FILES}) # shared lib

SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES SOVERSION 1)
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER circularBuffer.h)

TARGET_INCLUDE_DIRECTORIES(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

