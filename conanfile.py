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
        self.requires("miniaudio/0.11.22")
        self.requires("neargye-semver/0.3.1")
        self.requires("zeromq/4.3.5", options={
            "poller": "epoll",
            "encryption": False
        })
        self.requires("cppzmq/4.11.0")
        self.requires("yaml-cpp/0.9.0")

    def layout(self):
            self.folders.generators = os.path.join("build", str(self.settings.build_type), "generators")
            self.folders.build = os.path.join("build", str(self.settings.build_type))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
