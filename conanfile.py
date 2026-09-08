from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain

required_conan_version = ">=2.23"


class ConanApplication(ConanFile):
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "VirtualRunEnv"
    default_options = {
        "libarchive/*:shared": False,
        "libarchive/*:with_bzip2": True,
        "libarchive/*:with_lzma": True,
        "zlib/*:shared": False,
        "bzip2/*:shared": False,
        "xz_utils/*:shared": False,
        # Match the existing HTTP-only, header-only build.
        "cpp-httplib/*:with_openssl": False,
        "cpp-httplib/*:with_zlib": False,
        "cpp-httplib/*:with_brotli": False,
    }

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()

    def requirements(self):
        requirements = self.conan_data.get('requirements', [])
        for requirement in requirements:
            self.requires(requirement)
