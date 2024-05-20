#
SET(THE_PROJECT circular-buffer-example)

CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

PROJECT(${THE_PROJECT} VERSION 1.0.0 DESCRIPTION "example for circular buffer")

# Print logs; comment out if logs not wanted
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDO_LOGS")

# Set the variable source_files to the list of names of your C++ source code
# Note the lack of commas or other deliminators
SET(SOURCE_FILES
   circular-buffer-example.cpp
)

# Build a program called '${THE_PROJECT}' from the source files we specified above
ADD_EXECUTABLE(${THE_PROJECT} ${SOURCE_FILES})

SET(LINKED_LIBS circular-buffer)
TARGET_LINK_LIBRARIES(${THE_PROJECT} LINK_PUBLIC ${LINKED_LIBS})

