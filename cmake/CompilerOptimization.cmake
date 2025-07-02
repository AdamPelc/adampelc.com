if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(COMPILER_OPTIMIZATION_FLAGS_CXX
    -Ofast
  )
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Optimized")
  set(COMPILER_OPTIMIZATION_FLAGS_CXX
    -O2
  )
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(COMPILER_OPTIMIZATION_FLAGS_CXX
    -Og
    -g
  )
else()
  message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE selected: '${CMAKE_BUILD_TYPE}'")
endif()

message(STATUS "COMPILER_OPTIMIZATION_FLAGS_CXX='${COMPILER_OPTIMIZATION_FLAGS_CXX}'")

add_library(compiler_optimization INTERFACE)
target_compile_options(compiler_optimization
INTERFACE
  ${COMPILER_OPTIMIZATION_FLAGS_CXX}
)
