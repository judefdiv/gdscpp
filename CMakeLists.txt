# To run the code:
#  - mkdir build && cd build
#  - cmake ..
#  - make

cmake_minimum_required(VERSION 2.6)
project(gdscpp)                                  # creates ${PROJECT_NAME}

set(SOURCES
  src/main.cpp
  src/gdsCpp.cpp
  src/gdsClass.cpp
  src/gdsParser.cpp
  src/gdsImport.cpp
  src/gdsForge.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Ensures that the header files of the project is included
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)