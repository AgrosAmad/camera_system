cmake_minimum_required(VERSION 3.16)

add_definitions(-w)

# Set main name
set(PROJECT_NAME CAM)

# Initialize
project(${PROJECT_NAME} LANGUAGES CXX CUDA)

# Set your architecture here
set(CUDA_ARCH 86)

# Set C++ and CUDA standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Include graphical library
add_subdirectory(external/X3)

# Sources and generate executable
file(GLOB_RECURSE SOURCES_LIB RELATIVE "${PROJECT_SOURCE_DIR}" "src/*.*")
add_executable(${PROJECT_NAME} ${SOURCES_LIB})
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
find_package(OpenGL REQUIRED) # Graphics
find_package(glfw3 REQUIRED) # GL wrapper
find_package(glad REQUIRED) # OpenGL loader
find_package(CUDA REQUIRED)

# Link all external libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw OpenGL::GL glad::glad OpenMP::OpenMP_CXX)
target_include_directories(${PROJECT_NAME} PUBLIC ${Stb_INCLUDE_DIR} "${CUDA_INCLUDE_DIRS}")

# Filters in visual studio
IF (MSVC)
	foreach(source IN LISTS SOURCES_LIB)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
ENDIF ()

# Enable GLM experimental features
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Link graphical library
target_link_libraries(${PROJECT_NAME} PUBLIC X3)
