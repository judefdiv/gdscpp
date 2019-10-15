# CMake source folder
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Add source files to target
target_sources(gdscpp PRIVATE
  gdsCpp.cpp
  gdsImport.cpp
  gdsForge.cpp
  main.cpp
  gdsParser.cpp
)
