import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.cache_variables["CMAKE_MSVC_DEBUG_INFORMATION_FORMAT"] = "Embedded"
        tc.generator = 'Ninja'
        tc.generate()

    def requirements(self):
        pass
        # self.requires('package_name/version', options={ 'shared': False })

    def layout(self):
        self.folders.generators = os.path.join("build", str(self.settings.build_type), "generators")
        self.folders.build = os.path.join("build", str(self.settings.build_type))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
