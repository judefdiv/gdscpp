# CMakeLists for GdsCpp containing the gdscpp library
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(GdsCpp VERSION 1.0)

# Conanfile
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Library dependencies

# Add library
add_library(gdscpp)
add_library(gdscpp::gdscpp ALIAS gdscpp)
add_subdirectory(src)

# Minimum compiler requirements
target_compile_features(gdscpp PUBLIC cxx_std_17)

# Include directories
target_include_directories(gdscpp PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Link dependencies
# Testing
include(CTest)
enable_testing()
add_subdirectory(tests)

# Installation: Targets
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS gdscpp EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE gdscpp::
  DESTINATION share/${PROJECT_NAME}/
)

write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Installation: Headers
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/")