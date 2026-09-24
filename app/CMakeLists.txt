set(RESOURCES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(RESOURCES_DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/app")

# Add a custom command to copy the resources folder
file(COPY resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

set(SHADERS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
set(SHADERS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# function(add_slang_shader_target TARGET)
#   cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
#   set(SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
#   set(ENTRY_POINTS -entry vertMain -entry fragMain)
#   add_custom_command(
#     OUTPUT ${SHADERS_DIR}
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
#     # COMMAND ${CMAKE_COMMAND} -E make_directory     ${SHADERS_BINARY_DIR}
#   )
#   add_custom_command(
#     OUTPUT ${SHADERS_DIR}/slang.spv
#     COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
#     WORKING_DIRECTORY ${SHADERS_DIR}
#     DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
#     COMMENT "Compiling Slang Shaders"
#     VERBATIM
#   )
#   add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
# endfunction()

function(add_slang_shader_target TARGET)
  cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
  # set(SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)

  set(ENTRY_POINTS -entry vertMain -entry fragMain)

  # add_custom_command(
  #   OUTPUT ${SHADERS_DIR}
  #   # COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
  #   COMMAND ${CMAKE_COMMAND} -E make_directory     ${SHADERS_BINARY_DIR}
  # )
  # add_custom_command(
  #   OUTPUT ${SHADERS_DIR}/slang.spv
  #   COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
  #   WORKING_DIRECTORY ${SHADERS_DIR}
  #   DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
  #   COMMENT "Compiling Slang Shaders"
  #   VERBATIM
  # )
  # add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)

  add_custom_command(
    OUTPUT ${SHADERS_BINARY_DIR}/slang.spv

    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${SHADERS_BINARY_DIR}

    COMMAND ${SLANGC_EXECUTABLE}
            ${SHADER_SOURCES}
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            ${ENTRY_POINTS}
            -o ${SHADERS_BINARY_DIR}/slang.spv

    DEPENDS ${SHADER_SOURCES}

    COMMENT "Compiling Slang Shaders"
    VERBATIM
    )

    add_custom_target(
        ${TARGET}
        DEPENDS ${SHADERS_BINARY_DIR}/slang.spv
    )

endfunction()

find_program(SLANGC_EXECUTABLE slangc REQUIRED
  PATHS
    $ENV{VULKAN_SDK}/bin
    /Users/divyanshxg/VulkanSDK/vulkan/macOS/bin
)
message(STATUS "slangc: ${SLANGC_EXECUTABLE}")







add_executable(Executable main.cpp)


add_slang_shader_target(Shaders SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.slang)
# add_custom_target(
#     ${TARGET}
#     DEPENDS ${SHADERS_BINARY_DIR}/slang.spv
# )

add_dependencies(Executable Shaders)

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
