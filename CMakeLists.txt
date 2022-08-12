# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("stringtests" VERSION 0.01)

include(CheckLanguage)
check_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER)
  enable_language(ASM_NASM)
else()
  message(AUTHOR_WARNING "No ASM_NASM support, disabling some tests")
endif()

################### OUTPUT DIRS ##############################

math(EXPR platform_bits "${CMAKE_SIZEOF_VOID_P} * 8")
set(platform_dir ${CMAKE_SYSTEM_NAME}${platform_bits}-${CMAKE_CXX_COMPILER_ID}${CMAKE_CXX_COMPILER_VERSION})

message(STATUS platform_dir =	${platform_dir})
message(STATUS CMAKE_BUILD_TYPE =	${CMAKE_BUILD_TYPE})
message(STATUS "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}${platform_bits}")
message(STATUS DCMAKE_TOOLCHAIN_FILE = ${DCMAKE_TOOLCHAIN_FILE})

# Default to c++17 mininimum for linux (string_view), works on mscv in cmake > 3.10
# need c++20 for ranges tests
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${platform_bits}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${platform_bits}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${platform_bits}")

#add_compile_definitions(ENABLE_BOOST_TESTS=0)

# Include sub-projects.
add_subdirectory ("stringtests")