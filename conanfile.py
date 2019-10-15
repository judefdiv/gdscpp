from conans import ConanFile, CMake
from conans.tools import Git


def get_version():
    """ Determine project version from git """
    git = Git()
    try:
        # Return the git commit hash
        return git.get_revision()
    except:
        # Return None if it's not a git repo
        return None


class GdsCppConan(ConanFile):
    """ Conan file for gdscpp::gdscpp """

    name = "GdsCpp"
    version = get_version()
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = ("include/*", "src/*", "tests/*", "CMakeLists.txt")
    generators = "cmake"
    no_copy_source = True

    def _cmake_configure(self):
        """ Shared CMake configure """
        cmake = CMake(self)

        cmake.definitions["BUILD_TESTS"] = "ON"
        cmake.definitions["BUILD_DOCS"] = "OFF"

        cmake.configure()

        return cmake

    def build(self):
        """ Build by building and testing with CMake """
        cmake = self._cmake_configure()
        cmake.build()
        cmake.test(["--", "CTEST_OUTPUT_ON_FAILURE=TRUE"])

    def package(self):
        """ Package by installing with CMake """
        cmake = self._cmake_configure()
        cmake.install()

    def build_requirements(self):
        """ Build dependencies """

    def requirements(self):
        """ Interface dependencies """
