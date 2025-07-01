add_library(spsc_queue INTERFACE)

add_executable(spsc_queue.test)
target_link_libraries(spsc_queue.test
PRIVATE
  project_utilities
  gtest
  gtest_main
)

add_subdirectory(single_threaded)
