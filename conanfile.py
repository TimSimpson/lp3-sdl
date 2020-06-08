import os
import os.path

import conans


class Lp3Sdl(conans.ConanFile):
    name = "Lp3-Sdl"
    version = "1.0.2"
    license = "Zlib"
    author = "Tim Simpson"
    url = "https://github.com/TimSimpson/Lp3-Sdl"
    description = "some C++ wrappers for SDL2, used by me"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}

    requires = tuple()

    sdl2_requires = (
        "sdl2/2.0.9@bincrafters/stable",
    )

    def requirements(self):
        if self.settings.os != "Emscripten":
            for r in self.sdl2_requires:
                self.requires.add(r)

    build_requires = []

    test_requires = [
        "catch2/2.4.1@bincrafters/stable",
        "Lp3-Main/1.0.1@TimSimpson/testing",
    ]

    @property
    def tests_enabled(self):
        return (
            self.develop
            and (os.environ.get("CONAN_SKIP_TESTS") or "").lower() != 'true'
        )

    def build_requirements(self):
        if self.tests_enabled:
            for tr in self.test_requires:
                self.build_requires(tr)

    generators = "cmake_paths", "cmake_find_package"

    exports_sources = (
        "src/*", "include/*", "demos/*", "tests/*", "CMakeLists.txt"
    )

    def _configed_cmake(self):
        cmake = conans.CMake(self)
        cmake.configure(defs={
            "CMAKE_FIND_PACKAGE_PREFER_CONFIG": True,
            "LP3_SDL_Build_Tests": self.tests_enabled,
        })
        return cmake

    def build(self):
        cmake = self._configed_cmake()
        cmake.build()
        # If SDL2 is shared, we won't be able to find it in most cases.
        if self.settings.os != "Emscripten" and not self.options.shared:
            cmake.test()

    def package(self):
        cmake = self._configed_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["lp3_sdl"]
