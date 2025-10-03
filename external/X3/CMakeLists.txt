cmake_minimum_required(VERSION 3.16)

add_definitions(-w)

# Set main name
set(PROJECT_NAME X3)

# Options
option(BUILD_WITH_DEMO "Build with example" ON)

# Initialize
project(${PROJECT_NAME} LANGUAGES CXX CUDA)

# Set your architecture here
set(CUDA_ARCH 86)

# Set C++ and CUDA standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Detect operating system
if(WIN32)
    set(CURRENT_OS 0)
elseif(UNIX)
    set(CURRENT_OS 1)
endif()
add_definitions(-DCURRENT_OS=${CURRENT_OS})

# Sources and generate library
file(GLOB_RECURSE SOURCES_LIB RELATIVE "${PROJECT_SOURCE_DIR}" "src/*.*")
add_library(${PROJECT_NAME} ${SOURCES_LIB})
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_ARCHITECTURES "${CUDA_ARCH}")

# Headers
##############################################################################
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/src")

# Function to add all subdirectories recursively
function(add_subdirectories_recursively DIR)
    file(GLOB_RECURSE SUBDIRS LIST_DIRECTORIES true "${DIR}/*")

    foreach(SUBDIR ${SUBDIRS})
        if(IS_DIRECTORY ${SUBDIR})
            target_include_directories(${PROJECT_NAME} PUBLIC "${SUBDIR}")
        endif()
    endforeach()
endfunction()

# Call the function on the main directory
add_subdirectories_recursively("${PROJECT_SOURCE_DIR}/src")
##############################################################################

# External libraries
find_package(OpenMP) # Parallel CPU
find_package(nlohmann_json REQUIRED) # Save/Load
find_package(OpenGL REQUIRED) # Graphics
find_package(assimp REQUIRED) # Obj loading
find_package(imgui REQUIRED) # GUI
find_package(glfw3 REQUIRED) # GL wrapper
find_package(glad REQUIRED) # OpenGL loader
find_package(Stb REQUIRED) # Image loading
find_package(glm REQUIRED) # GL math
find_package(CUDA REQUIRED)

# Link all external libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw assimp::assimp OpenGL::GL nlohmann_json::nlohmann_json glad::glad glm::glm imgui::imgui OpenMP::OpenMP_CXX)
target_include_directories(${PROJECT_NAME} PUBLIC ${Stb_INCLUDE_DIR} "${CUDA_INCLUDE_DIRS}")

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Filters in visual studio
IF (MSVC)
	foreach(source IN LISTS SOURCES_LIB)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
ENDIF ()

# Example
if (BUILD_WITH_DEMO)
	file(GLOB_RECURSE SOURCES_DEMO RELATIVE "${PROJECT_SOURCE_DIR}" "demo/*.*")
	add_executable(Demo ${SOURCES_DEMO})
	target_include_directories(Demo PUBLIC "${PROJECT_SOURCE_DIR}/src")
	target_link_libraries(Demo PUBLIC ${PROJECT_NAME})
endif ()