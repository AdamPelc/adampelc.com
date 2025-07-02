if(ASAN_ENABLED AND TSAN_ENABLED)
  message(FATAL_ERROR "Address and thread sanitizers can NOT be enabled at the same time: ASAN_ENABLED=${ASAN_ENABLED}, TSAN_ENABLED=${TSAN_ENABLED}")
endif()

if(ASAN_ENABLED OR TSAN_ENABLED OR UBSAN_ENABLED)
  set(SANITIZER_COMPILER_FLAGS_CXX
    -fno-omit-frame-pointer
    -g
  )
endif()

set(SANITIZER_LINKER_FLAGS_CXX "")
if(${ASAN_ENABLED})
  list(APPEND SANITIZER_COMPILER_FLAGS_CXX
    -fsanitize=address
  )
  list(APPEND SANITIZER_LINKER_FLAGS_CXX
    -fsanitize=address
  )
endif()

if(${TSAN_ENABLED})
  list(APPEND SANITIZER_COMPILER_FLAGS_CXX
    -fsanitize=thread
  )
  list(APPEND SANITIZER_LINKER_FLAGS_CXX
    -fsanitize=thread
  )
endif()

if(${UBSAN_ENABLED})
  list(APPEND SANITIZER_COMPILER_FLAGS_CXX
    -fsanitize=undefined
  )
  list(APPEND SANITIZER_LINKER_FLAGS_CXX
    -fsanitize=undefined
  )
endif()

message(STATUS "SANITIZER_COMPILER_FLAGS_CXX='${SANITIZER_COMPILER_FLAGS_CXX}'")
message(STATUS "SANITIZER_LINKER_FLAGS_CXX='${SANITIZER_LINKER_FLAGS_CXX}'")

add_library(sanitizers INTERFACE)
target_compile_options(sanitizers
INTERFACE
  ${SANITIZER_COMPILER_FLAGS_CXX}
)
target_link_options(sanitizers
INTERFACE
  ${SANITIZER_LINKER_FLAGS_CXX}
)
