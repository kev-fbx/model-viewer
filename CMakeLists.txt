cmake_minimum_required(VERSION 3.15)

# Set policies to avoid warnings
cmake_policy(SET CMP0111 NEW)

project(ModelViewer VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# For multi-config generators (Visual Studio)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
endforeach()

# Find OpenGL (system dependency)
find_package(OpenGL REQUIRED)

# Set dependency paths (relative to project root)
set(DEPS_ROOT "${CMAKE_SOURCE_DIR}/dependencies")

# === ASSIMP SETUP ===
set(ASSIMP_INCLUDE_DIR "${DEPS_ROOT}/assimp/include")
set(ASSIMP_LIBRARY_DEBUG "${DEPS_ROOT}/assimp/lib/Debug/assimp-vc143-mtd.lib")
set(ASSIMP_LIBRARY_RELEASE "${DEPS_ROOT}/assimp/lib/Release/assimp-vc143-mt.lib")
set(ASSIMP_DLL_DEBUG "${DEPS_ROOT}/assimp/bin/Debug/assimp-vc143-mtd.dll")
set(ASSIMP_DLL_RELEASE "${DEPS_ROOT}/assimp/bin/Release/assimp-vc143-mt.dll")

# Create Assimp imported target
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIR}"
    IMPORTED_LOCATION_DEBUG "${ASSIMP_DLL_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${ASSIMP_DLL_RELEASE}"
    IMPORTED_LOCATION_MINSIZEREL "${ASSIMP_DLL_RELEASE}"
    IMPORTED_LOCATION_RELWITHDEBINFO "${ASSIMP_DLL_DEBUG}"
    IMPORTED_IMPLIB_DEBUG "${ASSIMP_LIBRARY_DEBUG}"
    IMPORTED_IMPLIB_RELEASE "${ASSIMP_LIBRARY_RELEASE}"
    IMPORTED_IMPLIB_MINSIZEREL "${ASSIMP_LIBRARY_RELEASE}"
    IMPORTED_IMPLIB_RELWITHDEBINFO "${ASSIMP_LIBRARY_DEBUG}"
)

# === GLFW SETUP ===
set(GLFW_INCLUDE_DIR "${DEPS_ROOT}/glfw/include")
set(GLFW_LIBRARY_RELEASE "${DEPS_ROOT}/glfw/lib/Release/glfw3.lib")

# Create GLFW imported target
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
    IMPORTED_LOCATION "${GLFW_LIBRARY_RELEASE}"
)

# === GLAD SETUP ===
set(GLAD_INCLUDE_DIR "${DEPS_ROOT}/glad/include")
set(GLAD_SOURCE "${DEPS_ROOT}/glad/src/glad.c")

# Add glad as a static library
add_library(glad STATIC ${GLAD_SOURCE})
target_include_directories(glad PUBLIC ${GLAD_INCLUDE_DIR})

# === GLM SETUP ===
set(GLM_INCLUDE_DIR "${DEPS_ROOT}/glm")

# Create GLM interface target (header-only)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

# === STB SETUP ===
set(STB_INCLUDE_DIR "${DEPS_ROOT}/stb")

# Create STB interface target (header-only, included in source)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})

# === SOURCE FILES ===
set(SOURCES
    src/Application.cpp
    src/Model.cpp
    src/Mesh.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/VertexBufferLayout.cpp
    src/Renderer.cpp
    src/stb_image.cpp
)

# === CREATE EXECUTABLE ===
add_executable(${PROJECT_NAME} ${SOURCES})

# === INCLUDE DIRECTORIES ===
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# === LINK LIBRARIES ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    assimp
    glfw
    glad
    glm
    stb
)

# === PLATFORM-SPECIFIC LIBRARIES ===
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        opengl32
        user32
        gdi32
        shell32
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        pthread 
        dl
        X11
    )
endif()

# === COPY RESOURCES ===
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
    COMMENT "Copying resources to output directory"
)

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# === COPY ASSIMP DLLS ===
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<IF:$<CONFIG:Debug>,${ASSIMP_DLL_DEBUG},${ASSIMP_DLL_RELEASE}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Assimp DLL to output directory"
    )
endif()

# === COMPILER SETTINGS ===
if(MSVC)
    # Visual Studio specific settings
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3
        $<$<CONFIG:Debug>:/MDd /Od /Zi /RTC1>
        $<$<CONFIG:Release>:/MD /O2 /Ob2 /DNDEBUG>
    )
    
    # Set working directory for Visual Studio debugging
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
        
else()
    # GCC/Clang settings
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

# === VALIDATION CHECKS ===
# Verify all dependency files exist
set(REQUIRED_FILES
    "${ASSIMP_INCLUDE_DIR}/assimp/Importer.hpp"
    "${ASSIMP_LIBRARY_DEBUG}"
    "${ASSIMP_LIBRARY_RELEASE}" 
    "${ASSIMP_DLL_DEBUG}"
    "${ASSIMP_DLL_RELEASE}"
    "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h"
    "${GLFW_LIBRARY_DEBUG}"
    "${GLFW_LIBRARY_RELEASE}"
    "${GLAD_INCLUDE_DIR}/glad/glad.h"
    "${GLAD_SOURCE}"
    "${GLM_INCLUDE_DIR}/glm/glm.hpp"
    "${STB_INCLUDE_DIR}/stb_image.h"
)

foreach(FILE ${REQUIRED_FILES})
    if(NOT EXISTS "${FILE}")
        message(FATAL_ERROR "
MISSING DEPENDENCY FILE: ${FILE}

Expected directory structure:
   dependencies/
   ├── assimp/
   │   ├── include/assimp/
   │   ├── lib/Debug/ & lib/Release/
   │   └── bin/Debug/ & bin/Release/
   ├── glfw/
   │   ├── include/GLFW/
   │   └── lib/Debug/ & lib/Release/
   ├── glad/
   │   ├── include/glad/ & include/KHR/
   │   └── src/
   ├── glm/glm/
   └── stb/

Download dependencies from:
   - Assimp: https://github.com/assimp/assimp/releases
   - GLFW: https://github.com/glfw/glfw/releases  
   - GLAD: https://glad.dav1d.de/
   - GLM: https://github.com/g-truc/glm/releases
   - STB: https://github.com/nothings/stb
        ")
    endif()
endforeach()

# === CONFIGURATION SUMMARY ===
message(STATUS "")
message(STATUS "=== ModelViewer Configuration Summary ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Dependencies (embedded):")
message(STATUS "  Assimp: ${ASSIMP_INCLUDE_DIR}")
message(STATUS "  GLFW: ${GLFW_INCLUDE_DIR}")
message(STATUS "  GLAD: ${GLAD_INCLUDE_DIR}")
message(STATUS "  GLM: ${GLM_INCLUDE_DIR}")
message(STATUS "  STB: ${STB_INCLUDE_DIR}")
message(STATUS "  OpenGL: ${OPENGL_FOUND}")
message(STATUS "")
message(STATUS "Output directory: $<TARGET_FILE_DIR:${PROJECT_NAME}>")
message(STATUS "=========================================")
message(STATUS "")