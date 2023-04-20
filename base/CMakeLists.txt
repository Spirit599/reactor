


set(base_SRCS
  Log.cc
  Timestamp.cc
)

add_library(base ${base_SRCS})
target_link_libraries(base boost_filesystem boost_log boost_log_setup pthread)

add_subdirectory(test)