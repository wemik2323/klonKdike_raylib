from conan import ConanFile

class ConanTutorialRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("glm/0.9.9.8")
        self.requires("glfw/3.4")
        self.requires("stb/cci.20230920")
        self.requires("glad/0.1.36")
        self.requires("raylib/5.0")
