#
SET(THE_PROJECT circular-buffer-unit-tests)

CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

PROJECT(${THE_PROJECT} VERSION 1.0.0 DESCRIPTION "unit tests for circular buffer")

FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

FIND_PACKAGE(GMock REQUIRED)
INCLUDE_DIRECTORIES(${GMOCK_INCLUDE_DIRS})

SET(SOURCES_TO_BE_TESTED
    ../circularBuffer.cpp
)
SET(UNIT_TESTS_SOURCES
    unitTests.cpp
)

# Set the variable source_files to the list of names of your C++ source code
# Note the lack of commas or other deliminators
SET(SOURCE_FILES
    ${SOURCES_TO_BE_TESTED}
    ${UNIT_TESTS_SOURCES}
)

# Build a program called '${THE_PROJECT}' from the source files we specified above
ADD_EXECUTABLE(${THE_PROJECT} ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(${THE_PROJECT} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES} )

