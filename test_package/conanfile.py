import os
import conans

class TestPackage(conans.ConanFile):

    generators = "cmake_paths", "cmake_find_package"
    settings = "os", "compiler", "build_type", "arch"

    requires = (
         "Lp3-Main/1.0.1@TimSimpson/testing",
         "Lp3-Sdl/1.0.1@TimSimpson/testing"
    )

    def build(self):
        cmake = conans.CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        pass
