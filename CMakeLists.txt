cmake_minimum_required(VERSION 3.13.0)
project(gdscpp)                                  # creates ${PROJECT_NAME}

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
  src/main.cpp
  src/gdsCpp.cpp
  src/gdsParser.cpp
  src/gdsImport.cpp
  src/gdsForge.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Ensures that the header files of the project is included
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)