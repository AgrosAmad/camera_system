cmake_minimum_required(VERSION 3.16)

add_definitions(-w)

# Set main names
set(PHX_LIB_DIR "${PROJECT_SOURCE_DIR}/external/silicon/lib64")
set(PHX_INC_DIR "${PROJECT_SOURCE_DIR}/external/silicon/include")
set(PROJECT_NAME CAM)

# Initialize
project(${PROJECT_NAME} LANGUAGES C CXX CUDA)

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

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

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
add_subdirectories_recursively("${PROJECT_SOURCE_DIR}/external/silicon")
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

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/silicon/common.c)

set_source_files_properties(${CMAKE_SOURCE_DIR}/src/silicon/common.c PROPERTIES LANGUAGE C)
    
#set_property(SOURCE ${CMAKE_SOURCE_DIR}/src/silicon/common.c PROPERTY LANGUAGE C)
#set(CMAKE_C_STANDARD 99)

# Linking Silicon SDK
target_link_libraries(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/external/silicon/lib64/libphxapi-x86_64.so"
)

# Make runtime find the .so without LD_LIBRARY_PATH:
set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "${PHX_LIB_DIR}")

# PHX SDK compile-time switches (Linux, 64-bit)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  _PHX_LINUX=1
  _PHX_64BIT=1        
  _PHX_BUILD_64BIT=1  
  _GNU_SOURCE         
)

