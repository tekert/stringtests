# CMakeList.txt : CMake project for stringtests, include source and define
# project specific logic here.
#
# Add source to this project's executable.
add_executable (stringtests "stringtests.cpp" "stringtests.h")

# MSVC: link with dynamic standard libs
set_property(TARGET stringtests PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

#set_property(TARGET stringtests PROPERTY CXX_STANDARD 17)
#if (CMAKE_VERSION VERSION_GREATER 3.10)
#  set_property(TARGET stringtests PROPERTY CXX_STANDARD 20)
#endif()

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(stringtests PRIVATE benchmark::benchmark benchmark::benchmark_main)

# Find boost headers (any version)
find_package(Boost)
if ( Boost_FOUND )
    target_compile_definitions(stringtests PUBLIC ENABLE_BOOST_TESTS=1)
    target_include_directories(stringtests PRIVATE ${Boost_INCLUDE_DIRS})
else()
    target_compile_definitions(stringtests PUBLIC ENABLE_BOOST_TESTS=0)
    message( AUTHOR_WARNING "Boost not Found, disabling some tests" )
endif()

# Test messages
foreach(Component ${BoostComponents})
  string(TOUPPER "${Component}" ComponentUpper)
  message(STATUS "${ComponentUpper}: ${Boost_${ComponentUpper}_FOUND} - ${Boost_${ComponentUpper}_LIBRARY}")
endforeach()

# TODO: Add tests and install targets if needed.
