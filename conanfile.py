import os
import os.path

import conans


class Lp3Sdl(conans.ConanFile):
    name = "lp3-sdl"
    version = "1.0.6"
    license = "Zlib"
    author = "Tim Simpson"
    url = "https://github.com/TimSimpson/lp3-sdl"
    description = "some C++ wrappers for SDL2, used by me"

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "use_basic_sdl2": [True, False],
    }
    default_options = {
        "shared": False,
        "use_basic_sdl2": False
    }

    requires = tuple()

    def get_sdl2_dependency(self):
        if not self.options.use_basic_sdl2:
            return "sdl2/2.0.9@bincrafters/stable"
        else:
            return "sdl2/b_2.0.9@TimSimpson/testing"

    def requirements(self):
        # don't require SDL2 on Emscripten since that platform bundles the
        # libraries.
        if self.settings.os != "Emscripten":
            self.requires.add(self.get_sdl2_dependency())

    build_requires = []

    test_requires = [
        "Catch2/2.11.1@catchorg/stable",
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

    generators = "cmake_find_package"

    exports_sources = (
        "src/*", "include/*", "demos/*", "tests/*", "CMakeLists.txt", "cmake/*",
    )

    def _configed_cmake(self):
        cmake = conans.CMake(self)
        cmake.configure(defs={
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
        _set_for_cmake(self.cpp_info.names, "lp3")
        _set_for_cmake(self.cpp_info.filenames, "lp3-sdl")
        _set_for_cmake(self.cpp_info.components['sdl'].names, "sdl")
        self.cpp_info.components['sdl'].libs = [ "lp3-sdl" ]
        self.cpp_info.components['sdl'].build_modules = [
            os.path.join(
                self.package_folder,
                "lib/cmake/lp3-sdl/ConanFindModuleExtra.cmake"
            )
        ]
        if self.settings.os != "Emscripten":
            self.cpp_info.components['sdl'].requires = [ 'sdl2::sdl2' ]


def _set_for_cmake(attr, value):
    for generator in ['cmake_find_package', 'cmake_find_package_multi']:
        attr[generator] = value
