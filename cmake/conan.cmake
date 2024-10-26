#=========================================================================================================
# Create an autodetected Conan profile for this project and install the Conan-listed project dependencies.
#=========================================================================================================

# Create a new conan profile from detected properties named the project name
# detect      =>  auto detect settings and fill [settings] section
# --force     =>  overwrite existing profile with given name, if exists
# --name      =>  set profile name to the CMake project name
execute_process(
    COMMAND conan profile detect --force --name ${PROJ_NAME}
)

# Install missing Conan packages
# --settings               => apply an overwriting setting
#   compiler.cppstd        => set the C++ standard to C++ 20 with GNU extensions
# --build missing          => build packages from source whose binary package is not found
# --profile                => Use the CMake project name profile as the host and build profile
execute_process(
    COMMAND conan install ../conanfile.py --build=missing --profile:host=${PROJ_NAME} --profile:build=${PROJ_NAME} --output-folder=. --settings=build_type=${CMAKE_BUILD_TYPE} --settings:host=compiler.cppstd=20 --settings:build=compiler.cppstd=20
)


#=========================================================================================================
# Why is this done by CMake instead of being an explicit call in a project wrapper script?
# 1. Project wrapper scripts which would run Conan and CMake are platform specific (Batch, Shell) or
#    require another programming language/interpreter (Python)
# 2. Configuring this project should be as simple as `cmake ..` within the build directory