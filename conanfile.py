import os
from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("spdlog/1.14.1")
        self.requires("lyra/1.6.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("wxwidgets/3.2.6")

    #def configure(self):
        #self.options["qt*"].shared = True