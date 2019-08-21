# To run the code:
#  - mkdir build && cd build
#  - cmake ..
#  - make

cmake_minimum_required(VERSION 2.6)
project(gdscpp)                                  # creates ${PROJECT_NAME}

set(SOURCES
  src/main.cpp
  src/gdscpp.cpp
  src/ParserGdsii.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Ensures that the header files of the project is included
target_include_directories(${PROJECT_NAME} PRIVATE 
  ${PROJECT_SOURCE_DIR}/include
)