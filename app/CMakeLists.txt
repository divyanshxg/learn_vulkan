set(RESOURCES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(RESOURCES_DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/app")

# Add a custom command to copy the resources folder
file(COPY resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(Executable main.cpp)

target_compile_definitions(
    Executable
    PRIVATE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)

# FIND PACKAGE SECTION
# --------------------
# Search for GLEW, which should be installed via Homebrew.
find_package(GLEW REQUIRED) 

# INCLUDE DIRECTORY SECTION
# -------------------------
# Ensure the compiler can find the GLEW headers (like GL/glew.h).
# This resolves the 'glew/glew.h' file not found error.
target_include_directories(
    Executable
    PRIVATE
    ${GLEW_INCLUDE_DIRS}
)

# LINKING SECTION
# ---------------
target_link_libraries(
    Executable 
    PRIVATE 
    glfw 
    glad
    assimp
    GLEW::GLEW            # Modern CMake target for GLEW, replacing GLEW_LIBRARIES variable
    # customClass 
    stb_image 
    glm
    "-framework OpenGL"   # MANDATORY for linking system OpenGL on macOS
)

# NOTE: GLAD and OpenGL::GL have been removed to avoid conflicts and redundancy.
