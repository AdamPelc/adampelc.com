target_include_directories(spsc_queue
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(spsc_queue.test
PRIVATE
  queue_single_threaded.test.cpp
)