# CMakeList.txt : CMake project for stringtests, include source and define
# project specific logic here.
#
# Add source to this project's executable.
if(CMAKE_ASM_NASM_COMPILER)
  add_executable(stringtests_asm "stringtests.cpp" "stringtests.h" "nasm.asm" "cstringtest.c")
endif()
add_executable(stringtests "stringtests.cpp" "stringtests.h" "cstringtest.c")

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} COMPILER_NAME)
set_target_properties(stringtests_asm PROPERTIES OUTPUT_NAME "stringtests_asm_${COMPILER_NAME}")
set_target_properties(stringtests PROPERTIES OUTPUT_NAME "stringtests_${COMPILER_NAME}")

# MSVC: link with dynamic standard libs
set_property(TARGET stringtests_asm stringtests PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# set_property(TARGET stringtests PROPERTY CXX_STANDARD 17)
# if (CMAKE_VERSION VERSION_GREATER 3.10)
# set_property(TARGET stringtests PROPERTY CXX_STANDARD 20)
# endif()

# Benchmark Google lib
find_package(benchmark CONFIG REQUIRED)
target_link_libraries(stringtests_asm PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_link_libraries(stringtests PRIVATE benchmark::benchmark benchmark::benchmark_main)

# Find boost headers (any version)
find_package(Boost QUIET)

if(Boost_FOUND)
target_compile_definitions(stringtests_asm PUBLIC ENABLE_BOOST_TESTS=1)
target_compile_definitions(stringtests PUBLIC ENABLE_BOOST_TESTS=1)
target_include_directories(stringtests_asm PRIVATE ${Boost_INCLUDE_DIRS})
else()
target_compile_definitions(stringtests_asm PUBLIC ENABLE_BOOST_TESTS=0)
target_compile_definitions(stringtests PUBLIC ENABLE_BOOST_TESTS=0)
message(AUTHOR_WARNING "Boost not Found, disabling some tests")
endif()

# Doesnt compile on clang https://github.com/microsoft/vcpkg/issues/13945 (fix incoming 9/jun/23)
find_package(absl CONFIG REQUIRED)
target_link_libraries(stringtests_asm PRIVATE absl::any absl::log absl::base absl::bits)
target_link_libraries(stringtests PRIVATE absl::any absl::log absl::base absl::bits)

# fmt lib
# find_package(fmt)
# if ( fmt_FOUND )
# target_compile_definitions(stringtests PUBLIC ENABLE_FMT_TESTS=1)
# target_link_libraries(stringtests PRIVATE fmt::fmt-header-only)
# else()
# target_compile_definitions(stringtests PUBLIC ENABLE_FMT_TESTS=0)
# message( AUTHOR_WARNING "fmt not Found, disabling some tests" )
# endif()

# Test messages
foreach(Component ${BoostComponents})
string(TOUPPER "${Component}" ComponentUpper)
message(STATUS "${ComponentUpper}: ${Boost_${ComponentUpper}_FOUND} - ${Boost_${ComponentUpper}_LIBRARY}")
endforeach()

# TODO: Add tests and install targets if needed.
