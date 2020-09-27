import os
import conans

class TestPackage(conans.ConanFile):

    generators = "cmake_find_package"
    settings = "os", "compiler", "build_type", "arch"

    requires = (
# ~end-doc
         "lp3-main/1.0.7@TimSimpson/testing",
# ~start-doc
         "lp3-sdl/1.0.6@TimSimpson/testing"
    )

    sdl2_requires = (
        "sdl2/2.0.9@bincrafters/stable",
        "sdl2_image/2.0.5@bincrafters/stable",
    )

    def requirements(self):
        if self.settings.os != "Emscripten":
            for r in self.sdl2_requires:
                self.requires.add(r)

    def build(self):
        cmake = conans.CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        pass
