set(CLANG_WARNINGS
  -Wall
  -Wextra # reasonable and standard
  -Wshadow # warn the user if a variable declaration shadows one from a parent context
  -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
  -Wold-style-cast # warn for c-style casts
  -Wcast-align # warn for potential performance problem casts
  -Wunused # warn on anything being unused
  -Woverloaded-virtual # warn if you overload (not override) a virtual function
  -Wpedantic # warn if non-standard C++ is used
  -Wconversion # warn on type conversions that may lose data
  -Wsign-conversion # warn on sign conversions
  -Wnull-dereference # warn if a null dereference is detected
  -Wdouble-promotion # warn if float is implicit promoted to double
  -Wformat=2 # warn on security issues around functions that format output (ie printf)
  -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
)

set(GCC_WARNINGS
  ${CLANG_WARNINGS}
  -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
  -Wduplicated-cond # warn if if / else chain has duplicated conditions
  -Wduplicated-branches # warn if if / else branches have duplicated code
  -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
  -Wuseless-cast # warn if you perform a cast to the same type
  -Wsuggest-override # warn if an overridden member function is not marked 'override' or 'final'
)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(COMPILER_WARNINGS_CXX ${CLANG_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(COMPILER_WARNINGS_CXX ${GCC_WARNINGS})
else()
  message(FATAL_ERROR "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
endif()

message(STATUS "COMPILER_WARNINGS_CXX='${COMPILER_WARNINGS_CXX}'")

add_library(compiler_warnings INTERFACE)
target_compile_options(compiler_warnings
INTERFACE
  ${COMPILER_WARNINGS_CXX}
)
